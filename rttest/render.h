#pragma once

#define MAX_REFLECTION_DEPTH 10
#define SAMPLES_PER_PIXEL 4


V2 sampleGrid[][8] = {
	{},
	// 1 spp
	{
		{0.5f, 0.5f}
	},
	{},
	{},
	// 4 spp
	{
		{0.375f, 0.125f},
		{0.875f, 0.375f},
		{0.625f, 0.875f},
		{0.125f, 0.625f},
	},
	{},
	{},
	{},
	// 8 spp
	{
	},

};

#define RGBA32(r, g, b, a)  ((((int)(a*255) & 0xff) << 24) |		\
							 (((int)(r*255) & 0xff) << 16) |		\
							 (((int)(g*255) & 0xff) << 8)  |		\
							 (((int)(b*255) & 0xff) << 0))

void PutPixel(V4 * bitmap, int x, int y, V4 color)
{
	bitmap[y*1280 + x] = color;
}

V4 Schlick(V4 rf0, float theta)
{
	V4 reflectance = rf0 + (V4{1.0f, 1.0f, 1.0f, 1.0f} - rf0) * pow(1 - max(0, cos(theta)), 5);
	return reflectance;
}

bool TraceRay(Ray r, Scene * s, Intersection * out_ix, Object ** out_io)
{
	bool result = false;
	Intersection ix;
	Object * io = nullptr;
	ix.t = 1000000000;
	bool intersectionFound = false;

	for(unsigned int i = 0; i < s->objectCount; ++i)
	{
		Object * o = &s->objects[i];
		Intersection intermix;
		if(Intersect(r, o->geometry, &intermix))
		{
			if(intermix.t < ix.t)
			{
				intersectionFound = true;
				ix = intermix;
				io = o;
			}
		}
	}

	if(intersectionFound)
	{
		result = true;
		if(out_ix)
		{
			*out_ix = ix;
		}
		if(out_io)
		{
			*out_io = io;
		}
	}

	return result;
}

V4 TraceReflectionPath(Ray ray, Scene * scene, int depth)
{
	V4 radiance = {0.0f, 0.0f, 0.0f, 0.0f};

	Intersection ix = {};
	Object * io = 0;
	Light * light = &scene->lights[0];

	if(TraceRay(ray, scene, &ix, &io))
	{
		if(io->material.reflectivity == 0)
		{
			V3 toLightRefl = Normalize(light->position - ix.point);
			float reflLightDistanceSq = LengthSq(light->position - ix.point);
			float reflNdL = max(0, Dot(ix.normal, toLightRefl));

			Ray reflShadowRay = {ix.point, toLightRefl};
					
			Intersection shadowIx;
			float reflShadowFactor = 1.0f; // fully lit
			if(TraceRay(reflShadowRay, scene, &shadowIx, 0))
			{
				if(shadowIx.t*shadowIx.t < LengthSq(light->position - ix.point))
				{
					reflShadowFactor = 0.0f;
				}
			}

			V4 reflRadiance = ComponentMultiply(io->material.diffuse / PI, (light->color * light->intensity / reflLightDistanceSq) * reflNdL);
			reflRadiance = reflRadiance * reflShadowFactor;
			radiance = reflRadiance;
			//radiance = {1.0f, 0.0f, 0.0f, 0.0f};
			//Lerp(diffuseRadiance*shadowFactor*ndl, io->material.exponent, reflectedRadiance*ndr/*?*/);
		}
		else
		{
			V3 reflectionVector = Reflect(ray.d, ix.normal);
			Ray reflectionRay = {ix.point, reflectionVector};
			if(depth < MAX_REFLECTION_DEPTH)
			{
				radiance = TraceReflectionPath(reflectionRay, scene, depth + 1);
				//radiance = {1.0f, 0.0f, 0.0f, 0.0f};
			}
		}
	}
	return radiance;
}