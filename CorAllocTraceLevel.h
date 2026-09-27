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
// The trace levels are the EXECUTABLE's number space - COR_LIB_T hands them to its
// log function - and every library takes a hundred of its own: corAlloc's is 300.
//
typedef enum CorAllocTraceLevel
{
  CorAllocTraceInit = 300,
  CorAllocTraceNewBuffer,
  CorAllocTraceAllocBytesLeft,
  CorAllocTraceAlloc,
  CorAllocTraceLast
} CorAllocTraceLevel;

#endif  // CORALLOC_CORALLOCTRACELEVEL_H_
