//
// FILE            corAllocMem.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019-2026 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORALLOC_CORALLOCMEM_H_
#define CORALLOC_CORALLOCMEM_H_

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif



// -----------------------------------------------------------------------------
//
// CorAllocMemStats - statistics about memory usage
//
typedef struct CorAllocMemStats
{
  size_t  limit;           // configured memory limit (0 = unlimited)
  size_t  allocated;       // currently allocated bytes
  size_t  highWaterMark;   // peak allocation
  size_t  totalAllocated;  // total bytes allocated over lifetime
  size_t  totalFreed;      // total bytes freed over lifetime
  size_t  allocCalls;      // number of alloc calls
  size_t  freeCalls;       // number of free calls
  size_t  waitCount;       // number of times allocation had to wait
} CorAllocMemStats;



// -----------------------------------------------------------------------------
//
// corAllocMemInit - initialize memory budget tracking
//
// @param limit      maximum bytes allowed (0 = unlimited)
// @param softLimit  percentage (0-100) at which to start applying backpressure
//                   e.g., 80 means start slowing down at 80% of limit
//
extern void corAllocMemInit(size_t limit, int softLimitPercent);



// -----------------------------------------------------------------------------
//
// corAllocMemLimitSet - change the memory limit at runtime
//
extern void corAllocMemLimitSet(size_t limit);



// -----------------------------------------------------------------------------
//
// corAllocMemAlloc - allocate memory with budget tracking
//
// If the allocation would exceed the limit, this function will:
// 1. Wait for memory to be freed (with timeout)
// 2. Return NULL if timeout expires and memory is still not available
//
// @param size    bytes to allocate
// @return        pointer to allocated memory, or NULL on failure
//
extern void* corAllocMemAlloc(size_t size);



// -----------------------------------------------------------------------------
//
// corAllocMemCalloc - allocate zeroed memory with budget tracking
//
extern void* corAllocMemCalloc(size_t n, size_t size);



// -----------------------------------------------------------------------------
//
// corAllocMemRealloc - reallocate memory with budget tracking
//
// IMPORTANT: oldSize must be provided because we track allocated bytes
//            and realloc doesn't tell us the old size
//
// @param ptr      pointer to existing memory (or NULL for new allocation)
// @param oldSize  size of existing allocation (0 if ptr is NULL)
// @param newSize  new desired size
// @return         pointer to reallocated memory, or NULL on failure
//
extern void* corAllocMemRealloc(void* ptr, size_t oldSize, size_t newSize);



// -----------------------------------------------------------------------------
//
// corAllocMemFree - free memory with budget tracking
//
// @param ptr   pointer to memory to free
// @param size  size of the allocation being freed
//
extern void corAllocMemFree(void* ptr, size_t size);



// -----------------------------------------------------------------------------
//
// corAllocMemStrdup - duplicate string with budget tracking
//
extern char* corAllocMemStrdup(const char* s);



// -----------------------------------------------------------------------------
//
// corAllocMemStrndup - duplicate string with length limit and budget tracking
//
extern char* corAllocMemStrndup(const char* s, size_t n);



// -----------------------------------------------------------------------------
//
// corAllocMemStatsGet - get current memory statistics
//
extern void corAllocMemStatsGet(CorAllocMemStats* statsP);



// -----------------------------------------------------------------------------
//
// corAllocMemAvailable - check if size bytes can be allocated without blocking
//
extern bool corAllocMemAvailable(size_t size);



// -----------------------------------------------------------------------------
//
// corAllocMemWaitTimeoutSet - set the timeout for waiting on memory (milliseconds)
//
// Default is 5000ms (5 seconds). Set to 0 to fail immediately when limit reached.
//
extern void corAllocMemWaitTimeoutSet(int timeoutMs);



#ifdef __cplusplus
}
#endif

#endif   // CORALLOC_CORALLOCMEM_H_
