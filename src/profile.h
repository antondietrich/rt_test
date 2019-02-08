#include <cstdio>

#define PROFILE 1

#define PROFILE_NAME_MAX_LENGTH 64
#define MAX_PROFILE_RECORDS 1024
#define PROFILE_PER_THREAD_OUTPUT 0


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
	char name[PROFILE_NAME_MAX_LENGTH];
	uint64 callCount;
	uint64 timeTotal;
	uint64 cyclesTotal;
};

struct Profile
{
	ProfileRecord records[MAX_PROFILE_RECORDS][PROGRAM_THREAD_COUNT];
	inline ProfileRecord & operator[](int index)
	{
		return records[index][LOCAL_THREAD_ID];
	}
};

Profile profile = {};

void InitProfiler_()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	countsPerSecond = (double)freq.QuadPart;
	countsPerMs = ((double)freq.QuadPart / 1000.0);
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



struct ProfileProxyFast
{
	ProfileProxyFast(uint i, char * name)
	{
		index = i;
		startCycles = GetCycles();
		if(profile[index].callCount == 0)
		{
			profile[index] = {};
			profile[index].callCount++;
			strncpy(profile[index].name, name, PROFILE_NAME_MAX_LENGTH);
		}
	}

	~ProfileProxyFast()
	{
		profile[index].cyclesTotal += GetCycles() - startCycles;
		// uint64 y = GetCycles() - startCycles;
		profile[index].callCount++;
	}


	uint index;
	uint64 startCycles;
};

struct ProfileProxy : public ProfileProxyFast
{
	ProfileProxy(uint i, char * name) : ProfileProxyFast(i, name)
	{
		startTime = GetHiresTime();
	}
	~ProfileProxy()
	{
		profile[index].timeTotal += GetHiresTime() - startTime;
	}

	uint64 startTime;
};



int PrintProfile_(char * buf, uint bufLength)
{
	uint length = 0;

	length += _snprintf(buf + length, bufLength - length, "CC: Call Count\t TC: Time per Call\t TT: Time Total\tRD: rdtsc per call\n\n");

	for(int i = 0; i < MAX_PROFILE_RECORDS; ++i)
	{
		uint64 totalCallCount = 0;
#if !PROFILE_PER_THREAD_OUTPUT
		char * profileName = 0;
		uint profileIndex = 0;
		uint64 minTimePC = 0xffffffffffffffff;
		uint64 maxTimePC = 0;
		uint64 totalTime = 0;
		uint64 minCyclesPC = 0xffffffffffffffff;
		uint64 maxCyclesPC= 0;
		uint64 totalCycles = 0;
#endif
		for(int t = 0; t < gThreadCounter; ++t)
		{

			if(profile.records[i][t].callCount)
			{
				ProfileRecord r = profile.records[i][t];
				totalCallCount += r.callCount;

#if PROFILE_PER_THREAD_OUTPUT
				if(length < bufLength)
				{
					// length += _snprintf(buf + length, bufLength - length, "Thread %2i: %-20s\t", t, r.name);
					length += _snprintf(buf + length, bufLength - length, "Thread %2i:\t", t);

					if(r.callCount >= 1000000)
					{
						double callCount = r.callCount / 1000000.0;
						length += _snprintf(buf + length, bufLength - length, "CC: %6.2fM\t", callCount);
					}
					else
					{
						length += _snprintf(buf + length, bufLength - length, "CC: %6u \t", r.callCount);
					}

					double totalMs = r.timeTotal / countsPerMs;
					double usPerCall = (totalMs * 1000) / (double)r.callCount;

					if(usPerCall >= 10.0f)
					{
						length += _snprintf(buf + length, bufLength - length, "TC: %5.2fms\t", usPerCall / 1000);
					}
					else if(usPerCall >= 0.1f)
					{
						length += _snprintf(buf + length, bufLength - length, "TC: %5.2fus\t", usPerCall);
					}
					else
					{
						length += _snprintf(buf + length, bufLength - length, "TC: %5.3fns\t", usPerCall * 1000);
					}

					length += _snprintf(buf + length, bufLength - length, "TT: %4.1fs\t", totalMs / 1000);
					length += _snprintf(buf + length, bufLength - length, "RD: %llu\n", r.cyclesTotal / r.callCount);
				}

#else
				profileIndex = i;
				profileName = r.name;
				minTimePC = r.timeTotal < minTimePC ? r.timeTotal : minTimePC;
				maxTimePC = r.timeTotal > maxTimePC ? r.timeTotal : maxTimePC;
				totalTime += r.timeTotal;
				minCyclesPC = r.cyclesTotal < minCyclesPC ? r.cyclesTotal : minCyclesPC;
				maxCyclesPC = r.cyclesTotal > maxCyclesPC ? r.cyclesTotal : maxCyclesPC;
				totalCycles += r.cyclesTotal;
#endif
			}
		} // end for each thread

		if(totalCallCount > 0)
		{
#if PROFILE_PER_THREAD_OUTPUT
			length += _snprintf(buf + length, bufLength - length, "\t\tCall total: %u\n", totalCallCount);
#else
			double totalMs = totalTime / countsPerMs;
			double usPerCall = (totalMs * 1000) / (double)totalCallCount;

			length += _snprintf(buf + length, bufLength - length, "%-20s\t", profileName);
			// length += _snprintf(buf + length, bufLength - length, "%u\t", profileIndex);

			if(totalCallCount >= 1000000)
			{
				length += _snprintf(buf + length, bufLength - length, "CC: %6.2fM\t", totalCallCount / 1000000.0);
			}
			else
			{
				length += _snprintf(buf + length, bufLength - length, "CC: %6llu \t", totalCallCount);
			}

			if(usPerCall >= 10.0f)
			{
				length += _snprintf(buf + length, bufLength - length, "TC: %5.2fms\t", usPerCall / 1000);
			}
			else if(usPerCall >= 0.1f)
			{
				length += _snprintf(buf + length, bufLength - length, "TC: %5.2fus\t", usPerCall);
			}
			else
			{
				length += _snprintf(buf + length, bufLength - length, "TC: %5.3fns\t", usPerCall * 1000);
			}

			// length += _snprintf(buf + length, bufLength - length, "TT: %4.1fs\t", totalMs / 1000);
			length += _snprintf(buf + length, bufLength - length, "RD: %llu\n", totalCycles / totalCallCount);
#endif
		}
	}
	return length;
}


#ifdef PROFILE

	#define SCOPED_PROFILE(n) ProfileProxy proxy##n = ProfileProxy(__COUNTER__, n)
	#define PROFILED_FUNCTION SCOPED_PROFILE(__FUNCTION__)
	#define SCOPED_PROFILE_FAST(n) ProfileProxyFast proxy##n = ProfileProxyFast(__COUNTER__, n)
	#define PROFILED_FUNCTION_FAST SCOPED_PROFILE_FAST(__FUNCTION__)

	#define BEGIN_PROFILE(n) 																			\
		int index##n = __COUNTER__;																		\
		uint64 startTime##n = GetHiresTime();															\
		uint64 startCycles##n = GetCycles();															\
		if(profile[index##n].callCount == 0)															\
		{																								\
			profile[index##n] = {};																		\
			strncpy(profile[index##n].name, #n, PROFILE_NAME_MAX_LENGTH);								\
		}



	#define END_PROFILE(n)																				\
		profile[index##n].cyclesTotal += GetCycles() - startCycles##n;									\
		profile[index##n].timeTotal += GetHiresTime() - startTime##n;									\
		profile[index##n].callCount++;

	#define PrintProfile(b, l) PrintProfile_((b), (l))
	#define InitProfiler() InitProfiler_()

#else
	#define SCOPED_PROFILE(n)
	#define PROFILED_FUNCTION
	#define SCOPED_PROFILE_FAST(n)
	#define PROFILED_FUNCTION_FAST
	#define BEGIN_PROFILE(n)
	#define END_PROFILE(n)
	#define InitProfiler()
	#define PrintProfile(b, l) 0
#endif
