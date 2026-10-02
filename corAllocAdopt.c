//
// FILE            corAllocAdopt.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stddef.h>                     // NULL
#include <pthread.h>                    // pthread_mutex_lock

#include "corAlloc/CorAlloc.h"          // CorAlloc
#include "corAlloc/CorAllocBuffer.h"    // CorAllocBuffer
#include "corAlloc/corAllocAdopt.h"     // Own interface



// -----------------------------------------------------------------------------
//
// corAllocAdopt -
//
// Every block's list entry lives at the start of the block itself (corAlloc.c), so the blocks change
// owner by splicing srcP's list onto the tail of dstP's.
//
void corAllocAdopt(CorAlloc* dstP, CorAlloc* srcP)
{
  if ((dstP == NULL) || (srcP == NULL) || (srcP->allocList == NULL))
    return;

  if (dstP->threadSafe == true)
    pthread_mutex_lock(&dstP->mutex);

  if (dstP->allocList == NULL)
    dstP->allocList = srcP->allocList;
  else
    dstP->allocListTail->next = srcP->allocList;

  dstP->allocListTail = srcP->allocListTail;

  if (dstP->threadSafe == true)
    pthread_mutex_unlock(&dstP->mutex);

  srcP->allocList     = NULL;
  srcP->allocListTail = NULL;
  srcP->allocPointer  = NULL;
  srcP->bytesLeft     = 0;
  srcP->allocations   = 0;
}
