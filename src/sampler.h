#define Random(min, max) gPerThreadRng[LOCAL_THREAD_ID].Next((min), (max))

/* Random distribution */

V3 RotateSample(V3 s, V3 n)
{
	V3 u, v;
	OrthonormalBasisFromAxis(n, &u, &v);
	M4x4 m = M4x4::Transpose(M4x4::RotationIntoBasis(u, v, n));
	return m * s;
}

void GetRandomSamplesOnUnitCubeSurface(uint sampleCount, V3 * samples)
{
	for(uint i = 0; i < sampleCount; ++i)
	{
		float x, y, z;
		V3 direction = {};

		x = Random(-1.0f, 1.0f);
		y = Random(-1.0f, 1.0f);
		z = Random(-1.0f, 1.0f);

		if((i % 6) == 0) x = 1.0f;
		if((i % 6) == 1) y = 1.0f;
		if((i % 6) == 2) z = 1.0f;
		if((i % 6) == 3) x = -1.0f;
		if((i % 6) == 4) y = -1.0f;
		if((i % 6) == 5) z = -1.0f;

		samples[i] = V3{x, y, z};
	}
}

uint GetRandomSamplesInUnitCube(uint sampleCount, V3 * samples)
{
	for(uint i = 0; i < sampleCount; ++i)
	{
		float x, y, z;
		V3 direction = {};

		x = Random(-1.0f, 1.0f);
		y = Random(-1.0f, 1.0f);
		z = Random(-1.0f, 1.0f);

		samples[i] = V3{x, y, z};
	}
	return sampleCount;
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
	V3 direction = {};

	while(true)
	{
		x = Random(-1.0f, 1.0f);
		y = Random(-1.0f, 1.0f);
		z = Random(-1.0f, 1.0f);

		direction = {x, y, z};
		if(LengthSq(direction) <= 1.0f)
		{
			break;
		}
	}

	direction = Normalize(direction);
	if(Dot(direction, n) < 0)
	{
		direction = -direction;
	}

	return direction;
}

uint GetRandomSamplesOnHemisphere(uint sampleCount, V3 * samples)
{
	for(uint i = 0; i < sampleCount; ++i)
	{
		samples[i] = RandomDirectionOnHemisphere({0, 0, 1});
	}
	return sampleCount;
}

V2 GetRandomSampleOnDisk()
{
	while(true)
	{
		float x = Random(-1.0f, 1.0f);
		float y = Random(-1.0f, 1.0f);
		V2 sample = V2{x, y};
		if(LengthSq(sample) <= 1.0f)
			return sample;
	}
}

uint GetRandomSamplesOnDisk(uint sampleCount, V3 * samples)
{
	for(uint i = 0; i < sampleCount; ++i)
	{
		V2 sample2D = GetRandomSampleOnDisk();
		samples[i] = V3{sample2D.x, sample2D.y, 0.0f};
	}
	return sampleCount;
}

/* uniform distribution */

uint GetUniformSamplesOnSquare(uint requestedSampleCount, V3 * samples)
{
	int gridSize = (int)floor(sqrt(requestedSampleCount));
	float side = 2.0f;
	float cellSize = side / gridSize;
	float halfCellSize = cellSize * 0.5f;
	for(int y = 0; y < gridSize; ++y)
	{
		for(int x = 0; x < gridSize; ++x)
		{
			int i = y*gridSize + x;
			samples[i] = V3{-side*0.5f + x*cellSize + halfCellSize, -side*0.5f + y*cellSize + halfCellSize, 0.0f};
		}
	}
	return gridSize*gridSize;
}

// NOTE: see "A Low Distortion Map Between Disk and Square" by Peter Shirley, Kenneth Chiu
V2 Shirley(V2 s)
{
	assert(Clamp(s.x, -1.0f, 1.0f) == s.x);
	assert(Clamp(s.y, -1.0f, 1.0f) == s.y);
	float phi, r, u, v;
	float a = s.x;
	float b = s.y;
	if(a > -b)  // region 1 or 2
	{
		if(a > b) // region 1, also |a| > |b|
		{
			r = a;
			phi = (PI/4.0f) * (b/a);
		}
		else // region 2, also |b| > |a|
		{
			r = b;
			phi = (PI/4.0f) * (2 - (a/b));
		}
	}
	else // region 3 or 4
	{
		if(a < b) // region 3, also |a| >= |b|, a != 0
		{
			r = -a;
			phi = (PI/4.0f) * (4 + (b/a));
		}
		else // region 4, |b| >= |a|, but a==0 and b==0 could occur.
		{
			r = -b;
			if(b != 0)
			{
				phi = (PI/4.0f) * (6 - (a/b));
			}
			else
			{
				phi = 0;
			}
		}
	}

	u=r*cos(phi);
	v=r*sin(phi);
	return V2{u, v};
}

uint GetUniformSamplesOnDisk(uint requestedSampleCount, V3 * samples)
{
	uint sampleCount = GetUniformSamplesOnSquare(requestedSampleCount, samples);
	for(uint i = 0; i < sampleCount; ++i)
	{
		V2 s = Shirley(V2{samples[i].x, samples[i].y});
		samples[i] = V3{s.x, s.y, 0.0f};
	}

	return sampleCount;
}

uint GetUniformSamplesOnHemisphere(uint requestedSampleCount, V3 * samples)
{
	uint sampleCount = GetUniformSamplesOnDisk(requestedSampleCount, samples);
	for(uint i = 0; i < sampleCount; ++i)
	{
		V3 s = samples[i];
		float rr = s.x*s.x + s.y*s.y;
		s.x = samples[i].x*sqrt(2.0f - rr);
		s.y = samples[i].y*sqrt(2.0f - rr);
		s.z = 1.0f - rr;
		samples[i] = V3{s.x, s.y, s.z};
	}
	return sampleCount;
}

/* Jittered */

uint GetJitteredSamplesOnSquare(uint requestedSampleCount, V3 * samples)
{
	int gridSize = (int)floor(sqrt(requestedSampleCount));
	float side = 2.0f;
	float cellSize = side / gridSize;
	float halfCellSize = cellSize * 0.5f;
	for(int y = 0; y < gridSize; ++y)
	{
		for(int x = 0; x < gridSize; ++x)
		{
			float ox = Random(-0.5f, 0.5f);
			float oy = Random(-0.5f, 0.5f);
			int i = y*gridSize + x;
			samples[i] = V3{-side*0.5f + x*cellSize + ox*cellSize + halfCellSize, -side*0.5f + y*cellSize + oy*cellSize + halfCellSize, 0.0f};
		}
	}
	return gridSize*gridSize;
}

uint GetJitteredSamplesOnDisk(uint requestedSampleCount, V3 * samples)
{
	uint sampleCount = GetJitteredSamplesOnSquare(requestedSampleCount, samples);
	for(uint i = 0; i < sampleCount; ++i)
	{
		V2 s = Shirley(V2{samples[i].x, samples[i].y});
		samples[i] = V3{s.x, s.y, 0.0f};
	}

	return sampleCount;
}

uint GetJitteredSamplesOnHemisphere(uint requestedSampleCount, V3 * samples)
{
	uint sampleCount = GetJitteredSamplesOnDisk(requestedSampleCount, samples);
	for(uint i = 0; i < sampleCount; ++i)
	{
		V3 s = samples[i];
		float rr = s.x*s.x + s.y*s.y;
		s.x = samples[i].x*sqrt(2.0f - rr);
		s.y = samples[i].y*sqrt(2.0f - rr);
		s.z = 1.0f - rr;
		samples[i] = V3{s.x, s.y, s.z};
	}
	return sampleCount;
}
