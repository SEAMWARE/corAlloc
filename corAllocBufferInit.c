//
// FILE            corAllocBufferInit.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdint.h>                     // uintptr_t
#include <stdio.h>                      // NULL
#include <string.h>                     // strdup

#include "corBase/corLibLog.h"          // COR_LIB_*
#include "corAlloc/CorAllocTraceLevel.h"        // CorAllocTraceLevel
#include "corAlloc/corAllocInit.h"      // corAllocInit
#include "corAlloc/CorAlloc.h"          // CorAlloc
#include "corAlloc/corAllocHooks.h"     // CorAllocErrorHook
#include "corAlloc/corAllocBufferInit.h"        // Own interface



// -----------------------------------------------------------------------------
//
// corAllocBufferInit -
//
void corAllocBufferInit
(
  CorAlloc*          kaP,        // pointer to corAlloc buffer
  char*              buf,        // the initial buffer to use for allocations
  unsigned long long bufSize,    // the size of the initial buffer
  unsigned long long allocSize,  // the size to use for subsequent allocations, when the init-buffer runs out
  CorAllocErrorHook  errorHook,  // function pointer to be called when errors occur
  const char*        name        // name of the buffer
)
{
  if (kaP == NULL)
    return;

  //
  // The first allocation 8-aligned, whatever the alignment of the caller's buffer (a char array) - every
  // allocation is a multiple of 8 (corAlloc.c), so all of them are then. initBuf stays the caller's
  // buffer: corAllocBufferReset clears and re-inits it from there.
  //
  unsigned long long skip = (8 - ((unsigned long long) (uintptr_t) buf & 7)) & 7;

  if (skip > bufSize)
    skip = bufSize;

  kaP->initBuf       = buf;
  kaP->initBufSize   = bufSize;
  kaP->allocations   = 0;
  kaP->allocPointer  = buf + skip;
  kaP->bytesLeft     = bufSize - skip;
  kaP->errorHook     = errorHook;
  kaP->allocSize     = allocSize;
  kaP->allocList     = NULL;
  kaP->allocListTail = NULL;
  kaP->name          = (char*) name;
  kaP->threadSafe    = false;         // corAllocThreadSafe() opts in, after this

  COR_LIB_T(CorAllocTraceInit, "Initialized buffer at %p, allocSize is %llu", kaP->initBuf, kaP->allocSize);
}
