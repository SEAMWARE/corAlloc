//
// FILE            corAllocStrdup.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <string.h>                     // strlen, strcpy

#include "corAlloc/CorAlloc.h"          // CorAlloc
#include "corAlloc/corAlloc.h"          // corAlloc
#include "corAlloc/corAllocStrdup.h"    // Own interface



// -----------------------------------------------------------------------------
//
// corAllocStrdup -
//
char* corAllocStrdup(CorAlloc* kaP, const char* s)
{
  if (kaP == NULL || s == NULL)
    return NULL;

  int   size  = strlen(s) + 1;  // +1 for Zero-termination
  char* buf   = corAlloc(kaP, size);

  if (buf != NULL)
    strcpy(buf, s);

  return buf;
}
