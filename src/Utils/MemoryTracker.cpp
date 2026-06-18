#include "Utils/MemoryTracker.h"

#include <atomic>
#include <cstdlib>
#include <limits>
#include <new>

namespace {
struct AllocationHeader {
  std::size_t size;
  void *raw;
};

std::atomic_size_t g_currentBytes{0};
std::atomic_size_t g_peakBytes{0};
std::atomic_size_t g_totalAllocatedBytes{0};
std::atomic_size_t g_allocationCount{0};

void UpdatePeak(std::size_t currentBytes) {
  std::size_t peakBytes = g_peakBytes.load(std::memory_order_relaxed);
  while (currentBytes > peakBytes &&
         !g_peakBytes.compare_exchange_weak(peakBytes, currentBytes,
                                            std::memory_order_relaxed)) {
  }
}

void TrackAllocation(std::size_t size) {
  const std::size_t currentBytes =
      g_currentBytes.fetch_add(size, std::memory_order_relaxed) + size;
  g_totalAllocatedBytes.fetch_add(size, std::memory_order_relaxed);
  g_allocationCount.fetch_add(1, std::memory_order_relaxed);
  UpdatePeak(currentBytes);
}

void TrackFree(void *ptr) noexcept {
  if (!ptr)
    return;

  AllocationHeader *header = reinterpret_cast<AllocationHeader *>(ptr) - 1;
  g_currentBytes.fetch_sub(header->size, std::memory_order_relaxed);
  std::free(header->raw);
}

void *Allocate(std::size_t size) {
  if (size == 0)
    size = 1;

  if (size >
      std::numeric_limits<std::size_t>::max() - sizeof(AllocationHeader)) {
    throw std::bad_alloc();
  }

  void *raw = std::malloc(sizeof(AllocationHeader) + size);
  if (!raw)
    throw std::bad_alloc();

  AllocationHeader *header = static_cast<AllocationHeader *>(raw);
  header->size = size;
  header->raw = raw;
  TrackAllocation(size);
  return header + 1;
}

void *AllocateAligned(std::size_t size, std::size_t alignment) {
  if (size == 0)
    size = 1;

  const std::size_t overhead = sizeof(AllocationHeader) + alignment - 1;
  if (alignment == 0 ||
      size > std::numeric_limits<std::size_t>::max() - overhead) {
    throw std::bad_alloc();
  }

  void *raw = std::malloc(size + overhead);
  if (!raw)
    throw std::bad_alloc();

  const auto rawAddress = reinterpret_cast<std::uintptr_t>(raw);
  const auto dataStart = rawAddress + sizeof(AllocationHeader);
  const auto alignedAddress = (dataStart + alignment - 1) &
                              ~(static_cast<std::uintptr_t>(alignment) - 1);
  AllocationHeader *header =
      reinterpret_cast<AllocationHeader *>(alignedAddress) - 1;
  header->size = size;
  header->raw = raw;
  TrackAllocation(size);
  return reinterpret_cast<void *>(alignedAddress);
}
} // namespace

MemoryStats MemoryTracker::GetStats() {
  return {g_currentBytes.load(std::memory_order_relaxed),
          g_peakBytes.load(std::memory_order_relaxed),
          g_totalAllocatedBytes.load(std::memory_order_relaxed),
          g_allocationCount.load(std::memory_order_relaxed)};
}

void *operator new(std::size_t size) { return Allocate(size); }

void *operator new[](std::size_t size) { return Allocate(size); }

void *operator new(std::size_t size, std::align_val_t alignment) {
  return AllocateAligned(size, static_cast<std::size_t>(alignment));
}

void *operator new[](std::size_t size, std::align_val_t alignment) {
  return AllocateAligned(size, static_cast<std::size_t>(alignment));
}

void *operator new(std::size_t size, const std::nothrow_t &) noexcept {
  try {
    return Allocate(size);
  } catch (...) {
    return nullptr;
  }
}

void *operator new[](std::size_t size, const std::nothrow_t &) noexcept {
  try {
    return Allocate(size);
  } catch (...) {
    return nullptr;
  }
}

void operator delete(void *ptr) noexcept { TrackFree(ptr); }

void operator delete[](void *ptr) noexcept { TrackFree(ptr); }

void operator delete(void *ptr, std::size_t) noexcept { TrackFree(ptr); }

void operator delete[](void *ptr, std::size_t) noexcept { TrackFree(ptr); }

void operator delete(void *ptr, std::align_val_t) noexcept { TrackFree(ptr); }

void operator delete[](void *ptr, std::align_val_t) noexcept { TrackFree(ptr); }

void operator delete(void *ptr, std::size_t, std::align_val_t) noexcept {
  TrackFree(ptr);
}

void operator delete[](void *ptr, std::size_t, std::align_val_t) noexcept {
  TrackFree(ptr);
}

void operator delete(void *ptr, const std::nothrow_t &) noexcept {
  TrackFree(ptr);
}

void operator delete[](void *ptr, const std::nothrow_t &) noexcept {
  TrackFree(ptr);
}
