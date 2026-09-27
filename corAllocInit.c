//
// FILE            corAllocInit.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//

#include <stdbool.h>                    // bool

#include "corAlloc/CorAllocStatus.h"    // CorAllocStatus
#include "corAlloc/corAllocLog.h"       // CorAllocLogFunction
#include "corAlloc/CorAllocTraceLevel.h"        // CorAllocTraceLevel


static bool   initialized = false;



// -----------------------------------------------------------------------------
//
// corAllocInit - initialize the corAlloc library
//
CorAllocStatus corAllocInit(CorAllocLogFunction logFunction)
{
  if (initialized == true)
    return CorAllocAlreadyInitialized;

  //
  // Assign the "library log function", so that the logs are tied to the executable that the library is part of
  //
  corAllocLogFunction = logFunction;

  initialized = true;
  return CorAllocOk;
}
