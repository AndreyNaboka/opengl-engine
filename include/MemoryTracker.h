#pragma once

#include <cstddef>

struct MemoryStats {
  std::size_t currentBytes = 0;
  std::size_t peakBytes = 0;
  std::size_t totalAllocatedBytes = 0;
  std::size_t allocationCount = 0;
};

class MemoryTracker {
public:
  static MemoryStats GetStats();
};
