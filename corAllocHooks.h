//
// FILE            corAllocHooks.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORALLOC_CORALLOCHOOKS_H_
#define CORALLOC_CORALLOCHOOKS_H_

#include "corAlloc/CorAllocStatus.h"    // CorAllocStatus



// -----------------------------------------------------------------------------
//
// Forward declaration of CorAlloc
//
// NOTE
//   CorAlloc contains a CorAllocErrorHook, so CorAlloc.h cannot be included here.
//
struct CorAlloc;


// -----------------------------------------------------------------------------
//
// CORALLOC_ERROR_HOOK
//
#define CORALLOC_ERROR_HOOK(kaP, kas, errorString, vP)              \
do                                                            \
{                                                             \
  if (kaP->errorHook != NULL)                                 \
    kaP->errorHook(kaP, kas, errorString, vP);                \
} while (0)



// ----------------------------------------------------------------------------- 
//
// CorAllocErrorHook - function type for Error Hook callbacks
//
typedef void (*CorAllocErrorHook)(struct CorAlloc* kaP, CorAllocStatus kas, char* errorString, void* vP);

#endif  // CORALLOC_CORALLOCHOOKS_H_
