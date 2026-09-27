//
// FILE            CorAllocBuffer.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORALLOC_CORALLOCBUFFER_H_
#define CORALLOC_CORALLOCBUFFER_H_



// ----------------------------------------------------------------------------- 
//
// CorAllocBuffer - 
//
typedef struct CorAllocBuffer
{
  char*                  toFree;
  struct CorAllocBuffer* next;
} CorAllocBuffer;

#endif  // CORALLOC_CORALLOCBUFFER_H_
