//
// FILE            corAllocBufferInit.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORALLOC_CORALLOCBUFFERINIT_H_
#define CORALLOC_CORALLOCBUFFERINIT_H_

#include "corAlloc/CorAllocStatus.h"    // CorAllocStatus
#include "corAlloc/CorAlloc.h"          // CorAlloc
#include "corAlloc/corAllocHooks.h"     // CorAllocErrorHook



// -----------------------------------------------------------------------------
//
// corAllocBufferInit -
//
extern void corAllocBufferInit
(
  CorAlloc*          kaP,        // pointer to corAlloc buffer
  char*              buf,        // the initial buffer to use for allocations
  unsigned long long bufSize,    // the size of the initial buffer
  unsigned long long allocSize,  // the size to use for subsequent allocations, when the init-buffer runs out
  CorAllocErrorHook  errorHook,  // function pointer to be called when errors occur
  const char*        name        // name of the buffer
);

#endif  // CORALLOC_CORALLOCBUFFERINIT_H_
