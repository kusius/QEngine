#ifndef Q_PLATFORM_H
#define Q_PLATFORM_H
#include <Windows.h>
#include <cstdint>

// CPS
extern int64_t performanceFrequency;
extern bool isInitialized;

int64_t GetCPUCycles();
void InitPlatform();
double TicksToMilliseconds(int64_t ticks);
double DiffTimeMilliseconds(int64_t start, int64_t end);

#endif // Q_PLATFORM_H