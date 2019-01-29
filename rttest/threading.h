#pragma once

#define MAX_RENDER_JOBS 10*6

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

void PerformRenderJob(RenderJob * job)
{
	//V4 * rowHDR = job->bitmap;
	for(int y = job->y0; y < job->y1; ++y)
	{
		for(int x = job->x0; x < job->x1; ++x)
		{
			V4 totalRadiance = {};

			for(int s = 0; s < job->spp; ++s)
			{
				V2 sampleOffset = sampleGrid[job->spp][s];
				float mpp = job->camera->filmWidth / job->viewportWidth;
				V3 camRight = -Cross(job->camera->direction, job->camera->up);
				V3 target = job->camera->position + job->camera->direction*job->camera->focalLength + camRight*(x - job->viewportWidth/2 + sampleOffset.x)*mpp + -job->camera->up*(y - job->viewportHeight/2 + sampleOffset.y)*mpp;
				V3 dir = Normalize(target - job->camera->position);
		
				Ray ray = {0};
				ray.o = job->camera->position;
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
					//V4 diffuseRadiance = io->material.diffuse * (light->intensity / lightDistanceSq);

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


					V4 diffuseRadiance = ComponentMultiply(io->material.diffuse / PI, (light->color * light->intensity / lightDistanceSq) * ndl);
					V4 reflectedRadiance = TraceReflectionPath(reflectionRay, &scene, reflectionDepth);

					//shadowFactor = 1.0f;
					totalRadiance = ComponentAdd(totalRadiance, Lerp(diffuseRadiance*shadowFactor*ndl, io->material.reflectivity, reflectedRadiance));
				}
			}

			totalRadiance = totalRadiance / job->spp;
			//*(rowHDR + x) = 
			PutPixel(job->bitmap, x, y, totalRadiance);
		}
		//rowHDR += job->viewportWidth;
	}
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