#include <Platform/Win64Platform.h>
#include <iostream>
#include "CountDebugRegions.h"

using std::cout;
using std::endl;

int64_t TotalCycles[NumRegions];
int64_t AvgCycles[NumRegions];
int FrameCount[NumRegions];
const char *DebugRegionStrings[] = {"Draw",
                                    "Main Loop",
                                    "Process Input",
                                    "Update World Objects",
                                    "Update Debug UI",
                                    "Poll Inputs"};
void InitDebug()
{
  std::fill_n(std::begin(TotalCycles), NumRegions, 0);
  std::fill_n(std::begin(FrameCount), NumRegions, 0);
  std::fill_n(std::begin(AvgCycles), NumRegions, 0);
}

void PrintDebugCounters()
{
  for (unsigned short region = 0; region < NumRegions; region++)
  {
    cout << "[DEBUG] " << DebugRegionStrings[region] << ": "
         << TotalCycles[region] << " cycles" << endl;
  }
}

void ResetDebugCounters()
{
  std::fill_n(std::begin(TotalCycles), NumRegions, 0);
  std::fill_n(std::begin(FrameCount), NumRegions, 1);
}

void UpdateDebugRegion(int DebugRegion)
{
  if (++FrameCount[DebugRegion] > NUM_FRAMES_BATCH)
  {
    AvgCycles[DebugRegion] = TotalCycles[DebugRegion] / FrameCount[DebugRegion];
    FrameCount[DebugRegion] = 0;
    TotalCycles[DebugRegion] = 0;
  }
}