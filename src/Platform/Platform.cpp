#include "Win64Platform.h"
#include <stack>
#include <iostream>

int64_t performanceFrequency = -1;
bool isInitialized           = false;

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
    isInitialized        = true;
}

bool ListFiles(std::string path, std::string mask,
               std::vector<std::string> &files)
{
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
    std::string spec;
    std::stack<std::string> directories;

    directories.push(path);
    files.clear();

    while (!directories.empty())
    {
        path = directories.top();
        spec = path + "\\" + mask;
        directories.pop();

        hFind = FindFirstFile(spec.c_str(), &ffd);
        if (hFind == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        do
        {
            if (strcmp(ffd.cFileName, ".") != 0 &&
                strcmp(ffd.cFileName, "..") != 0)
            {
                if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    directories.push(path + "\\" + ffd.cFileName);
                }
                else
                {
                    files.push_back(path + "\\" + ffd.cFileName);
                }
            }
        } while (FindNextFile(hFind, &ffd) != 0);

        if (GetLastError() != ERROR_NO_MORE_FILES)
        {
            FindClose(hFind);
            return false;
        }

        FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
    }

    return true;
}