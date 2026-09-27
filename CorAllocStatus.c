//
// FILE            CorAllocStatus.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include "corAlloc/CorAllocStatus.h"         // Own interface



// -----------------------------------------------------------------------------
//
// corAllocStatus - 
//
const char* corAllocStatus(CorAllocStatus kas)
{
  switch (kas)
  {
  case CorAllocOk:            return "OK";
  case CorAllocLogComponentError:  return "Error registering log component";  // kept: public enum member, no longer returned
  case CorAllocAllocError:    return "Alloc Error";
  case CorAllocBufSizeTooSmall:    return "Buf Size Too Small";
  case CorAllocAlreadyInitialized: return "Already Initialized";
  }

  return "Unknown CorAllocStatus";
}
