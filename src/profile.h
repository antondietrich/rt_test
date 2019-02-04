#define PROFILE_NAME_LENGTH 64
#define MAX_PROFILE_RECORDS 1024


double countsPerSecond;
double countsPerMs;


/*
Function/Block name | Call count | ms total | ms per call | cycles per call
__FILE__
__LINE__
__FUNCTION__ (__func__ ?)
__COUNTER__
*/

struct ProfileRecord
{
	char name[PROFILE_NAME_LENGTH];
	uint64 callCount;
	uint64 timeTotal;
	uint64 cyclesTotal;
	double msTotal;
	double msPerCall;
};

struct Profile
{
	ProfileRecord records[MAX_PROFILE_RECORDS];
};

Profile profile = {};

void InitProfiler()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	countsPerSecond = (double)freq.QuadPart;
	countsPerMs = (double)(freq.QuadPart * 1000);
}

inline uint64 GetHiresTime()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return li.QuadPart;
}

inline uint64 GetCycles()
{
	return __rdtsc();
}

#define BEGIN_PROFILE(n) 																			\
	int index##n = __COUNTER__;																		\
	uint64 startTime##n = GetHiresTime();															\
	uint64 startCycles##n = GetCycles();															\
	if(profile.records[index##n].callCount == 0)													\
	{																								\
		strncpy(profile.records[index##n].name, #n, PROFILE_NAME_LENGTH);							\
	}



#define END_PROFILE(n)																				\
	profile.records[index##n].cyclesTotal += GetCycles() - startCycles##n;							\
	profile.records[index##n].timeTotal += GetHiresTime() - startTime##n;							\
	profile.records[index##n].callCount++;


int  PrintProfile(char * buf)
{
	int length = 0;
	for(int i = 0; i < MAX_PROFILE_RECORDS; ++i)
	{
		if(profile.records[i].callCount)
		{
			double totalMS = profile.records[i].timeTotal / countsPerMs;
			length = wsprintf(buf, "%i: %s: %i, PC: %2.2f", i, profile.records[i].name, profile.records[i].callCount, totalMS / (double)profile.records[i].callCount);
			break;
		}
	}
	return length;
}