//
// FILE            corAllocRealloc.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORALLOC_CORALLOCREALLOC_H_
#define CORALLOC_CORALLOCREALLOC_H_

#include "corAlloc/CorAlloc.h"   // CorAlloc struct



// -----------------------------------------------------------------------------
//
// corAllocRealloc -
//
extern char* corAllocRealloc(CorAlloc* kaP, char* origBuf, unsigned long long origSize, unsigned long long newSize);

#endif   // CORALLOC_CORALLOCREALLOC_H_
