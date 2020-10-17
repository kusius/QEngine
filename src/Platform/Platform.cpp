#include "Win64Platform.h"

int64_t performanceFrequency = -1;
bool isInitialized = false;

double TicksToMilliseconds(int64_t ticks)
{
  if (isInitialized)
    return 1000.0 * ticks / performanceFrequency;
  else
    return 0.0;
}

int64_t GetCPUCycles()
{
  LARGE_INTEGER clockCycles;
  QueryPerformanceCounter(&clockCycles);
  return clockCycles.QuadPart;
}

double DiffTimeMilliseconds(int64_t start, int64_t end)
{
  return ((double)end - (double)start) / 10000;
}

void InitPlatform()
{
  LARGE_INTEGER freq;
  QueryPerformanceFrequency(&freq);
  performanceFrequency = freq.QuadPart;
  isInitialized = true;
}