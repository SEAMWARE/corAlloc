//
// FILE            corAllocMem.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019-2026 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

#include "corAlloc/corAllocMem.h"



// -----------------------------------------------------------------------------
//
// Global state for memory tracking
//
static struct
{
  pthread_mutex_t  mutex;
  pthread_cond_t   cond;
  size_t           limit;           // 0 = unlimited
  size_t           softLimit;       // start backpressure at this level
  size_t           allocated;
  size_t           highWaterMark;
  size_t           totalAllocated;
  size_t           totalFreed;
  size_t           allocCalls;
  size_t           freeCalls;
  size_t           waitCount;
  int              waitTimeoutMs;
  bool             initialized;
} corAllocMemState = {
  .mutex          = PTHREAD_MUTEX_INITIALIZER,
  .cond           = PTHREAD_COND_INITIALIZER,
  .limit          = 0,
  .softLimit      = 0,
  .allocated      = 0,
  .highWaterMark  = 0,
  .totalAllocated = 0,
  .totalFreed     = 0,
  .allocCalls     = 0,
  .freeCalls      = 0,
  .waitCount      = 0,
  .waitTimeoutMs  = 5000,
  .initialized    = false
};



// -----------------------------------------------------------------------------
//
// corAllocMemInit -
//
void corAllocMemInit(size_t limit, int softLimitPercent)
{
  pthread_mutex_lock(&corAllocMemState.mutex);

  corAllocMemState.limit    = limit;
  corAllocMemState.softLimit      = (limit * softLimitPercent) / 100;
  corAllocMemState.allocated      = 0;
  corAllocMemState.highWaterMark  = 0;
  corAllocMemState.totalAllocated = 0;
  corAllocMemState.totalFreed     = 0;
  corAllocMemState.allocCalls     = 0;
  corAllocMemState.freeCalls      = 0;
  corAllocMemState.waitCount      = 0;
  corAllocMemState.initialized    = true;

  pthread_mutex_unlock(&corAllocMemState.mutex);
}



// -----------------------------------------------------------------------------
//
// corAllocMemLimitSet -
//
void corAllocMemLimitSet(size_t limit)
{
  pthread_mutex_lock(&corAllocMemState.mutex);
  corAllocMemState.limit = limit;
  // Recalculate soft limit assuming same percentage
  if (corAllocMemState.softLimit > 0 && limit > 0)
  {
    // Preserve the percentage ratio
    corAllocMemState.softLimit = (limit * 80) / 100;  // Default 80%
  }
  pthread_cond_broadcast(&corAllocMemState.cond);  // Wake waiters in case limit increased
  pthread_mutex_unlock(&corAllocMemState.mutex);
}



// -----------------------------------------------------------------------------
//
// corAllocMemWaitTimeoutSet -
//
void corAllocMemWaitTimeoutSet(int timeoutMs)
{
  pthread_mutex_lock(&corAllocMemState.mutex);
  corAllocMemState.waitTimeoutMs = timeoutMs;
  pthread_mutex_unlock(&corAllocMemState.mutex);
}



// -----------------------------------------------------------------------------
//
// waitForMemory - internal function to wait until memory is available
//
// Returns true if memory is available, false if timeout
// Must be called with mutex held
//
static bool waitForMemory(size_t size)
{
  // No limit set - always available
  if (corAllocMemState.limit == 0)
    return true;

  // Check if allocation would exceed limit
  if (corAllocMemState.allocated + size <= corAllocMemState.limit)
    return true;

  // Memory not available - need to wait
  if (corAllocMemState.waitTimeoutMs == 0)
    return false;  // No waiting configured

  corAllocMemState.waitCount++;

  // Calculate absolute timeout
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec  += corAllocMemState.waitTimeoutMs / 1000;
  ts.tv_nsec += (corAllocMemState.waitTimeoutMs % 1000) * 1000000;
  if (ts.tv_nsec >= 1000000000)
  {
    ts.tv_sec++;
    ts.tv_nsec -= 1000000000;
  }

  // Wait for memory to be freed
  while (corAllocMemState.allocated + size > corAllocMemState.limit)
  {
    int rc = pthread_cond_timedwait(&corAllocMemState.cond, &corAllocMemState.mutex, &ts);
    if (rc == ETIMEDOUT)
      return false;
    if (rc != 0 && rc != ETIMEDOUT)
      return false;  // Other error
  }

  return true;
}



// -----------------------------------------------------------------------------
//
// corAllocMemAlloc -
//
void* corAllocMemAlloc(size_t size)
{
  if (size == 0)
    return NULL;

  pthread_mutex_lock(&corAllocMemState.mutex);

  // Wait for memory if needed
  if (!waitForMemory(size))
  {
    pthread_mutex_unlock(&corAllocMemState.mutex);
    return NULL;
  }

  // Allocate
  void* ptr = malloc(size);
  if (ptr == NULL)
  {
    pthread_mutex_unlock(&corAllocMemState.mutex);
    return NULL;
  }

  // Update stats
  corAllocMemState.allocated += size;
  corAllocMemState.totalAllocated += size;
  corAllocMemState.allocCalls++;

  if (corAllocMemState.allocated > corAllocMemState.highWaterMark)
    corAllocMemState.highWaterMark = corAllocMemState.allocated;

  pthread_mutex_unlock(&corAllocMemState.mutex);

  return ptr;
}



// -----------------------------------------------------------------------------
//
// corAllocMemCalloc -
//
void* corAllocMemCalloc(size_t n, size_t size)
{
  size_t total = n * size;
  if (total == 0)
    return NULL;

  pthread_mutex_lock(&corAllocMemState.mutex);

  // Wait for memory if needed
  if (!waitForMemory(total))
  {
    pthread_mutex_unlock(&corAllocMemState.mutex);
    return NULL;
  }

  // Allocate
  void* ptr = calloc(n, size);
  if (ptr == NULL)
  {
    pthread_mutex_unlock(&corAllocMemState.mutex);
    return NULL;
  }

  // Update stats
  corAllocMemState.allocated += total;
  corAllocMemState.totalAllocated += total;
  corAllocMemState.allocCalls++;

  if (corAllocMemState.allocated > corAllocMemState.highWaterMark)
    corAllocMemState.highWaterMark = corAllocMemState.allocated;

  pthread_mutex_unlock(&corAllocMemState.mutex);

  return ptr;
}



// -----------------------------------------------------------------------------
//
// corAllocMemRealloc -
//
void* corAllocMemRealloc(void* ptr, size_t oldSize, size_t newSize)
{
  // Handle special cases
  if (ptr == NULL)
    return corAllocMemAlloc(newSize);

  if (newSize == 0)
  {
    corAllocMemFree(ptr, oldSize);
    return NULL;
  }

  pthread_mutex_lock(&corAllocMemState.mutex);

  // Calculate the difference
  if (newSize > oldSize)
  {
    size_t diff = newSize - oldSize;
    if (!waitForMemory(diff))
    {
      pthread_mutex_unlock(&corAllocMemState.mutex);
      return NULL;
    }
  }

  // Reallocate
  void* newPtr = realloc(ptr, newSize);
  if (newPtr == NULL)
  {
    pthread_mutex_unlock(&corAllocMemState.mutex);
    return NULL;
  }

  // Update stats
  corAllocMemState.allocated = corAllocMemState.allocated - oldSize + newSize;
  if (newSize > oldSize)
    corAllocMemState.totalAllocated += (newSize - oldSize);
  else
    corAllocMemState.totalFreed += (oldSize - newSize);

  if (corAllocMemState.allocated > corAllocMemState.highWaterMark)
    corAllocMemState.highWaterMark = corAllocMemState.allocated;

  // Signal waiters if we shrank
  if (newSize < oldSize)
    pthread_cond_signal(&corAllocMemState.cond);

  pthread_mutex_unlock(&corAllocMemState.mutex);

  return newPtr;
}



// -----------------------------------------------------------------------------
//
// corAllocMemFree -
//
void corAllocMemFree(void* ptr, size_t size)
{
  if (ptr == NULL)
    return;

  free(ptr);

  pthread_mutex_lock(&corAllocMemState.mutex);

  if (size <= corAllocMemState.allocated)
    corAllocMemState.allocated -= size;
  else
    corAllocMemState.allocated = 0;  // Shouldn't happen, but be safe

  corAllocMemState.totalFreed += size;
  corAllocMemState.freeCalls++;

  // Signal any threads waiting for memory
  pthread_cond_signal(&corAllocMemState.cond);

  pthread_mutex_unlock(&corAllocMemState.mutex);
}



// -----------------------------------------------------------------------------
//
// corAllocMemStrdup -
//
char* corAllocMemStrdup(const char* s)
{
  if (s == NULL)
    return NULL;

  size_t len = strlen(s) + 1;
  char* copy = (char*) corAllocMemAlloc(len);
  if (copy != NULL)
    memcpy(copy, s, len);

  return copy;
}



// -----------------------------------------------------------------------------
//
// corAllocMemStrndup -
//
char* corAllocMemStrndup(const char* s, size_t n)
{
  if (s == NULL)
    return NULL;

  size_t len = strlen(s);
  if (len > n)
    len = n;

  char* copy = (char*) corAllocMemAlloc(len + 1);
  if (copy != NULL)
  {
    memcpy(copy, s, len);
    copy[len] = '\0';
  }

  return copy;
}



// -----------------------------------------------------------------------------
//
// corAllocMemStatsGet -
//
void corAllocMemStatsGet(CorAllocMemStats* statsP)
{
  if (statsP == NULL)
    return;

  pthread_mutex_lock(&corAllocMemState.mutex);

  statsP->limit          = corAllocMemState.limit;
  statsP->allocated      = corAllocMemState.allocated;
  statsP->highWaterMark  = corAllocMemState.highWaterMark;
  statsP->totalAllocated = corAllocMemState.totalAllocated;
  statsP->totalFreed     = corAllocMemState.totalFreed;
  statsP->allocCalls     = corAllocMemState.allocCalls;
  statsP->freeCalls      = corAllocMemState.freeCalls;
  statsP->waitCount      = corAllocMemState.waitCount;

  pthread_mutex_unlock(&corAllocMemState.mutex);
}



// -----------------------------------------------------------------------------
//
// corAllocMemAvailable -
//
bool corAllocMemAvailable(size_t size)
{
  if (corAllocMemState.limit == 0)
    return true;

  pthread_mutex_lock(&corAllocMemState.mutex);
  bool available = (corAllocMemState.allocated + size <= corAllocMemState.limit);
  pthread_mutex_unlock(&corAllocMemState.mutex);

  return available;
}
