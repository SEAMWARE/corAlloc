//
// FILE            corAllocRealloc.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <string.h>                     // memcpy

#include "corBase/corLibLog.h"          // COR_LIB_*

#include "corAlloc/CorAllocStatus.h"    // CorAllocStatus
#include "corAlloc/corAllocInit.h"      // corAllocInit
#include "corAlloc/CorAllocTraceLevel.h"        // Trace Levels for corAlloc library
#include "corAlloc/corAllocHooks.h"     // CORALLOC_ERROR_HOOK
#include "corAlloc/CorAlloc.h"          // CorAlloc
#include "corAlloc/corAlloc.h"          // corAlloc
#include "corAlloc/corAllocRealloc.h"   // Own Interface



// -----------------------------------------------------------------------------
//
// corAllocRealloc -
//
char* corAllocRealloc(CorAlloc* kaP, char* origBuf, unsigned long long origSize, unsigned long long newSize)
{
  COR_LIB_T(CorAllocTraceAlloc, "%s re-allocating from %llu to %llu bytes", kaP->name, origSize, newSize);

  char* buf = corAlloc(kaP, newSize);

  if (buf == NULL)
  {
    CORALLOC_ERROR_HOOK(kaP, CorAllocAllocError, "Unable to allocate buffer", NULL);
    return NULL;
  }

  memcpy(buf, origBuf, origSize < newSize ? origSize : newSize);
  return buf;
}
