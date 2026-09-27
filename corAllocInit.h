//
// FILE            corAllocInit.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORALLOC_CORALLOCINIT_H_
#define CORALLOC_CORALLOCINIT_H_


#include "corAlloc/CorAllocStatus.h"    // CorAllocStatus
#include "corAlloc/corAllocLog.h"       // CorAllocLogFunction



// -----------------------------------------------------------------------------
//
// corAllocInit - initialize the corAlloc library
//
extern CorAllocStatus corAllocInit(CorAllocLogFunction logFunction);

#endif  // CORALLOC_CORALLOCINIT_H_
