//
// FILE            CorAllocStatus.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORALLOC_CORALLOCSTATUS_H_
#define CORALLOC_CORALLOCSTATUS_H_



// -----------------------------------------------------------------------------
//
// KjStatus - 
//
typedef enum CorAllocStatus
{
  CorAllocOk,
  CorAllocLogComponentError,
  CorAllocAllocError,
  CorAllocBufSizeTooSmall,
  CorAllocAlreadyInitialized
} CorAllocStatus;



// -----------------------------------------------------------------------------
//
// corAllocStatus - 
//
extern const char* corAllocStatus(CorAllocStatus kas);

#endif  // CORALLOC_CORALLOCSTATUS_H_
