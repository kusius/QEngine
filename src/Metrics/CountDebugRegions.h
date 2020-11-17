#ifndef COUNT_DEBUG_REGIONS_H
#define COUNT_DEBUG_REGIONS_H

#include <Platform/Win64Platform.h>

#include <cstdint>
#include <vector>
#include <string>

#define NUM_FRAMES_BATCH 60

#ifdef QDEBUG

#define BEGIN_DEBUG_REGION(ID) int64_t StartCycleCount_##ID = GetCPUCycles();
// TODO: Create a separate define for counted regions of a period of time (?)
#define END_DEBUG_REGION(ID)                                                   \
  TotalCycles[DebugCounted_##ID] += GetCPUCycles() - StartCycleCount_##ID;     \
  UpdateDebugRegion(DebugCounted_##ID);

#else
#define BEGIN_DEBUG_REGION(ID)
#define END_DEBUG_REGION(ID)
#endif

enum DebugRegions
{
  DebugCounted_Draw,
  DebugCounted_MainLoop,
  DebugCounted_ProcessInput,
  DebugCounted_UpdateWorldObjects,
  DebugCounted_UpdateDebugUI,
  DebugCounted_PollInput,
  NumRegions
};

extern int64_t TotalCycles[NumRegions];
extern int64_t AvgCycles[NumRegions];
extern int FrameCount[NumRegions];
extern const char *DebugRegionStrings[NumRegions];

void InitDebug();
void PrintDebugCounters();
void ResetDebugCounters();
void UpdateDebugRegion(int DebugRegion);
#endif // COUNT_DEBUG_REGIONS_H