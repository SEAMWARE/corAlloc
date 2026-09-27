//
// FILE            CorAllocTraceLevel.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORALLOC_CORALLOCTRACELEVEL_H_
#define CORALLOC_CORALLOCTRACELEVEL_H_



// -----------------------------------------------------------------------------
//
// CorAllocTraceLevel
//
typedef enum CorAllocTraceLevel
{
  CorAllocTraceInit,
  CorAllocTraceNewBuffer,
  CorAllocTraceAllocBytesLeft,
  CorAllocTraceAlloc,
  CorAllocTraceLast
} CorAllocTraceLevel;

#endif  // CORALLOC_CORALLOCTRACELEVEL_H_
