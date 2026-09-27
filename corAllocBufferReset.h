//
// FILE            corAllocBufferReset.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORALLOC_CORALLOCBUFFERRESET_H_
#define CORALLOC_CORALLOCBUFFERRESET_H_


#include <stdbool.h>                    // bool

#include "corAlloc/CorAlloc.h"          // CorAlloc



// -----------------------------------------------------------------------------
//
// corAllocBufferReset
//
extern void corAllocBufferReset(CorAlloc* kaP, bool reuse);

#endif // CORALLOC_CORALLOCBUFFERRESET_H_
