#include <cmath>
#include <float.h>
#include <cstdio> // for sprintf

#define __MATH_TEST__ 0

#if __MATH_TEST__
#include <directxmath.h>
using namespace DirectX;
#endif

#define PI 3.1415926535f
#define PI2 6.2831853072f
#define FLOAT_MIN FLT_MIN
#define FLOAT_MAX FLT_MAX

/* Misc */
inline float DegToRad(float deg)
{
	float rad = (deg * PI) / 180.0f;
	return rad;
}

inline float RadToDeg(float rad)
{
	float deg = (rad * 180.0f) / PI;
	return deg;
}

inline float Lerp(float a, float t, float b)
{
	float result = (1.0f - t)*a + t*b;
	return result;
}

inline int Sign(int i)
{
	return i >= 0 ? 1 : -1;
}

inline int Sign(float i)
{
	return i >= 0.0f ? 1 : -1;
}

inline float Clamp(float a, float min, float max)
{
	float result = a;
	if(result < min)
		result = min;
	else if(result > max)
		result = max;
	return result;
}

inline int Clamp(int a, int min, int max)
{
	int result = a;
	if(result < min)
		result = min;
	else if(result > max)
		result = max;
	return result;
}

inline float Min(float a, float b)
{
	if(a > b)
		return b;
	return a;
}

inline float Min(float a, float b, float c)
{
	return Min(Min(a, b), c);
}

inline float Max(float a, float b)
{
	if(a < b)
		return b;
	return a;
}

inline float Max(float a, float b, float c)
{
	return Max(Max(a, b), c);
}

/* Vector 2D */
union V2
{
	struct
	{
		float x, y;
	};
	float a[2];
	float operator[](int i) {
		assert(i >= 0 && i < 2);
		return a[i];
	};
};

V2 operator-(V2 & v)
{
	V2 result = {-v.x, -v.y};
	return result;
}

V2 operator-(const V2 & u, const V2 & v)
{
	V2 result = {u.x - v.x, u.y - v.y};
	return result;
}

V2 operator+(const V2 & u, const V2 & v)
{
	V2 result = {u.x + v.x, u.y + v.y};
	return result;
}

V2 operator+=(V2 & u, const V2 & v)
{
	u.x += v.x;
	u.y += v.y;
	return u;
}

V2 operator-=(V2 & u, const V2 & v)
{
	u.x -= v.x;
	u.y -= v.y;
	return u;
}

bool operator==(V2 & a, V2 & b)
{
	bool result = (a.x == b.x) && (a.y == b.y);
	return result;
}

bool operator!=(V2 & a, V2 & b)
{
	bool result = (a.x != b.x) || (a.y != b.y);
	return result;
}

V2 operator*(const V2 & v, float a)
{
	V2 result = {v.x * a, v.y * a,};
	return result;
}

V2 operator/(const V2 & v, float a)
{
	assert(a != 0);
	V2 result = {v.x / a, v.y / a};
	return result;
}

V2 Hadamard(const V2 & u, const V2 & v)
{
	V2 result = V2{u.x * v.x, u.y * v.y};
	return result;
}

inline float LengthSq(V2 v)
{
	float result = v.x*v.x + v.y*v.y;
	return result;
}

inline float Length(V2 v)
{
	float result = sqrt(v.x*v.x + v.y*v.y);
	return result;
}

inline V2 Normalize(V2 v)
{
	float length = sqrt(v.x*v.x + v.y*v.y);
	assert(length != 0);
	V2 result = v / length;
	return result;
}

inline V2 NormalizeSafe(V2 v)
{
	V2 result{};
	float length = sqrt(v.x*v.x + v.y*v.y);
	if(length != 0)
		result = v / length;
	return result;
}

inline float Dot(V2 u, V2 v)
{
	float result = {u.x*v.x + u.y*v.y};
	return result;
}

/* Vector 3D */
union V3
{
	struct
	{
		float x, y, z;
	};
	float a[3];
	float operator[](int i) {
		assert(i >= 0 && i < 3);
		return a[i];
	};

	static V3 Zero()	 	{ return V3{0.0f, 0.0f, 0.0f}; };
	static V3 FloatMax() 	{ return V3{FLOAT_MAX, FLOAT_MAX, FLOAT_MAX}; };
	static V3 FloatMinPos() { return V3{FLOAT_MIN, FLOAT_MIN, FLOAT_MIN}; };
	static V3 FloatMin() 	{ return V3{-FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX}; };
};

V3 operator-(const V3 & v)
{
	V3 result = {-v.x, -v.y, -v.z};
	return result;
}

V3 operator-(const V3 & u, const V3 & v)
{
	V3 result = {u.x - v.x, u.y - v.y, u.z - v.z};
	return result;
}

V3 operator+(const V3 & u, const V3 & v)
{
	V3 result = {u.x + v.x, u.y + v.y, u.z + v.z};
	return result;
}

V3 operator+=(V3 & u, const V3 & v)
{
	u.x += v.x;
	u.y += v.y;
	u.z += v.z;
	return u;
}

V3 operator-=(V3 & u, const V3 & v)
{
	u.x -= v.x;
	u.y -= v.y;
	u.z -= v.z;
	return u;
}

bool operator==(V3 & a, V3 & b)
{
	bool result = (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
	return result;
}

bool operator!=(V3 & a, V3 & b)
{
	bool result = (a.x != b.x) || (a.y != b.y) || (a.z != b.z);
	return result;
}

V3 operator*(const V3 & v, float a)
{
	V3 result = {v.x * a, v.y * a, v.z * a};
	return result;
}

V3 operator*(float a, const V3 & v)
{
	V3 result = {v.x * a, v.y * a, v.z * a};
	return result;
}

V3 operator/(const V3 & v, float a)
{
	assert(a != 0);
	V3 result = {v.x / a, v.y / a, v.z / a};
	return result;
}

inline float LengthSq(V3 v)
{
	float result = v.x*v.x + v.y*v.y + v.z*v.z;
	return result;
}

inline float Length(V3 v)
{
	float result = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	return result;
}

inline V3 Normalize(V3 v)
{
	float length = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	assert(length != 0);
	V3 result = v / length;
	return result;
}

inline V3 NormalizeSafe(V3 v)
{
	V3 result{};
	float length = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	if(length != 0)
		result = v / length;
	return result;
}

inline float Dot(V3 u, V3 v)
{
	float result = u.x*v.x + u.y*v.y + u.z*v.z;
	return result;
}

inline V3 Reflect(V3 v, V3 n)
{
	V3 result = v - 2*Dot(v, n)*n;
	return result;
}

inline V3 Cross(V3 u, V3 v)
{
	V3 result;
	result.x = u.y*v.z - u.z*v.y;
	result.y = u.z*v.x - u.x*v.z;
	result.z = u.x*v.y - u.y*v.x;
	return result;
}

V3 Lerp(V3 a, float t, V3 b)
{
	V3 result = {};
	result.x = Lerp(a.x, t, b.x);
	result.y = Lerp(a.y, t, b.y);
	result.z = Lerp(a.z, t, b.z);
	return result;
}

union V3i
{
	struct
	{
		int x, y, z;
	};
	int a[3];
	int operator[](int i) {
		assert(i >= 0 && i < 3);
		return a[i];
	};
};

union V3u
{
	struct
	{
		uint x, y, z;
	};
	uint a[3];
	uint operator[](int i) {
		assert(i >= 0 && i < 3);
		return a[i];
	};
};

/* Vector 4D */
union V4
{
	struct
	{
		float x, y, z, w;
	};
	struct
	{
		float r, g, b, a;
	};
	float v[4];
	float operator[](int i) {
		assert(i >= 0 && i < 4);
		return v[i];
	};
	V3 xyz;

	static V4 FloatMax() 	{ return V4{FLOAT_MAX, FLOAT_MAX, FLOAT_MAX, FLOAT_MAX}; };
	static V4 FloatMinPos() { return V4{FLOAT_MIN, FLOAT_MIN, FLOAT_MIN, FLOAT_MIN}; };
	static V4 FloatMin() 	{ return V4{-FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX}; };
	static V4 FromFloat(float v) { return V4{v, v, v, v}; };
};

// TODO: figure out, how to use constructor and not break init lists
V4 MakeV4(V3 v, float w)
{
	V4 result;
	result.x = v.x;
	result.y = v.y;
	result.z = v.z;
	result.w =   w;
	return result;
}

V4 operator-(const V4 & u, const V4 & v)
{
	V4 result = {u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w};
	return result;
}

V4 operator+(const V4 & u, const V4 & v)
{
	V4 result = {u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w};
	return result;
}

V4 operator+=(V4 & u, const V4 & v)
{
	u.x += v.x;
	u.y += v.y;
	u.z += v.z;
	u.w += v.w;
	return u;
}

V4 operator-=(V4 & u, const V4 & v)
{
	u.x -= v.x;
	u.y -= v.y;
	u.z -= v.z;
	u.w -= v.w;
	return u;
}

V4 operator*(const V4 & v, float c)
{
	V4 result = V4{v.x*c, v.y*c, v.z*c, v.w*c};
	return result;
}

V4 operator*(float c, const V4 & v)
{
	V4 result = v * c;
	return result;
}

V4 ComponentMultiply(const V4 & u, const V4 & v)
{
	V4 result = {u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w};
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

V4 operator/(const V4 & v, float c)
{
	V4 result = V4{v.x/c, v.y/c, v.z/c, v.w/c};
	return result;
}

float Dot(V4 a, V4 b)
{
	float result = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
	return result;
}

/**********************************************************
/			Matrices
/
/ Matrices are stored in memory as row-major.
/ In math operations vectors are treated as column-vectors,
/ which means that the order of multiplication is Mv
/ (matrix on the left)
/ View and projection transform assume RH coordinate system
/**********************************************************

/* 3x3 Matrix */
union M3x3
{
	struct
	{
		float m00, m01, m02,
			  m10, m11, m12,
			  m20, m21, m22;
	};
	float m[3][3];
	static M3x3 Identity()
	{
		M3x3 result = {1.0f, 0.0f, 0.0f,
					   0.0f, 1.0f, 0.0f,
					   0.0f, 0.0f, 1.0f};
		return result;
	}

	static M3x3 Transpose(M3x3 & M)
	{
		M3x3 result = {
			M.m00, M.m10, M.m20,
			M.m01, M.m11, M.m21,
			M.m02, M.m12, M.m22
		};
		return result;
	}
};

V3 operator*(M3x3 & M, V3 & v)
{
	V3 result;
	result.x = M.m00*v.x + M.m01*v.y + M.m02*v.z;
	result.y = M.m10*v.x + M.m11*v.y + M.m12*v.z;
	result.z = M.m20*v.x + M.m21*v.y + M.m22*v.z;
	return result;
}

/* 4x4 Matrix */
union M4x4
{
	struct
	{
		float m00, m01, m02, m03,
			  m10, m11, m12, m13,
			  m20, m21, m22, m23,
			  m30, m31, m32, m33;
	};
	float m[4][4];
	static M4x4 Identity()
	{
		M4x4 result = {1.0f, 0.0f, 0.0f, 0.0f,
					   0.0f, 1.0f, 0.0f, 0.0f,
					   0.0f, 0.0f, 1.0f, 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f};
		return result;
	}

	static M4x4 Transpose(M4x4 & M)
	{
		M4x4 result = {
			M.m00, M.m10, M.m20, M.m30,
			M.m01, M.m11, M.m21, M.m31,
			M.m02, M.m12, M.m22, M.m32,
			M.m03, M.m13, M.m23, M.m33
		};
		return result;
	}

	static M4x4 RotationX(float deg)
	{
		float rad = DegToRad(deg);
		float cs = cos(rad);
		float sn = sin(rad);
		M4x4 result = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f,   cs,  -sn, 0.0f,
			0.0f,   sn,   cs, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		return result;
	}

	static M4x4 RotationY(float deg)
	{
		float rad = DegToRad(deg);
		float cs = cos(rad);
		float sn = sin(rad);
		M4x4 result = {
			  cs, 0.0f,   sn, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			 -sn, 0.0f,   cs, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		return result;
	}

	static M4x4 RotationZ(float deg)
	{
		float rad = DegToRad(deg);
		float cs = cos(rad);
		float sn = sin(rad);
		M4x4 result = {
			  cs,  -sn, 0.0f, 0.0f,
			  sn,   cs, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		return result;
	}

	static M4x4 RotationIntoBasis(V3 xAxis, V3 yAxis, V3 zAxis)
	{
		M4x4 result = {xAxis.x, xAxis.y, xAxis.z, 0.0f,
					   yAxis.x, yAxis.y, yAxis.z, 0.0f,
					   zAxis.x, zAxis.y, zAxis.z, 0.0f,
					   0.0f, 	0.0f, 	 0.0f, 	  1.0f};
	   return result;
	}

	static M4x4 Translation(float x, float y, float z)
	{
		M4x4 result = {1.0f, 0.0f, 0.0f, x,
					   0.0f, 1.0f, 0.0f, y,
					   0.0f, 0.0f, 1.0f, z,
					   0.0f, 0.0f, 0.0f, 1.0f};
		return result;
	}

	static M4x4 Scale(float x, float y, float z)
	{
		M4x4 result = {   x, 0.0f, 0.0f, 0.0f,
					   0.0f,    y, 0.0f, 0.0f,
					   0.0f, 0.0f,    z, 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f};
		return result;
	}

	static void DebugPrintToBuffer(M4x4 M, char * buffer)
	{
		wsprintf(buffer, "\n|%12.9f %12.9f %12.9f %12.9f|\n|%12.9f %12.9f %12.9f %12.9f|\n|%12.9f %12.9f %12.9f %12.9f|\n|%12.9f %12.9f %12.9f %12.9f|\n\n",
				M.m00, M.m01, M.m02, M.m03,
				M.m10, M.m11, M.m12, M.m13,
				M.m20, M.m21, M.m22, M.m23,
				M.m30, M.m31, M.m32, M.m33);
	}

	static void DebugPrintToOutput(M4x4 M)
	{
		#if DEBUG
		char buffer[512];
		sprintf(buffer, "\n|%12.9f %12.9f %12.9f %12.9f|\n|%12.9f %12.9f %12.9f %12.9f|\n|%12.9f %12.9f %12.9f %12.9f|\n|%12.9f %12.9f %12.9f %12.9f|\n\n",
				M.m00, M.m01, M.m02, M.m03,
				M.m10, M.m11, M.m12, M.m13,
				M.m20, M.m21, M.m22, M.m23,
				M.m30, M.m31, M.m32, M.m33);
		OutputDebugStringA(buffer);
		#endif
	}

	// NOTE: view direction is -Z, X is right, Y is X×Z
	static M4x4 ViewRH(V3 eyePosition, V3 direction, V3 up)
	{
		V3 zAxis = Normalize(-direction);
		V3 xAxis = Normalize(Cross(up, zAxis));
		V3 yAxis = Cross(zAxis, xAxis);

		float translateX = Dot(-eyePosition, xAxis);
		float translateY = Dot(-eyePosition, yAxis);
		float translateZ = Dot(-eyePosition, zAxis);
		// NOTE: top left 3x3 matrix is the inverse of the camera rotation matrix
		//  and is equal to its transpose.
		//  Right column is the inverse camera translation
		M4x4 result = {xAxis.x, xAxis.y, xAxis.z, translateX,
					   yAxis.x, yAxis.y, yAxis.z, translateY,
					   zAxis.x, zAxis.y, zAxis.z, translateZ,
					   0.0f, 	0.0f, 	 0.0f, 	  1.0f};

		return result;
	}

	// NOTE: view direction is +Z, the rest is the same
	static M4x4 ViewLH(V3 eyePosition, V3 direction, V3 up)
	{
		V3 zAxis = Normalize(direction);
		V3 xAxis = Normalize(Cross(zAxis, up));
		V3 yAxis = Cross(xAxis, zAxis);

		float translateX = Dot(-eyePosition, xAxis);
		float translateY = Dot(-eyePosition, yAxis);
		float translateZ = Dot(-eyePosition, zAxis);
		// NOTE: top left 3x3 matrix is the inverse of the camera rotation matrix
		//  and is equal to its transpose.
		//  Right column is the inverse camera translation
		M4x4 result = {xAxis.x, xAxis.y, xAxis.z, translateX,
					   yAxis.x, yAxis.y, yAxis.z, translateY,
					   zAxis.x, zAxis.y, zAxis.z, translateZ,
					   0.0f, 	0.0f, 	 0.0f, 	  1.0f};

		return result;
	}

	// Transpose top left 3x3, negate translation
	static M4x4 InverseView(M4x4 M)
	{
		M4x4 result = {
			M.m00, M.m10, M.m20, -M.m03,
			M.m01, M.m11, M.m21, -M.m13,
			M.m02, M.m12, M.m22, -M.m23,
			M.m30, M.m31, M.m32,  M.m33};
		return result;
	}

	// NOTE: DirectX NDC range from {-1, -1, 0} to {1, 1, 1}

	// Ortho projection centered at the origin
	static M4x4 OrthographicRH(float width, float height, float nearZ, float farZ)
	{
		nearZ = -nearZ;
		farZ = -farZ;
		M4x4 result = {2/width, 0.0f,     0.0f,           0.0f,
					   0.0f,    2/height, 0.0f,           0.0f,
					   0.0f,    0.0f,     1/(farZ-nearZ), -nearZ/(farZ-nearZ),
					   0.0f,    0.0f,     0.0f,           1.0f};
		return result;
	}

	// Ortho projection offset from the origin
	static M4x4 OrthographicRH(float l, float r, float b, float t, float n, float f)
	{
		n = -n;
		f = -f;
		M4x4 result = {2/(r-l), 0.0f,    0.0f,    -(r+l)/(r-l),
					   0.0f,    2/(t-b), 0.0f,    -(t+b)/(t-b),
					   0.0f,    0.0f,    1/(f-n), -n/(f-n),
					   0.0f,    0.0f,    0.0f,    1.0f};
		return result;
	}

	// NOTE: for convenience nearZ and farZ are positive values, representing
	//  absolute distances along negative Z-axis
	static M4x4 PerspectiveRH(float hFOV, float aspect, float nearZ, float farZ)
	{
		float hFOVRad = DegToRad(hFOV);
		float xScale = 1.0f / tan(hFOVRad*0.5f);
		float yScale = xScale * aspect;

		float zScale = farZ / (farZ - nearZ);
		float zTranspose = nearZ*farZ / (farZ - nearZ);
		M4x4 result = {xScale, 0.0f,   0.0f,   0.0f,
					   0.0f,   yScale, 0.0f,   0.0f,
					   0.0f,   0.0f,   -zScale, -zTranspose,
					   0.0f,   0.0f,   -1.0f,   0.0f};
		return result;
	}

};

// Uses top left 3x3 sub-matrix for multiplication
V3 operator*(const M4x4 & M, V3 & v)
{
	V3 result;
	result.x = M.m00*v.x + M.m01*v.y + M.m02*v.z;
	result.y = M.m10*v.x + M.m11*v.y + M.m12*v.z;
	result.z = M.m20*v.x + M.m21*v.y + M.m22*v.z;
	return result;
}

V4 operator*(const M4x4 & M, V4 & v)
{
	V4 result;
	result.x = M.m00*v.x + M.m01*v.y + M.m02*v.z + M.m03*v.w;
	result.y = M.m10*v.x + M.m11*v.y + M.m12*v.z + M.m13*v.w;
	result.z = M.m20*v.x + M.m21*v.y + M.m22*v.z + M.m23*v.w;
	result.w = M.m30*v.x + M.m31*v.y + M.m32*v.z + M.m33*v.w;
	return result;
}

M4x4 operator*(const M4x4 & A, const M4x4 & B)
{
	M4x4 result = {
		A.m00*B.m00 + A.m01*B.m10 + A.m02*B.m20 + A.m03*B.m30,
		A.m00*B.m01 + A.m01*B.m11 + A.m02*B.m21 + A.m03*B.m31,
		A.m00*B.m02 + A.m01*B.m12 + A.m02*B.m22 + A.m03*B.m32,
		A.m00*B.m03 + A.m01*B.m13 + A.m02*B.m23 + A.m03*B.m33,

		A.m10*B.m00 + A.m11*B.m10 + A.m12*B.m20 + A.m13*B.m30,
		A.m10*B.m01 + A.m11*B.m11 + A.m12*B.m21 + A.m13*B.m31,
		A.m10*B.m02 + A.m11*B.m12 + A.m12*B.m22 + A.m13*B.m32,
		A.m10*B.m03 + A.m11*B.m13 + A.m12*B.m23 + A.m13*B.m33,

		A.m20*B.m00 + A.m21*B.m10 + A.m22*B.m20 + A.m23*B.m30,
		A.m20*B.m01 + A.m21*B.m11 + A.m22*B.m21 + A.m23*B.m31,
		A.m20*B.m02 + A.m21*B.m12 + A.m22*B.m22 + A.m23*B.m32,
		A.m20*B.m03 + A.m21*B.m13 + A.m22*B.m23 + A.m23*B.m33,

		A.m30*B.m00 + A.m31*B.m10 + A.m32*B.m20 + A.m33*B.m30,
		A.m30*B.m01 + A.m31*B.m11 + A.m32*B.m21 + A.m33*B.m31,
		A.m30*B.m02 + A.m31*B.m12 + A.m32*B.m22 + A.m33*B.m32,
		A.m30*B.m03 + A.m31*B.m13 + A.m32*B.m23 + A.m33*B.m33
	};
	return result;
}

V3 RotateX(V3 v, float deg)
{
	M4x4 M = M4x4::RotationX(deg);
	return M*v;
}
V3 RotateY(V3 v, float deg)
{
	M4x4 M = M4x4::RotationY(deg);
	return M*v;
}
V3 RotateZ(V3 v, float deg)
{
	M4x4 M = M4x4::RotationZ(deg);
	return M*v;
}

void OrthonormalBasisFromAxis(V3 xAxis, V3 * yAxis, V3 * zAxis)
{
	if((abs(xAxis.x) < abs(xAxis.y)) && (abs(xAxis.x) < abs(xAxis.z)))
	{
		*yAxis = {0.0f, -xAxis.z, xAxis.y};
	}
	else if((abs(xAxis.y) < abs(xAxis.x)) && (abs(xAxis.y) < abs(xAxis.z)))
	{
		*yAxis = {-xAxis.z, 0.0f, xAxis.x};
	}
	else
	{
		*yAxis = {-xAxis.y, xAxis.x, 0.0f};
	}
	*yAxis = Normalize(*yAxis);
	*zAxis = Cross(xAxis, *yAxis);
}

V3 RotateAxis(V3 v, V3 xAxis, float deg)
{
	V3 yAxis, zAxis, result;
	OrthonormalBasisFromAxis(xAxis, &yAxis, &zAxis);

	M4x4 M1 = M4x4::RotationIntoBasis(xAxis, yAxis, zAxis);
	M4x4 M2 = M4x4::Transpose(M1);
	M4x4 M3 = M4x4::RotationX(deg);
	result = M1*v;
	result = M3*result;
	result = M2*result;

	return result;
}

#pragma warning(disable:4189)
#pragma warning(disable:4101)


#if __MATH_TEST__

inline void TestMath()
{
	V3 test0;
	V3 test1 = {};
	V3 test2{};
	V3 test3 = {0};
	V3 test4{0};
	V3 test5 = {4, 5, 6};
	V3 test6{7, 8, 9};
	V3 test7{11};
	float test8 = test6.x;
	float test9 = test7.a[1];
	float test10 = test5[1];

	M4x4 identity = M4x4::Identity();
	V4 vec = {0.34f, 0.12f, 0.88f, 1.0f};
	vec = identity*vec;

	M4x4 translation = M4x4::Translation(0.0f, 1.0f, 0.0f);
	V4 translated = translation*vec;

	V4 vec2 = {0.0f, 0.0f, 1.0f, 1.0f};
	M4x4 view = M4x4::ViewRH({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f});
	M4x4::DebugPrintToOutput(view);
	V4 viewTransformed = view*vec2;

	M4x4 viewMy = M4x4::ViewRH({5.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f});
	XMFLOAT4 fpos = {5.0f, -1.0f, 0.0f, 0.0f};
	XMFLOAT4 fdir = {0.0f, 1.0f, 0.0f, 0.0f};
	XMFLOAT4 fup = {0.0f, 0.0f, 1.0f, 0.0f};
	XMVECTOR pos = XMLoadFloat4(&fpos);
	XMVECTOR dir = XMLoadFloat4(&fdir);
	XMVECTOR up = XMLoadFloat4(&fup);
	XMMATRIX xmviewDX = XMMatrixLookToRH(pos, dir, up);
	xmviewDX = XMMatrixTranspose(xmviewDX);

	XMFLOAT4X4 fviewDX;
	XMStoreFloat4x4(&fviewDX, xmviewDX);

	M4x4 viewDX = {fviewDX._11, fviewDX._12, fviewDX._13, fviewDX._14,
				   fviewDX._21, fviewDX._22, fviewDX._23, fviewDX._24,
				   fviewDX._31, fviewDX._32, fviewDX._33, fviewDX._34,
				   fviewDX._41, fviewDX._42, fviewDX._43, fviewDX._44};

	M4x4::DebugPrintToOutput(viewMy);
	M4x4::DebugPrintToOutput(viewDX);

	viewTransformed = viewMy*vec2;

	XMFLOAT4X4 m1, m2, m3;
	XMStoreFloat4x4(&m1, XMMatrixRotationRollPitchYaw(XM_PIDIV4, XM_PIDIV2, 0.0f));
	XMStoreFloat4x4(&m2, XMMatrixRotationY(XM_PIDIV2) * XMMatrixRotationX(XM_PIDIV4));
	XMStoreFloat4x4(&m3, XMMatrixRotationX(XM_PIDIV4) * XMMatrixRotationY(XM_PIDIV2));

	M4x4 M1 = {m1._11, m1._12, m1._13, m1._14,
				   m1._21, m1._22, m1._23, m1._24,
				   m1._31, m1._32, m1._33, m1._34,
				   m1._41, m1._42, m1._43, m1._44};
	M4x4 M2 = {m2._11, m2._12, m2._13, m2._14,
				   m2._21, m2._22, m2._23, m2._24,
				   m2._31, m2._32, m2._33, m2._34,
				   m2._41, m2._42, m2._43, m2._44};
	M4x4 M3 = {m3._11, m3._12, m3._13, m3._14,
				   m3._21, m3._22, m3._23, m3._24,
				   m3._31, m3._32, m3._33, m3._34,
				   m3._41, m3._42, m3._43, m3._44};

	M4x4::DebugPrintToOutput(M1);
	M4x4::DebugPrintToOutput(M2);
	M4x4::DebugPrintToOutput(M3);

#if 0
// Test tangent space transforms
{
	V3 v0 = V3{3.5f, 14.1f, 5.8f};
	V3 v1 = V3{9.1f, 11.4f, 7.8f};
	V3 v2 = V3{7.9f, 25.7f, 11.3f};

	V2 vt0 = V2{0.52f, 0.31};
	V2 vt1 = V2{0.70f, 0.25};
	V2 vt2 = V2{0.61f, 0.57};

	V2 vt0m = V2{1.0f - 0.52f, 0.31};
	V2 vt1m = V2{1.0f - 0.70f, 0.25};
	V2 vt2m = V2{1.0f - 0.61f, 0.57};

	V3 dp1 = v1 - v0;
	V3 dp2 = v2 - v0;

	float ds1 = vt1.x - vt0.x;
	float dt1 = vt1.y - vt0.y;
	float ds2 = vt2.x - vt0.x;
	float dt2 = vt2.y - vt0.y;

	float ds1m = vt1m.x - vt0m.x;
	float dt1m = vt1m.y - vt0m.y;
	float ds2m = vt2m.x - vt0m.x;
	float dt2m = vt2m.y - vt0m.y;

	float D = 1.0f / (ds1*dt2 - ds2*dt1);
	V3 n = Normalize(Cross(dp1, dp2));
	V3 t = Normalize((dp1*dt2 - dp2*dt1) * D);
	V3 b = Normalize((dp2*ds1 - dp1*ds2) * D);

	float Dm = 1.0f / (ds1m*dt2m - ds2m*dt1m);
	V3 nm = Normalize(Cross(dp1, dp2));
	V3 tm = Normalize((dp1*dt2m - dp2*dt1m) * Dm);
	V3 bm = Normalize((dp2*ds1m - dp1*ds2m) * Dm);

	M3x3 worldToTangent = {
		t.x, t.y, t.z,
		b.x, b.y, b.z,
		n.x, n.y, n.z
	};

	M3x3 worldToTangentM = {
		tm.x, tm.y, tm.z,
		bm.x, bm.y, bm.z,
		nm.x, nm.y, nm.z
	};

	M3x3 tangentToWorld = M3x3::Transpose(worldToTangent);
	M3x3 tangentToWorldM = M3x3::Transpose(worldToTangentM);

	V3 tangentSpaceNormal = Normalize(V3{1.0f, 1.0f, 1.0f});
	V3 tangentSpaceNormalM = Normalize(V3{-1.0f, 1.0f, 1.0f});

	V3 normalFromTangent = tangentToWorld * tangentSpaceNormal;
	V3 normalFromTangentM = tangentToWorldM * tangentSpaceNormalM;

	Log("{%2.3f, %2.3f, %2.3f}, {%2.3f, %2.3f, %2.3f}, {%2.3f, %2.3f, %2.3f}", n.x, n.y, n.z, t.x, t.y, t.z, b.x, b.y, b.z);
	Log("{%2.3f, %2.3f, %2.3f}, {%2.3f, %2.3f, %2.3f}, {%2.3f, %2.3f, %2.3f}", nm.x, nm.y, nm.z, tm.x, tm.y, tm.z, bm.x, bm.y, bm.z);
	Log("{%2.3f, %2.3f, %2.3f}", normalFromTangent.x, normalFromTangent.y, normalFromTangent.z);
	Log("{%2.3f, %2.3f, %2.3f}", normalFromTangentM.x, normalFromTangentM.y, normalFromTangentM.z);
}
#endif

}
#else
inline void TestMath() {};
#endif

#pragma warning(default:4101)
#pragma warning(default:4189)
