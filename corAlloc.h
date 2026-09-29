//
// FILE            corAlloc.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORALLOC_CORALLOC_H_
#define CORALLOC_CORALLOC_H_

//
// corAlloc.h is two headers in one: the declaration of corAlloc() AND every
// header of the library. kalloc had them apart, as kaAlloc.h and kalloc.h, and
// under the cor prefix both are named corAlloc.h - so including the function's
// header includes the whole (small) library, which costs nothing.
//
#include "corAlloc/CorAlloc.h"
#include "corAlloc/CorAllocBuffer.h"
#include "corAlloc/CorAllocStatus.h"
#include "corAlloc/CorAllocTraceLevel.h"
#include "corAlloc/corAllocBufferInit.h"
#include "corAlloc/corAllocBufferReset.h"
#include "corAlloc/corAllocHooks.h"
#include "corAlloc/corAllocInit.h"
#include "corAlloc/corAllocStrdup.h"
#include "corAlloc/corAllocMem.h"



// -----------------------------------------------------------------------------
//
// corAlloc - allocate 'size' bytes, zeroed, from the allocator
//
extern char* corAlloc(CorAlloc* kaP, unsigned long long size);



// -----------------------------------------------------------------------------
//
// corAllocThreadSafe - make kaP safe to allocate from on several threads at once (see corAlloc.c)
//
extern void corAllocThreadSafe(CorAlloc* kaP);

#endif  // CORALLOC_CORALLOC_H_
