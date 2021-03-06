#pragma once


#define MAX_RENDER_JOBS 20*12

struct RenderJob
{
	Scene * scene;
	Camera * camera;
	int x0, x1, y0, y1;
	V4 * bitmap;
	int viewportWidth;
	int viewportHeight;
	int spp;
};

struct JobQueue
{
	RenderJob jobs[MAX_RENDER_JOBS];
	LONG jobCount = 0;

	void Push(RenderJob job)
	{
		jobs[jobCount] = job;
		jobCount++;
	}
};

struct AsyncTask
{
	int threadId;
	DWORD systemId;
	JobQueue * jobQueue;
};

// struct List

void PerformRenderJob(RenderJob * job)
{
PROFILED_FUNCTION;
	uint tid = GetCurrentThreadId();
	gPerThreadRng[LOCAL_THREAD_ID] = RNG(job->y0 * 11239 + job->x0);
	float mpp = job->camera->filmWidth / job->viewportWidth;

	V2i * pixels = new V2i[(job->y1 - job->y0) * (job->x1 - job->x0)];
	int pixelCount = 0;
	for(int y = job->y0; y < job->y1; ++y)
	{
		for(int x = job->x0; x < job->x1; ++x)
		{
			pixels[pixelCount++] = V2i{x, y};
		}
	}

	for(int i = 0; i < pixelCount*2; ++i)
	{
		int index1 = (int)Random(0.0f, (float)pixelCount);
		int index2 = (int)Random(0.0f, (float)pixelCount);
		V2i temp = pixels[index1];
		pixels[index1] = pixels[index2];
		pixels[index2] = temp;
	}

	//V4 * rowHDR = job->bitmap;
	for(int i = 0; i < pixelCount; ++i)
	{
		int x = pixels[i].x;
		int y = pixels[i].y;
		V4 outgoingRadiance = {};

		for(int s = 0; s < job->spp; ++s)
		{
			V2 sampleOffset = sampleGrid[job->spp][s];
			V3 camRight = -Cross(job->camera->direction, job->camera->up);
			V3 target = job->camera->position + job->camera->direction*job->camera->focalLength + camRight*(x - job->viewportWidth/2 + sampleOffset.x)*mpp + -job->camera->up*(y - job->viewportHeight/2 + sampleOffset.y)*mpp;
			V3 dir = Normalize(target - job->camera->position);

			Ray ray = {0};
			ray.o = job->camera->position;
			ray.d = dir;

			V4 sampleRadiance = ComputeRadiance(ray, &scene, 0, 0);
			outgoingRadiance = outgoingRadiance + sampleRadiance;
		}

		outgoingRadiance = outgoingRadiance / (float)job->spp;
		//*(rowHDR + x) =
		PutPixel(job->bitmap, x, y, outgoingRadiance);
	}

	delete[] pixels;
}

DWORD WINAPI RenderThreadFunc(LPVOID param)
{
	AsyncTask * task = (AsyncTask*)param;

	char buffer[256];
	wsprintf(buffer, "Thread %d started.\n", task->threadId);
	OutputDebugString(buffer);

	bool hasTasks = true;
	while(hasTasks)
	{
		LONG jobIndex = InterlockedDecrement(&task->jobQueue->jobCount);
		if(jobIndex > 0)
		{
			char buffer1[256];
			wsprintf(buffer1, "Thread %d taking job %d.\n", task->threadId, jobIndex);
			OutputDebugString(buffer1);

			RenderJob * job = &task->jobQueue->jobs[jobIndex];
			PerformRenderJob(job);
		}
		else
		{
			hasTasks = false;
		}
	}

	char buffer2[256];
	wsprintf(buffer2, "Thread %d (0x%x) finished.\n", task->threadId, task->systemId);
	OutputDebugString(buffer2);
	return 0;
}
