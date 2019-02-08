#pragma once

#define SAMPLES_PER_PIXEL 1
#define MAX_REFLECTION_DEPTH 1
#define MAX_DIFFUSE_BOUNCES 1
#define SECONDARY_RAYS 32


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
// PROFILED_FUNCTION;
	bitmap[y*1280 + x] = color;
}

V4 Schlick(V4 rf0, float cosTheta)
{
	V4 reflectance = rf0 + (V4::FromFloat(1.0f) - rf0) * pow(1 - max(0, cosTheta), 5);
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

V4 ComputeRadiance(Ray ray, Scene * scene, int depth, int bounce)
{
	V4 radiance = {};
	Intersection ix;
	Object * io = nullptr;

	Material matGray;
	matGray.diffuse = {0.8f, 0.8f, 0.8f, 1.0f};
	matGray.rf0 = V4::FromFloat(0.001f);
	matGray.isConductor = false;

	if(TraceRay(ray, scene, &ix, &io))
	{

		// V3 toCam = -ray.d;
		Material * mat = &io->material;
#if 0
		mat = &matGray;
#endif

		// diffuse
		V4 diffuseRadiance = {};

		// indirect
		if(bounce < MAX_DIFFUSE_BOUNCES/* && ray.d.y < 0*/)
		{
			Ray secondaryRays[SECONDARY_RAYS];
			for(int i = 0; i < SECONDARY_RAYS; ++i)
			{
				V3 diffDir = RandomDirectionOnHemisphere(ix.normal);
				secondaryRays[i] = {ix.point, diffDir};
			}

			for(int i = 0; i < SECONDARY_RAYS; ++i)
			{
				V4 sampledRadiance = ComputeRadiance(secondaryRays[i], scene, depth, bounce+1);
				float cosTheta = Dot(ix.normal, secondaryRays[i].d);
				diffuseRadiance += sampledRadiance * cosTheta;
			}

			diffuseRadiance = ComponentMultiply(mat->diffuse / PI, diffuseRadiance);
			diffuseRadiance = diffuseRadiance / SECONDARY_RAYS;
		}

		// direct
		{
			for(uint li = 0; li < scene->lightCount; ++li)
			{
				Light * light = &scene->lights[li];
				V3 toLight = Normalize(light->position - ix.point);
				float lightDistanceSq = LengthSq(light->position - ix.point);
				if(!mat->isConductor)
				{
					float ndl = fmaxf(0, Dot(ix.normal, toLight));
					//V4 diffuseRadiance = mat->diffuse * (light->intensity / lightDistanceSq);

					// shadow
					Ray shadowRay = {ix.point, toLight};
					Intersection shadowIx;
					float shadowFactor = 1.0f; // fully lit
					if(TraceRay(shadowRay, scene, &shadowIx, 0))
					{
						if(shadowIx.t*shadowIx.t < lightDistanceSq)
						{
							shadowFactor = 0.0f;
						}
					}

					diffuseRadiance += shadowFactor * ComponentMultiply(mat->diffuse / PI, (light->color * light->intensity / lightDistanceSq) * ndl);
				}
			}
		}


		// reflection
		V4 reflectedRadiance = {};
		V4 specularReflectance = {};

		if(depth < MAX_REFLECTION_DEPTH)
		{
			V3 reflectionVector = Normalize(Reflect(ray.d, ix.normal));
			Ray reflectionRay = {ix.point, reflectionVector};
			float cosTheta = Dot(ix.normal, reflectionVector);
			specularReflectance = Schlick(mat->rf0, cosTheta);
			reflectedRadiance = cosTheta * ComputeRadiance(reflectionRay, scene, depth + 1, bounce);
		}

		radiance = io->material.emissive*io->material.power + ComponentMultiply(V4::FromFloat(1.0f) - specularReflectance, diffuseRadiance) + ComponentMultiply(specularReflectance, reflectedRadiance);
	}

	return radiance;
}


