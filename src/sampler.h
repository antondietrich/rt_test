void GetRandomSamplesOnUnitCubeSurface(uint sampleCount, V3 * samples)
{
	for(uint i = 0; i < sampleCount; ++i)
	{
		float x, y, z;
		V3 direction = {};

		x = gPerThreadRng[LOCAL_THREAD_ID].Next(-1.0f, 1.0f);
		y = gPerThreadRng[LOCAL_THREAD_ID].Next(-1.0f, 1.0f);
		z = gPerThreadRng[LOCAL_THREAD_ID].Next(-1.0f, 1.0f);

		if((i % 6) == 0) x = 1.0f;
		if((i % 6) == 1) y = 1.0f;
		if((i % 6) == 2) z = 1.0f;
		if((i % 6) == 3) x = -1.0f;
		if((i % 6) == 4) y = -1.0f;
		if((i % 6) == 5) z = -1.0f;

		samples[i] = V3{x, y, z};
	}
}

void GetRandomSamplesInUnitCube(uint sampleCount, V3 * samples)
{
	for(uint i = 0; i < sampleCount; ++i)
	{
		float x, y, z;
		V3 direction = {};

		x = gPerThreadRng[LOCAL_THREAD_ID].Next(-1.0f, 1.0f);
		y = gPerThreadRng[LOCAL_THREAD_ID].Next(-1.0f, 1.0f);
		z = gPerThreadRng[LOCAL_THREAD_ID].Next(-1.0f, 1.0f);

		samples[i] = V3{x, y, z};
	}
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
		x = gPerThreadRng[LOCAL_THREAD_ID].Next(-1.0f, 1.0f);
		y = gPerThreadRng[LOCAL_THREAD_ID].Next(-1.0f, 1.0f);
		z = gPerThreadRng[LOCAL_THREAD_ID].Next(-1.0f, 1.0f);

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

void GetRandomSamplesOnHemisphere(uint sampleCount, V3 * samples)
{
	for(uint i = 0; i < sampleCount; ++i)
	{
		samples[i] = RandomDirectionOnHemisphere({0, 0, 1});
	}
}
