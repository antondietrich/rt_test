#pragma once

#include <math.h>

#define EPSYLON 1e-4f

inline float Lerp(float a, float t, float b)
{
	return (1.0f - t)*a + t*b;
}

inline float Saturate(float v)
{
	float result = min(max(v, 0), 1);;
	return result;
}

//////////////////////////////////////////////////////////////////////////
// Vector 2

struct V2
{
	union
	{
		struct {float x, y;};
		float v[2];
	};
};

V2 operator+(const V2 & a, const V2 & b)
{
	V2 result = {a.x + b.x, a.y + b.y};
	return result;
}

V2 operator-(const V2 & a, const V2 & b)
{
	V2 result = {a.x - b.x, a.y - b.y};
	return result;
}

V2 operator*(const V2 & a, const float & c)
{
	V2 result = {a.x*c, a.y*c};
	return result;
}

//////////////////////////////////////////////////////////////////////////
// Vector 3

struct V3
{
	union
	{
		struct {float x, y, z;};
		float v[3];
	};
};

V3 operator+(const V3 & a, const V3 & b)
{
	V3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
	return result;
}

V3 operator-(const V3 & a, const V3 & b)
{
	V3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
	return result;
}

V3 operator-(const V3 & a)
{
	V3 result = {-a.x, -a.y, -a.z};
	return result;
}

V3 operator*(const V3 & a, const float & c)
{
	V3 result = {a.x*c, a.y*c, a.z*c};
	return result;
}

V3 operator/(const V3 & a, const float & c)
{
	V3 result = {a.x/c, a.y/c, a.z/c};
	return result;
}

inline float Dot(V3 a, V3 b)
{
	float result = a.x*b.x + a.y*b.y + a.z*b.z;
	return result;
}

inline V3 Cross(V3 a, V3 b)
{
	V3 result = {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
	return result;
}

inline float LengthSq(V3 v)
{
	float result = Dot(v, v);
	return result;
}

inline float Length(V3 v)
{
	return (float)sqrt(LengthSq(v));
}

inline V3 Normalize(V3 v)
{
	V3 result = v / Length(v);
	return result;
}

inline V3 Reflect(V3 v, V3 n)
{
	return v - n*2*Dot(v, n);
}

inline V3 Saturate(V3 v)
{
	V3 result = v;
	result.x = min(max(result.x, 0), 1);
	result.y = min(max(result.y, 0), 1);
	result.z = min(max(result.z, 0), 1);
	return result;
}

//////////////////////////////////////////////////////////////////////////
// Vector 4

struct V4
{
	union
	{
		struct {float x, y, z, w;};
		struct {float r, g, b, a;};
		float v[4];
	};
};

V4 operator+(const V4 & a, const V4 & b)
{
	V4 result = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
	return result;
}

V4 operator-(const V4 & a, const V4 & b)
{
	V4 result = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
	return result;
}

V4 operator-(const V4 & a)
{
	V4 result = {-a.x, -a.y, -a.z, -a.w};
	return result;
}

V4 operator*(const V4 & a, const float & c)
{
	V4 result = {a.x*c, a.y*c, a.z*c, a.w*c};
	return result;
}

V4 operator*(const float & c, const V4 & a)
{
	return a*c;
}

V4 operator/(const V4 & a, const float & c)
{
	V4 result = {a.x/c, a.y/c, a.z/c, a.w/c};
	return result;
}

V4 ComponentAdd(const V4 & a, const V4 & b)
{
	V4 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	result.w = a.w + b.w;
	return result;
}

V4 ComponentMultiply(const V4 & a, const V4 & b)
{
	V4 result;
	result.x = a.x * b.x;
	result.y = a.y * b.y;
	result.z = a.z * b.z;
	result.w = a.w * b.w;
	return result;
}

V4 ComponentDivide(const V4 & a, const V4 & b)
{
	V4 result;
	result.x = a.x / b.x;
	result.y = a.y / b.y;
	result.z = a.z / b.z;
	result.w = a.w / b.w;
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

V4 Lerp(V4 a, float t, V4 b)
{
	return (1.0f - t)*a + t*b;
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

float Dot(V2 a, V2 b)
{
	float result = a.x*b.x + a.y*b.y;
	return result;
}

float Dot(V4 a, V4 b)
{
	float result = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
	return result;
}

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
	switch(geo.type)
	{
		case GeoType::SPHERE:
		{
			return IntersectRaySphere(ray, geo.sphere, ix);
		} break;

		case GeoType::PLANE:
		{
			return IntersectRayPlane(ray, geo.plane, ix);
		} break;

		case GeoType::MESH:
		{
			return IntersectRayMesh(ray, geo.mesh, ix);
		} break;

		default:
		{
			return false;
		} break;
	}
}
