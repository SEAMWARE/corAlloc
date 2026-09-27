#ifndef CORALLOC_CORALLOCSTRUCT_H_
#define CORALLOC_CORALLOCSTRUCT_H_

// 
// FILE            CorAlloc.h
// 
// AUTHOR          Ken Zangelin
// 
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <semaphore.h>                  // sem_t

#include "corAlloc/corAllocHooks.h"     // CorAllocErrorHook
#include "corAlloc/CorAllocBuffer.h"    // CorAllocBuffer



// ----------------------------------------------------------------------------- 
//
// CorAlloc -
//
typedef struct CorAlloc
{
  sem_t           sem;            // semaphore to protect the allocation buffer
  char*           initBuf;        // pointer to the initial buffer
  unsigned long long initBufSize; // total size of initial buffer
  int             allocations;    // number of additional allocations done
  char*           allocPointer;   // pointer to next free byte
  unsigned long long bytesLeft;   // remaining bytes in current alloc buffer
  CorAllocErrorHook     errorHook;      // points to a function that is to be called on errors

  unsigned long long allocSize;   // size of new allocations
  CorAllocBuffer* allocList;      // linked list of allocated buffers - to be freed
  CorAllocBuffer* allocListTail;  // tail pointer for O(1) append

  char*           name;           // For debugging purposes only
} CorAlloc;

#endif  //  CORALLOC_CORALLOCSTRUCT_H_
