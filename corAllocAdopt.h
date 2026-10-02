//
// FILE            corAllocAdopt.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORALLOC_CORALLOCADOPT_H_
#define CORALLOC_CORALLOCADOPT_H_

#include "corAlloc/CorAlloc.h"          // CorAlloc



// -----------------------------------------------------------------------------
//
// corAllocAdopt - dstP takes over srcP's memory: freed when dstP is reset, and no longer srcP's
//
// For memory filled on one thread and kept by another - a response decoded, by whichever thread read
// it, into an arena of its own, then handed to the request that waits for it, whose allocator frees
// it with everything else of the request. Nothing is copied: the blocks change lists.
//
// srcP must have been initialised with NO initial buffer (corAllocBufferInit(srcP, NULL, 0, ...)):
// only malloc'd blocks can change owner. srcP is empty afterwards, and may be used again.
//
extern void corAllocAdopt(CorAlloc* dstP, CorAlloc* srcP);

#endif  // CORALLOC_CORALLOCADOPT_H_
