//
// FILE            corAlloc.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdlib.h>                     // calloc
#include <string.h>                     // memset
#include <unistd.h>                     // usleep

#include "corBase/corLibLog.h"          // COR_LIB_*
#include "corAlloc/CorAllocStatus.h"    // CorAllocStatus
#include "corAlloc/corAllocInit.h"      // corAllocInit
#include "corAlloc/CorAllocTraceLevel.h"        // Trace Levels for corAlloc library
#include "corAlloc/corAllocHooks.h"     // CORALLOC_ERROR_HOOK
#include "corAlloc/CorAlloc.h"          // CorAlloc
#include "corAlloc/corAllocLog.h"       // CORALLOC_*
#include "corAlloc/corAlloc.h"          // Own Interface



// -----------------------------------------------------------------------------
//
// allocate - the allocation itself (the caller holds the mutex if kaP is thread-safe)
//
static char* allocate(CorAlloc* kaP, unsigned long long size)
{
  //
  // Oversized allocations: any single item that wouldn't fit in a regular
  // chunk gets its own dedicated buffer. We still thread it through allocList
  // so corAllocBufferReset() frees it along with the normal chunks — no separate
  // tracking, no leak. Current chunk state (allocPointer/bytesLeft) is
  // intentionally left untouched, so subsequent small allocations keep
  // draining the chunk we were already on.
  //
  if (size >= kaP->allocSize - sizeof(CorAllocBuffer))
  {
    unsigned long long  total = size + sizeof(CorAllocBuffer);
    CorAllocBuffer*     kabP  = (CorAllocBuffer*) calloc(1, total);

    if (kabP == NULL)
    {
      CORALLOC_ERROR_HOOK(kaP, CorAllocAllocError, "Unable to allocate oversized buffer", NULL);
      return NULL;
    }

    kabP->toFree = (char*) kabP;
    kabP->next   = NULL;

    if (kaP->allocList == NULL)
      kaP->allocList = kabP;
    else
      kaP->allocListTail->next = kabP;

    kaP->allocListTail = kabP;

    return (char*) (kabP + 1);  // data region starts right after the header
  }

  //
  // Take a chunk from the allocation buffer (kaP->allocPointer)
  //
  if (kaP->bytesLeft >= size)
  {
    char* start = kaP->allocPointer;  // As kaP->allocPointer is set to NEXT chunk in the next line

    kaP->allocPointer   += size;
    kaP->bytesLeft      -= size;

    memset(start, 0, size);
    return start;
  }

  // COR_LIB_I("KALL: ALLOCATING ADDITIONAL BUFFER of %d bytes (using calloc)", kaP->allocSize);
  kaP->allocPointer = (char*) calloc(1, kaP->allocSize);
  if (kaP->allocPointer == NULL)
  {
    int retries = 0;
    
    while (kaP->allocPointer == NULL)
    {
      usleep(100);
      kaP->allocPointer = (char*) calloc(1, kaP->allocSize);
      ++retries;
      if ((kaP->allocPointer == NULL) && (retries > 100))
      {
        COR_LIB_E("out of memory: calloc returned NULL, 100 times");
        CORALLOC_ERROR_HOOK(kaP, CorAllocAllocError, "Unable to allocate buffer", NULL);
        return NULL;
      }
    }
  }
  kaP->bytesLeft = kaP->allocSize;
  // COR_LIB_I("KALL: right after allocating more buffer: %d bytes left", kaP->bytesLeft);

  //
  // Save pointer to allocated buffer in kaP->allocList
  //
  // [ First, room for CorAllocBuffer is allocated in the beginning of the newly allocated buffer ]
  //
  CorAllocBuffer* newKabP = (CorAllocBuffer*) kaP->allocPointer;

  kaP->allocPointer += sizeof(CorAllocBuffer);
  kaP->bytesLeft    -= sizeof(CorAllocBuffer);

  newKabP->toFree = (char*) newKabP;
  newKabP->next   = NULL;

  if (kaP->allocList == NULL)
    kaP->allocList = newKabP;
  else
    kaP->allocListTail->next = newKabP;

  kaP->allocListTail = newKabP;
  
  char* start = kaP->allocPointer;  // As kaP->allocPointer is set to NEXT chunk in the next line
  
  // Now, position the allocPointer for the next call ...
  kaP->allocPointer   += size;

  // ... and count off the size of the chunk just given away
  kaP->bytesLeft      -= size;

  // COR_LIB_I("KALL: end-of-function: returning a buf od %d bytes at %p, and bytesLeft: %d", size, start, kaP->bytesLeft);

  return start;
}



// -----------------------------------------------------------------------------
//
// corAlloc -
//
// A buffer allocator is not thread-safe, and almost never needs to be: nearly every
// one is a request's own arena, used by one thread. The exception is an allocator
// SHARED between threads - the JSON-LD context store, into which every thread that
// downloads or parses a context allocates. Without the mutex, two threads doing that
// at once (two different uncached @contexts) raced on allocPointer / bytesLeft /
// allocList: overlapping allocations and a corrupted block list. Such an allocator
// calls corAllocThreadSafe() once, after corAllocBufferInit; every other one pays a
// branch.
//
char* corAlloc(CorAlloc* kaP, unsigned long long size)
{
  if (kaP == NULL)
    return NULL;

  if (kaP->threadSafe == false)
    return allocate(kaP, size);

  pthread_mutex_lock(&kaP->mutex);
  char* bufP = allocate(kaP, size);
  pthread_mutex_unlock(&kaP->mutex);

  return bufP;
}



// -----------------------------------------------------------------------------
//
// corAllocThreadSafe - from now on, every allocation from kaP takes kaP's mutex
//
// Call once, after corAllocBufferInit and before a second thread can reach kaP. What
// this protects is the allocator; what the allocations are used for is the caller's
// business (two threads building two different objects need nothing more).
//
void corAllocThreadSafe(CorAlloc* kaP)
{
  if (kaP == NULL)
    return;

  pthread_mutex_init(&kaP->mutex, NULL);
  kaP->threadSafe = true;
}
