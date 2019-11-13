// License Summary: MIT see LICENSE file
#include "al2o3_platform/platform.h"
#include "al2o3_platform/windows.h"

static int64_t highResTimerFrequency = 0;
static void initTime()
{
	LARGE_INTEGER frequency;
	if (QueryPerformanceFrequency(&frequency))
	{
		highResTimerFrequency = frequency.QuadPart;
	}
	else
	{
		highResTimerFrequency = 1000LL;
	}
}

static int64_t getTimerFrequency()
{
	if (highResTimerFrequency == 0)
		initTime();

	return highResTimerFrequency;
}

AL2O3_EXTERN_C int64_t Os_GetUSec()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart * (int64_t)1e6 / getTimerFrequency();
}
