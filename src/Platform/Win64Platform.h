#ifndef Q_PLATFORM_H
#define Q_PLATFORM_H
#include <Windows.h>
#include <cstdint>
#include <string>
#include <vector>

// CPS
extern int64_t performanceFrequency;
extern bool isInitialized;

int64_t GetCPUCycles();
void InitPlatform();
double TicksToMilliseconds(int64_t ticks);
double DiffTimeMilliseconds(int64_t start, int64_t end);
bool ListFiles(std::string path, std::string mask,
               std::vector<std::string> &files);

#endif // Q_PLATFORM_H