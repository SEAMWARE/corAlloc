#ifndef CORALLOC_CORALLOCLOG_H_
#define CORALLOC_CORALLOCLOG_H_

// 
// FILE            corAllocLog.h
// 
// AUTHOR          Ken Zangelin
// 
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//



// -----------------------------------------------------------------------------
//
// CorAllocLogFunction - 
//
typedef void (*CorAllocLogFunction)
(
  int          severity,              // 1: Error, 2: Warning, 3: Info, 4: Msg, 5: Verbose, 6: Trace
  int          level,                 // Trace level || Error code || Info Code
  const char*  fileName,
  int          lineNo,
  const char*  functionName,
  const char*  format,
  ...
);



#ifdef CORALLOC_LOG_ON
// -----------------------------------------------------------------------------
//
// CORALLOC_E
//
#define CORALLOC_E(...) corAllocLogFunction(1, 0, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)



// -----------------------------------------------------------------------------
//
// CORALLOC_RE
//
#define CORALLOC_RE(retVal, ...) do { corAllocLogFunction(1, 0, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); return retVal; } while (0)



// -----------------------------------------------------------------------------
//
// CORALLOC_W
//
#define CORALLOC_W(...) corAllocLogFunction(2, 0, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)



// -----------------------------------------------------------------------------
//
// CORALLOC_I
//
#define CORALLOC_I(...) corAllocLogFunction(3, 0, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)



// -----------------------------------------------------------------------------
//
// CORALLOC_M
//
#define CORALLOC_M(...) corAllocLogFunction(4, 0, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)



// -----------------------------------------------------------------------------
//
// CORALLOC_V
//
#define CORALLOC_V(...) corAllocLogFunction(5, 0, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)



// -----------------------------------------------------------------------------
//
// CORALLOC_T
//
#define CORALLOC_T(tLevel, ...) corAllocLogFunction(6, tLevel, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)



#else

#define CORALLOC_E(...)
#define CORALLOC_RE(retVal, ...) return retVal
#define CORALLOC_W(...)
#define CORALLOC_I(...)
#define CORALLOC_M(...)
#define CORALLOC_V(...)
#define CORALLOC_T(tLevel, ...)
#endif



// -----------------------------------------------------------------------------
//
// corAllocLogFunction - 
//
extern CorAllocLogFunction corAllocLogFunction;

#endif  // CORALLOC_CORALLOCLOG_H_
