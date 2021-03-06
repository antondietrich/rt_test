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

struct AABB
{
	V3 min;
	V3 max;
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
	AABB aabb;
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

void ComputeMeshBound(Mesh * mesh)
{
	V3 min = V3::FloatMax();
	V3 max = V3::FloatMin();
	for(int i = 0; i < mesh->vertexCount; ++i)
	{
		Vertex * v = &mesh->vertices[i];
		if(min.x > v->position.x) min.x = v->position.x;
		if(min.y > v->position.y) min.y = v->position.y;
		if(min.z > v->position.z) min.z = v->position.z;
		if(max.x < v->position.x) max.x = v->position.x;
		if(max.y < v->position.y) max.y = v->position.y;
		if(max.z < v->position.z) max.z = v->position.z;
	}
	mesh->aabb.min = min;
	mesh->aabb.max = max;
	mesh->bound = {};
	mesh->bound.o = (min + max) / 2;
	mesh->bound.r = Length((min - max) / 2);
}

bool IntersectRaySphere(Ray ray, Sphere sphere, Intersection * intersection)
{
// PROFILED_FUNCTION_FAST;
	// See Real-Time Rendering 3rd ed., p. 741
	V3 l = sphere.o - ray.o;
	float s = Dot(l, ray.d);
	float ll = Dot(l, l);
	float rr = sphere.r*sphere.r;
	if(s < 0 && ll > rr) return false;

	float mm = ll - s*s;
	if(mm > rr) return false;

	float q = sqrt(rr - mm);
	float t = 0;
	if(ll > rr)
		t = s - q;
	else
		t = s + q;

	if(intersection)
	{
		(*intersection).t = t;
		(*intersection).point = ray.o + ray.d*t;
		(*intersection).normal = Normalize((*intersection).point - sphere.o);
	}
	return true;
}

bool TestRaySphere(Ray ray, Sphere sphere)
{
 PROFILED_FUNCTION_FAST;
	// See Real-Time Rendering 3rd ed., p. 741
	V3 l = sphere.o - ray.o;
	float s = Dot(l, ray.d);
	float ll = Dot(l, l);
	float rr = sphere.r*sphere.r;
	if(s < 0 && ll > rr) return false;

	float mm = ll - s*s;
	if(mm > rr) return false;

	return true;
}

bool TestRayAABB(Ray ray, AABB aabb)
{
	PROFILED_FUNCTION_FAST;
	// NOTE: cheating! Represent ray as a very long line segment

	V3 offset = -(aabb.min + aabb.max) * 0.5f;
	V3 start = ray.o + offset;
	V3 end = start + ray.d*10000.0f;
	V3 c = (start + end)*0.5f;
	V3 w = (end - start)*0.5f;
	V3 h = aabb.max + offset;

	V3 v;
	v.x = fabsf(w.x);
	v.y = fabsf(w.y);
	v.z = fabsf(w.z);
	if(fabsf(c.x) > v.x + h.x) return false;
	if(fabsf(c.y) > v.y + h.y) return false;
	if(fabsf(c.z) > v.z + h.z) return false;
	if(fabsf(c.y*w.z - c.z*w.y) > h.y*v.z + h.z*v.y) return false;
	if(fabsf(c.x*w.z - c.z*w.x) > h.x*v.z + h.z*v.x) return false;
	if(fabsf(c.x*w.y - c.y*w.x) > h.x*v.y + h.y*v.x) return false;
	return true;
}

bool IntersectRayPlane(Ray ray, Plane plane, Intersection * intersection)
{
// PROFILED_FUNCTION_FAST;
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
 PROFILED_FUNCTION_FAST;
	bool result = false;
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
	return result;
}

bool TryIntersectRayMesh(Ray ray, Mesh mesh, Intersection * intersection)
{
	bool result = false;
	//return IntersectRaySphere(ray, mesh.bound, intersection);
	//if(TestRaySphere(ray, mesh.bound))
	{
		if(TestRayAABB(ray, mesh.aabb))
		{
			result = IntersectRayMesh(ray, mesh, intersection);
		}
	}
	return result;
}

bool Intersect(Ray ray, Geometry geo, Intersection * ix)
{
// PROFILED_FUNCTION_FAST;
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
			result = TryIntersectRayMesh(ray, geo.mesh, ix);
		} break;

		default:
		{

		} break;
	}

	return result;
}

