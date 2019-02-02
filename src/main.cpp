#include "Windows.h"
#include <inttypes.h>
#include <assert.h>

typedef int8_t		int8;
typedef int16_t		int16;
typedef int32_t		int32;
typedef int64_t		int64;

typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;

typedef uint32_t	uint;

#include "math.h"
#include "geometry.h"
#include "camera.h"
#include "object.h"
#include "scene.h"
#include "render.h"
#include "threading.h"

#define WIDTH 1280
#define HEIGHT 768
#define THREAD_COUNT 8



bool running = true;
int frameCount = 0;
bool renderFinished = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BITMAPINFO bmpinfo = {0};
uint8_t * bitmap = nullptr;
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
	clientRect.right = WIDTH;
	clientRect.bottom = HEIGHT;
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

	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth = WIDTH;
	bmpinfo.bmiHeader.biHeight = -HEIGHT;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = 32;
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	bmpinfo.bmiHeader.biSizeImage = WIDTH * HEIGHT * 4;


	bitmap = new uint8_t[WIDTH * HEIGHT * 4];
	memset(bitmap, 0, WIDTH * HEIGHT * 4);
	uint32_t * row = (uint32_t*)bitmap;
	for(int y = 0; y < HEIGHT; ++y)
	{
		for(int x = 0; x < WIDTH; ++x)
		{
			float r, g, b, a;
			r = x < 640 ? 0.0f : 1.0f;
			g = y < 400 ? 0.0f : 1.0f;
			b = 0.0f;
			a = 1.0f;
			uint32_t color = RGBA32(r, g, b, a);
			*(row + x) = color;
		}
		row += WIDTH;
	}

	InitScene();

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

	bitmapHDR = new V4[WIDTH*HEIGHT];
	V4 * rowHDR = bitmapHDR;

	const int xSubdivs = 10;
	const int ySubdivs = 6;
	int bucketW = WIDTH / xSubdivs;
	int bucketH = HEIGHT / ySubdivs;
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
			job.x0 = xs * bucketW;
			job.x1 = xs * bucketW + bucketW;
			job.y0 = ys * bucketH;
			job.y1 = ys * bucketH + bucketH;
			job.spp = SAMPLES_PER_PIXEL;
			jobqueue.Push(job);
		}
	}


	AsyncTask taskpool[THREAD_COUNT];
	HANDLE threadpool[THREAD_COUNT];
	for(int i = 0; i < THREAD_COUNT; ++i)
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
	}

#if 0
	for(int y = 0; y < HEIGHT; ++y)
	{
		for(int x = 0; x < WIDTH; ++x)
		{
			V4 totalRadiance = {};

			for(int s = 0; s < SAMPLES_PER_PIXEL; ++s)
			{
				V2 sampleOffset = sampleGrid[SAMPLES_PER_PIXEL][s];
				float mpp = cam.filmWidth / WIDTH;
				V3 camRight = -Cross(cam.direction, cam.up);
				V3 target = cam.position + cam.direction*cam.focalLength + camRight*(x - WIDTH/2 + sampleOffset.x)*mpp + -cam.up*(y - HEIGHT/2 + sampleOffset.y)*mpp;
				V3 dir = Normalize(target - cam.position);

				Ray ray = {0};
				ray.o = cam.position;
				ray.d = dir;


				// albedo = ix.object.material

				//V4 diffuse = ComputeRadianceForRay(ray, &scene);
				//V4 reflection = ComputeRadianceForRay(reflectionRay, &scene);

				Intersection ix;
				Object * io = nullptr;
				Light * light = &scene.lights[0];

				if(TraceRay(ray, &scene, &ix, &io))
				{
					V3 toLight = Normalize(light->position - ix.point);
					float lightDistanceSq = LengthSq(light->position - ix.point);
					V3 toCam = -dir;

					// diffuse
					float ndl = fmaxf(0, Dot(ix.normal, toLight));
					V4 diffuseRadiance = io->material.diffuse * (light->intensity / lightDistanceSq);

					// shadow
					Ray shadowRay = {ix.point, toLight};
					Intersection shadowIx;
					float shadowFactor = 1.0f; // fully lit
					if(TraceRay(shadowRay, &scene, &shadowIx, 0))
					{
						if(shadowIx.t*shadowIx.t < lightDistanceSq)
						{
							shadowFactor = 0.0f;
						}
					}

					// reflection

					V3 reflectionVector = Normalize(Reflect(ray.d, ix.normal));
					Ray reflectionRay = {ix.point, reflectionVector};
					int reflectionDepth = 0;

					V4 reflectedRadiance = TraceReflectionPath(reflectionRay, &scene, reflectionDepth);

					//shadowFactor = 1.0f;
					totalRadiance = ComponentAdd(totalRadiance, Lerp(diffuseRadiance*shadowFactor*ndl, io->material.exponent, reflectedRadiance));
				}
			}

			*(rowHDR + x) = totalRadiance / SAMPLES_PER_PIXEL;
		}
		rowHDR += WIDTH;
	}
#endif
	renderFinished = true;

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

#if 0
	row = (uint32_t*)bitmap;
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
				sample.r = (float)pow(sample.r, 0.45);
				sample.g = (float)pow(sample.g, 0.45);
				sample.b = (float)pow(sample.b, 0.45);
			}
			uint32_t color = RGBA32(sample.r, sample.g, sample.b, sample.a);
			*(row + x) = color;
		}
		row += WIDTH;
	}
#endif

	InvalidateRect(window, &clientRect, false);

	double elapsedS = 0.0;
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq); // counts per sec
	LARGE_INTEGER now;
	LARGE_INTEGER last;
	QueryPerformanceCounter(&last);

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


		QueryPerformanceCounter(&now);
		long long elapsedCounts = now.QuadPart - last.QuadPart;
		elapsedS += (double)elapsedCounts / freq.QuadPart;

		if(elapsedS > 1.0)
		{
			elapsedS -= 1.0;
		}
		InvalidateRect(window, &clientRect, false);
		++frameCount;

		double deltams = (double)elapsedCounts / freq.QuadPart / 1000.0;
		if(deltams < 16.6667)
		{
			Sleep((DWORD)(16.6667 - deltams));
		}

		last = now;
	}

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
			dc = BeginPaint(hwnd, &ps);
			//char buffer[256];
			//int c = wsprintf(buffer, "Hello, %i", frameCount);
			//TextOut(dc, 0, 0, buffer, c);
			if(renderFinished)
			{
				uint32_t * row = (uint32_t*)bitmap;
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
						uint32_t color = RGBA32(sample.r, sample.g, sample.b, sample.a);
						*(row + x) = color;
					}
					row += WIDTH;
				}
			}
			StretchDIBits(dc, 0, 0, WIDTH, HEIGHT,
							0, 0, WIDTH, HEIGHT,
							bitmap, &bmpinfo, DIB_RGB_COLORS, SRCCOPY);
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
