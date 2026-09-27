//
// FILE            corAllocBufferReset.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdlib.h>                     // free
#include <string.h>                     // memset
#include <stdbool.h>                    // bool

#include "kbase/kLibLog.h"              // KLOG_*

#include "corAlloc/CorAlloc.h"          // CorAlloc
#include "corAlloc/CorAllocBuffer.h"    // CorAllocBuffer
#include "corAlloc/corAllocBufferInit.h"        // corAllocBufferInit
#include "corAlloc/corAllocBufferReset.h"       // Own Interface



// -----------------------------------------------------------------------------
//
// corAllocBufferReset
//
void corAllocBufferReset(CorAlloc* kaP, bool reuse)
{
  if (kaP == NULL)
    return;

  CorAllocBuffer* kabP = kaP->allocList;

  while (kabP != NULL)
  {
    char* toFree = kabP->toFree;

    kabP = kabP->next;

    if (toFree != NULL)
      free(toFree);
  }

  kaP->allocListTail = NULL;

  if (reuse == true)
  {
    memset(kaP->initBuf, 0, kaP->initBufSize);
    corAllocBufferInit(kaP, kaP->initBuf, kaP->initBufSize, kaP->allocSize, kaP->errorHook, kaP->name);
  }
}
