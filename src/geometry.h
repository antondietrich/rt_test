#pragma once

#include <math.h>



inline float Saturate(float v)
{
	float result = min(max(v, 0), 1);;
	return result;
}



inline V4 Saturate(V4 v)
{
	V4 result = v;
	result.x = min(max(result.x, 0), 1);
	result.y = min(max(result.y, 0), 1);
	result.z = min(max(result.z, 0), 1);
	result.w = min(max(result.w, 0), 1);
	return result;
}


//////////////////////////////////////////////////////////////////////////
// Intersections

struct Ray
{
	V3 o;
	V3 d;
};

struct Sphere
{
	V3 o;
	float r;
};

struct Plane
{
	V3 p;
	V3 n;
};

struct Vertex
{
	V3 position;
	V3 normal;
};

struct Mesh
{
	int vertexCount;
	Vertex * vertices;
	Sphere bound;
};

enum GeoType
{
	SPHERE,
	PLANE,
	MESH,

	COUNT
};

struct Geometry
{
	GeoType type;
	union
	{
		Sphere sphere;
		Plane plane;
		Mesh mesh;
	};
};

struct Intersection
{
	V3 point;
	V3 normal;
	float t;
};

bool IntersectRaySphere(Ray ray, Sphere sphere, Intersection * intersection)
{
	bool result = false;
	float proj = Dot(sphere.o - ray.o, ray.d);
	float dist = Length(sphere.o - (ray.o + ray.d*proj));
	if(dist <= sphere.r)
	{
		float pen = (float)sqrt(sphere.r*sphere.r - dist*dist);
		float t1 = proj - pen;
		float t2 = proj + pen;
		float t = t1 < EPSYLON ? t2 : t1;
		if(t > EPSYLON)
		{
			result = true;
			if(intersection)
			{
				V3 point = ray.o + ray.d*t;
				(*intersection).t = t;
				(*intersection).point = point;
				(*intersection).normal = Normalize(point - sphere.o);
			}
		}
	}

	return result;
}

bool IntersectRayPlane(Ray ray, Plane plane, Intersection * intersection)
{
	bool result = false;
	float denom = Dot(ray.d, -plane.n);
	if(denom > EPSYLON)
	{
		float t = Dot(plane.p - ray.o, -plane.n) / denom;
		if(t >= 0)
		{
			result = true;
			if(intersection)
			{
				(*intersection).t = t;
				(*intersection).point = ray.o + ray.d*t;
				(*intersection).normal = plane.n;
			}
		}
	}

	return result;
}

bool IntersectRayMesh(Ray ray, Mesh mesh, Intersection * intersection)
{
	bool result = false;
	//return IntersectRaySphere(ray, mesh.bound, intersection);
	if(IntersectRaySphere(ray, mesh.bound, nullptr))
	{
		for(int i = 0; i < mesh.vertexCount; i += 3)
		{
			Vertex v0 = mesh.vertices[i + 0];
			Vertex v1 = mesh.vertices[i + 1];
			Vertex v2 = mesh.vertices[i + 2];
			V3 edge1 = v1.position - v0.position;
			V3 edge2 = v2.position - v0.position;
			V3 h = Cross(ray.d, edge2);
			float a = Dot(edge1, h);
			if(fabs(a) > EPSYLON)
			{
				float f = 1.0f/a;
				V3 s = ray.o - v0.position;
				float u = f*Dot(s, h);
				if(Saturate(u) == u)
				{
					V3 q = Cross(s, edge1);
					float v = f*Dot(ray.d, q);
					if(v >= 0.0f && u + v <= 1.0f)
					{
						float t = f*Dot(edge2, q);
						// TODO: add this test to all intersections, remove ray origin shifting from main
						if(t > EPSYLON)
						{
							result = true;
							if(intersection)
							{
								(*intersection).t = t;
								(*intersection).point = ray.o + ray.d*t;
								(*intersection).normal = v0.normal;
							}
						}
					}
				}
			}
		}
	}
	return result;
}

bool Intersect(Ray ray, Geometry geo, Intersection * ix)
{
	BEGIN_PROFILE(Intersect);
	bool result = false;
	switch(geo.type)
	{
		case GeoType::SPHERE:
		{
			result = IntersectRaySphere(ray, geo.sphere, ix);
		} break;

		case GeoType::PLANE:
		{
			result = IntersectRayPlane(ray, geo.plane, ix);
		} break;

		case GeoType::MESH:
		{
			result = IntersectRayMesh(ray, geo.mesh, ix);
		} break;

		default:
		{

		} break;
	}

	END_PROFILE(Intersect);
	return result;
}

V3 RandomDirectionOnHemisphere2(V3 n)
{
	float incl, azimuth;
	incl = ((float)rand() / RAND_MAX) * PI * 0.5f;
	azimuth = ((float)rand() / RAND_MAX) * PI2;

	float x, y, z;
	x = sin(incl)*cos(azimuth);
	y = sin(incl)*sin(azimuth);
	z = cos(incl);
	V3 direction = {x, y, z};

	V3 u, v;
	OrthonormalBasisFromAxis(n, &u, &v);
	M4x4 m = M4x4::RotationIntoBasis(n, u, v);
	V3 result = m * direction;

	return Normalize(result);
}

V3 RandomDirectionOnHemisphere(V3 n)
{

	float x, y, z;
	x = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
	y = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
	z = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
	V3 direction = Normalize({x, y, z});

	if(Dot(direction, n) < 0)
	{
		direction = -direction;
	}

	return direction;
}
