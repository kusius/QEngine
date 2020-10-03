#ifndef Q_PLATFORM_H
#define Q_PLATFORM_H
#include <Windows.h>
#include <cstdint>

namespace Platform
{
    int64_t GetWallClockTime()
    {
        LARGE_INTEGER clockTime;
        QueryPerformanceCounter(&clockTime);
        return clockTime.QuadPart;
    }

    double DiffTimeMilliseconds(int64_t start, int64_t end)
    {
        return ((double)end - (double)start) / 10000;
    }
} // namespace Platform

#endif // Q_PLATFORM_H