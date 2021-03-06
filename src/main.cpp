//#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include <inttypes.h>
#include <assert.h>
#include <random>

typedef int8_t		int8;
typedef int16_t		int16;
typedef int32_t		int32;
typedef int64_t		int64;

typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;

typedef uint32_t	uint;

#define RENDER_THREAD_COUNT 8
#define PROGRAM_THREAD_COUNT RENDER_THREAD_COUNT + 1
uint8 gThreadCounter = 0;
uint8 gThreadIdMap[1<<16];

#define WIDTH (1280/2)
#define HEIGHT (768/2)
#define SAMPLE_VIEWER 0

struct RNG
{
	RNG(int seed = 1147987)
	{
		generator = std::mt19937(seed);
	}

	float Next(float min, float max)
	{
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(generator);
	}

	std::mt19937 generator;
};

RNG gPerThreadRng[PROGRAM_THREAD_COUNT];

#define LOCAL_THREAD_ID (gThreadIdMap[GetCurrentThreadId()])
// #define LOCAL_THREAD_ID 0

#include "profile.h"
#include "math.h"
#include "geometry.h"
#include "sampler.h"
#include "camera.h"
#include "object.h"
#include "scene.h"
#include "render.h"
#include "threading.h"



bool running = true;
int frameCount = 0;
bool renderStarted = false;
bool renderFinished = false;
double renderTime;

HFONT fontMono;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BITMAPINFO bmpinfo = {0};
uint32 * bitmap = nullptr;
V4 * bitmapHDR = nullptr;



V4 ComputeRadianceForRay(Ray r, Scene * s)
{
	return V4{};
}

int __stdcall WinMain(HINSTANCE inst, HINSTANCE pinst, LPSTR cmdline, int cmdshow)
{
	UNREFERENCED_PARAMETER(pinst);
	UNREFERENCED_PARAMETER(cmdline);

	WNDCLASSEX windowClass = {0};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
	windowClass.hInstance = inst;
	windowClass.hIcon = LoadIcon(inst, MAKEINTRESOURCE(IDI_APPLICATION));
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "RTWndClass";

	if(!RegisterClassEx(&windowClass))
	{
		OutputDebugStringA("Failed to register window class!");
		return 1;
	}

	RECT clientRect, desktopRect;
	HWND desktop = GetDesktopWindow();
	GetWindowRect(desktop, &desktopRect);
	clientRect.left = 10;
	clientRect.top = 30;
	clientRect.right = WIDTH + 10;
	clientRect.bottom = HEIGHT + 30;
	AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, FALSE);

	HWND window = CreateWindow("RTWndClass", "RT", WS_OVERLAPPEDWINDOW,
								clientRect.left, clientRect.top, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
								NULL, NULL, inst, NULL );
	if( !window ) {
		OutputDebugStringA("Failed to create window!");
		return 1;
	}

	ShowWindow(window, cmdshow);
	UpdateWindow(window);

	fontMono = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "Droid Sans Mono");

	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth = WIDTH;
	bmpinfo.bmiHeader.biHeight = -HEIGHT;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = 32;
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	bmpinfo.bmiHeader.biSizeImage = WIDTH * HEIGHT * 4;


	bitmap = new uint32[WIDTH * HEIGHT];
	memset(bitmap, 0, WIDTH * HEIGHT * sizeof(uint32));
#if 0
	uint32 * row = bitmap;
	for(int y = 0; y < HEIGHT; ++y)
	{
		for(int x = 0; x < WIDTH; ++x)
		{
			float r, g, b, a;
			r = x < WIDTH/2 ? 0.0f : 1.0f;
			g = y < HEIGHT/2 ? 0.0f : 1.0f;
			b = 0.0f;
			a = 1.0f;
			uint32 color = RGBA32(r, g, b, a);
			*(row + x) = color;
		}
		row += WIDTH;
	}
#endif

	InitProfiler();
	InitScene();

	gThreadIdMap[GetCurrentThreadId()] = gThreadCounter++;


	// Left-handed, +X is front, +Y is right, +Z is up
	Camera cam;
	cam.position = {-10.0f, 0.0f, 3.0f};
	cam.direction = {1, 0, 0};
	cam.up = {0, 0, 1};
	cam.filmWidth = 0.35f;
	cam.focalLength = 0.28f;

#if 0
	cam.position.y = 1.0f;
	cam.position.z = 0.75f;
	cam.focalLength = 0.55f;
#endif

#if 0
	cam.position.y = 0.0f;
	cam.position.z = 3.0f;
	cam.position.x = 0.0f;
	cam.direction = Normalize(V3{0.2f, 1.0f, 0.0f});
	cam.focalLength = 0.35f;
#endif


#if !SAMPLE_VIEWER
	bitmapHDR = new V4[WIDTH*HEIGHT];
	memset(bitmapHDR, 0, sizeof(V4)*WIDTH*HEIGHT);
	V4 * rowHDR = bitmapHDR;

	const uint bucketWidth = 64;
	const int xSubdivs = WIDTH % bucketWidth == 0 ? WIDTH / bucketWidth : WIDTH / bucketWidth + 1;
	const int ySubdivs = HEIGHT % bucketWidth == 0 ? HEIGHT / bucketWidth : HEIGHT / bucketWidth + 1;
	JobQueue jobqueue;

	int jobIndex = 0;
	for(int xs = 0; xs < xSubdivs; ++xs)
	{
		for(int ys = 0; ys < ySubdivs; ++ys)
		{
			RenderJob job = {};
			job.scene = &scene;
			job.bitmap = bitmapHDR;
			job.camera = &cam;
			job.viewportWidth = WIDTH;
			job.viewportHeight = HEIGHT;
			if(xs == xSubdivs - 1)
			{
				job.x0 = xs * bucketWidth;
				job.x1 = WIDTH;
			}
			else
			{
				job.x0 = xs * bucketWidth;
				job.x1 = xs * bucketWidth + bucketWidth;
			}

			if(ys == ySubdivs - 1)
			{
				job.y0 = ys * bucketWidth;
				job.y1 = HEIGHT;
			}
			else
			{
				job.y0 = ys * bucketWidth;
				job.y1 = ys * bucketWidth + bucketWidth;
			}
			job.spp = SAMPLES_PER_PIXEL;
			jobqueue.Push(job);
		}
	}

	for(int a = 0; a < jobqueue.jobCount; ++a)
	{
		bool swapped = false;
		for(int b = 0; b < jobqueue.jobCount - 1; ++b)
		{
			float scoreA = LengthSq(V2{(float)jobqueue.jobs[b].x0, (float)jobqueue.jobs[b].y0} - V2{WIDTH/2, HEIGHT/2});
			float scoreB = LengthSq(V2{(float)jobqueue.jobs[b + 1].x0, (float)jobqueue.jobs[b + 1].y0} - V2{WIDTH/2, HEIGHT/2});
			if(scoreA < scoreB)
			{
				RenderJob job = jobqueue.jobs[b];
				jobqueue.jobs[b] = jobqueue.jobs[b + 1];
				jobqueue.jobs[b + 1] = job;
				swapped = true;
			}
		}
		if(!swapped)
			break;
	}


	AsyncTask taskpool[RENDER_THREAD_COUNT];
	HANDLE threadpool[RENDER_THREAD_COUNT];

	renderStarted = true;
	uint64 renderStartTime = GetHiresTime();

	for(int i = 0; i < RENDER_THREAD_COUNT; ++i)
	{
		taskpool[i].threadId = i;
		taskpool[i].jobQueue = &jobqueue;
		threadpool[i] = CreateThread(
			NULL,
			0,
			RenderThreadFunc,
			&taskpool[i],
			0,
			&taskpool[i].systemId
		);
		gThreadIdMap[taskpool[i].systemId] = gThreadCounter++;
	}
#else

#endif

#if 0
	row = (uint32_t*)bitmap;
	float totalLuminance = 0.0f;
	float maxLuminance = 0.0f;
	for(int y = 0; y < HEIGHT; ++y)
	{
		for(int x = 0; x < WIDTH; ++x)
		{
			V4 sample = bitmapHDR[y*WIDTH + x];
			float luminance = 0.2126f*sample.r + 0.7152f*sample.g + 0.0722f*sample.b;
			totalLuminance += luminance;
			if(luminance > maxLuminance) maxLuminance = luminance;
		}
	}

	float avgLuminance = totalLuminance / (WIDTH*HEIGHT);
	float exposure = 0.16f;
	float toneControl = exposure / avgLuminance;
#endif

	InvalidateRect(window, &clientRect, false);

	double elapsedS = 0.0;
	double deltams = 0.0;
	float angle = 0.0f;
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq); // counts per sec
	uint64 countsPerSec = freq.QuadPart;
	LARGE_INTEGER now;
	LARGE_INTEGER last;
	QueryPerformanceCounter(&last);

#if SAMPLE_VIEWER
	const uint requestedSampleCount = 100;
	V3 samples[requestedSampleCount];
	memset(samples, 0, sizeof(V3)*requestedSampleCount);
	//const uint sampleCount = GetRandomSamplesOnDisk(requestedSampleCount, samples);
	//const uint sampleCount = GetRandomSamplesInUnitCube(requestedSampleCount, samples);
	//const uint sampleCount = GetUniformSamplesOnSquare(requestedSampleCount, samples);
	//const uint sampleCount = GetUniformSamplesOnDisk(requestedSampleCount, samples);
	//const uint sampleCount = GetRandomSamplesOnHemisphere(requestedSampleCount, samples);
	//const uint sampleCount = GetUniformSamplesOnHemisphere(requestedSampleCount, samples);
	//const uint sampleCount = GetJitteredSamplesOnSquare(requestedSampleCount, samples);
	//const uint sampleCount = GetJitteredSamplesOnDisk(requestedSampleCount, samples);
	const uint sampleCount = GetJitteredSamplesOnHemisphere(requestedSampleCount, samples);
	for(uint i = 0; i < sampleCount; ++i)
	{
		//samples[i] = RotateSample(samples[i], Normalize(V3{0.99f, 0.01f, 0}));
		//samples[i] = RotateSample(samples[i], Normalize(V3{0.01f, 0.99f, 0}));
		//samples[i] = RotateSample(samples[i], Normalize(V3{0.5f, 0.5f, 0}));
	}
#endif

	MSG msg = {0};
	while(running)
	{
		while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				running = false;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

#if !SAMPLE_VIEWER
		if(!renderFinished && WAIT_OBJECT_0 == WaitForMultipleObjects(RENDER_THREAD_COUNT, threadpool, true, 0))
		{
			uint64 renderEndTime = GetHiresTime();
			renderTime = (double)(renderEndTime - renderStartTime) / countsPerSec;
			renderFinished = true;
		}
#else
		memset(bitmap, 0, WIDTH*HEIGHT*sizeof(uint32));

		float aspect = (float)WIDTH / HEIGHT;
		float viewX = cos(DegToRad(angle));
		float viewY = sin(DegToRad(angle));
		angle += 16.6f * (10.0f / 1000.0f);

		M4x4 view[4];
		view[0] = M4x4::ViewRH(V3{0, 0, 0}, V3{0, 0, -1}, V3{0, 1, 0});
		view[1] = M4x4::ViewRH(V3{0, 0, 0}, V3{0, 1, 0}, V3{0, 0, 1});
		view[2] = M4x4::ViewRH(V3{0, 0, 0}, V3{-1, 0, 0}, V3{0, 0, 1});
		view[3] = M4x4::ViewRH(V3{0, 0, 0}, V3{viewX, viewY, 0}, V3{0, 0, 1});

		M4x4 proj[4];
		proj[0] = M4x4::OrthographicRH(2.2f*aspect, 2.2f, -5.0f, 5.0f);
		proj[1] = M4x4::OrthographicRH(2.2f*aspect, 2.2f, -5.0f, 5.0f);
		proj[2] = M4x4::OrthographicRH(2.2f*aspect, 2.2f, -5.0f, 5.0f);
		proj[3] = M4x4::OrthographicRH(2.2f*aspect, 2.2f, -5.0f, 5.0f);

		M4x4 vp[4];
		vp[0] = proj[0] * view[0];
		vp[1] = proj[1] * view[1];
		vp[2] = proj[2] * view[2];
		vp[3] = proj[3] * view[3];

		Viewport viewport[4];
		viewport[0] = {0, 0, WIDTH/2, HEIGHT/2};
		viewport[1] = {WIDTH/2 + 1, 0, WIDTH/2, HEIGHT/2};
		viewport[2] = {0, HEIGHT/2 + 1, WIDTH/2, HEIGHT/2};
		viewport[3] = {WIDTH/2 + 1, HEIGHT/2 + 1, WIDTH/2, HEIGHT/2};

		V3 min = V3{-1.0f, -1.0f, -1.0f};
		V3 max = V3{1.0f, 1.0f, 1.0f};
		min = vp[0] * min;
		max = vp[0] * max;
		int minx = (int)floor( (min.x*0.5f + 0.5f) * viewport[0].w + viewport[0].x);
		int miny = (int)floor(-(min.y*0.5f - 0.5f) * viewport[0].h + viewport[0].y);
		int maxx = (int)floor( (max.x*0.5f + 0.5f) * viewport[0].w + viewport[0].x);
		int maxy = (int)floor(-(max.y*0.5f - 0.5f) * viewport[0].h + viewport[0].y);

		for(int x = 0; x < WIDTH; ++x)
		{
			bitmap[(HEIGHT/4)*WIDTH + Clamp(x, 0, WIDTH/2)] = RGBA32(1.0f, 0.0f, 0.0f, 1.0f);
			bitmap[Clamp(x, 0, HEIGHT/2)*WIDTH + WIDTH/4] = RGBA32(0.0f, 1.0f, 0.0f, 1.0f);

			bitmap[miny*WIDTH + Clamp(x, minx, maxx)] = RGBA32(0.5f, 0.5f, 0.5f, 1.0f);
			bitmap[maxy*WIDTH + Clamp(x, minx, maxx)] = RGBA32(0.5f, 0.5f, 0.5f, 1.0f);
			bitmap[Clamp(x, maxy, miny)*WIDTH + minx] = RGBA32(0.5f, 0.5f, 0.5f, 1.0f);
			bitmap[Clamp(x, maxy, miny)*WIDTH + maxx] = RGBA32(0.5f, 0.5f, 0.5f, 1.0f);
		}

		for(uint viewIndex = 0; viewIndex < 4; viewIndex++)
		{
			for(uint i = 0; i < sampleCount; ++i)
			{
				V3 projectedSample = vp[viewIndex]*samples[i];
				int x = (int)floor( (projectedSample.x*0.5f + 0.5f) * viewport[viewIndex].w + viewport[viewIndex].x);
				int y = (int)floor(-(projectedSample.y*0.5f - 0.5f) * viewport[viewIndex].h + viewport[viewIndex].y);
				x = Clamp(x, 0, WIDTH);
				y = Clamp(y, 0, HEIGHT);
				uint32 color = RGBA32(1.0f, 1.0f, 1.0f, 1.0f);
				bitmap[y * WIDTH + x] = color;
			}
		}
#endif

		QueryPerformanceCounter(&now);
		long long elapsedCounts = now.QuadPart - last.QuadPart;
		elapsedS += (double)elapsedCounts / countsPerSec;

		if(elapsedS > 1.0)
		{
			elapsedS -= 1.0;
		}
		InvalidateRect(window, &clientRect, false);
		++frameCount;

		deltams = (double)elapsedCounts / countsPerSec / 1000.0;
		if(deltams < 16.6667)
		{
			Sleep((DWORD)(16.6667 - deltams));
		}

		last = now;
	}

	DeleteObject(fontMono);
	delete[] vb;
	delete[] bitmapHDR;
	delete[] bitmap;
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps = {0};
	HDC dc = {0};

	switch(message)
	{
		case WM_PAINT:
		{
			if(renderStarted)
			{
				uint32 * row = bitmap;
				for(int y = 0; y < HEIGHT; ++y)
				{
					for(int x = 0; x < WIDTH; ++x)
					{
						V4 sample = bitmapHDR[y*WIDTH + x];

						// tone mapping
						float luminance = 0.2126f*sample.r + 0.7152f*sample.g + 0.0722f*sample.b;
						float whitepoint = 0.6f;
						//if(x < WIDTH/2)
						{
							sample = ComponentDivide(ComponentDivide(sample, (sample + V4{1, 1, 1, 1})*whitepoint), V4{whitepoint, whitepoint, whitepoint, whitepoint} + V4{1, 1, 1, 1});
						}
						sample = Saturate(sample);

						// gamma
						//if(x < WIDTH/2)
						{
							sample.r = (float)pow(sample.r, 0.45f);
							sample.g = (float)pow(sample.g, 0.45f);
							sample.b = (float)pow(sample.b, 0.45f);
						}
						uint32 color = RGBA32(sample.r, sample.g, sample.b, sample.a);
						*(row + x) = color;
					}
					row += WIDTH;
				}
			}

			dc = BeginPaint(hwnd, &ps);

			StretchDIBits(dc, 0, 0, WIDTH, HEIGHT,
							0, 0, WIDTH, HEIGHT,
							bitmap, &bmpinfo, DIB_RGB_COLORS, SRCCOPY);

			char buf[4096];
			int len = PrintProfile(buf, 4096);

			SetBkMode(dc, TRANSPARENT);
			SetTextColor(dc, RGB(0, 255, 255));
			SelectObject(dc, fontMono);
			DrawText(dc, buf, len, &ps.rcPaint, DT_LEFT | DT_TOP | DT_EXPANDTABS);
			if(renderFinished)
			{
				char buf2[256];
				int len2 = _snprintf(buf2, 256, "Rendering finished in %.2fs", renderTime);
				// RECT statusRect = ps.rcPaint;
				// statusRect.top = statusRect.bottom - 30;
				DrawText(dc, buf2, len2, &ps.rcPaint, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_EXPANDTABS);
			}
			//TextOut(dc, 0, 0, buf, len);
			EndPaint(hwnd, &ps);

		}	break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}	break;
		default:
		{
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
	}
	return 0;
}
