# corAlloc - Fast Memory Pool Allocator

A high-performance memory allocation library optimized for temporary buffers in request-response workflows like REST servers.

- **Version:** 0.1.0
- **Language:** C
- **License:** [Apache License 2.0](LICENSE)

The only dependency is **kbase**.

## Where it comes from

corAlloc is **kalloc** under the cor prefix, copied, not forked: kalloc itself is
untouched and keeps serving its own users.

The renames: `KAlloc` → `CorAlloc`, `kaAlloc` → `corAlloc`, and every other `ka*`
function → `corAlloc*` (`kaStrdup` → `corAllocStrdup`, `kaBufferInit` →
`corAllocBufferInit`, `kaBufferReset` → `corAllocBufferReset`, `kaRealloc` →
`corAllocRealloc`, ...); the types `KaStatus`/`KaErrorHook`/`KaAllocBuffer` →
`CorAllocStatus`/`CorAllocErrorHook`/`CorAllocBuffer`. `KBool` became `bool`.

kalloc had the function's header (`kaAlloc.h`) and the all-in-one header
(`kalloc.h`) apart; under the cor prefix both are `corAlloc.h`, so it is one
header that declares `corAlloc()` and includes the rest of the library.

## Features

- **Extremely fast allocation** - O(1) pointer arithmetic, no complex data structures
- **Bulk deallocation** - Reset entire buffer in one call instead of individual frees
- **Thread-safe** - POSIX semaphore protection for multi-threaded use
- **Buffer reuse** - Initial buffer can be stack-allocated and reused across cycles
- **Expandable** - Automatically allocates additional buffers when needed

## Design Philosophy

Instead of using slow `malloc()` calls, corAlloc maintains large pre-allocated buffers and portions them out sequentially. When the buffer runs out, another large buffer is allocated. There is no individual `free()` - all memory is released at once via `corAllocBufferReset()`.

This approach trades memory efficiency for dramatic speed improvements, making it ideal for:
- REST server request handling
- JSON parsing/rendering cycles
- Any workload with many short-lived allocations

## API Reference

### Types

```c
typedef struct CorAlloc {
    char*              initBuf;       // Initial buffer pointer
    unsigned long long initBufSize;   // Initial buffer size
    unsigned long long bytesLeft;     // Remaining bytes in current buffer
    unsigned long long allocSize;     // Size for subsequent allocations
    // ... internal fields
} CorAlloc;
```

### Functions

#### corAllocBufferInit

```c
void corAllocBufferInit(
    CorAlloc*          kaP,        // Pointer to CorAlloc structure
    char*              buf,        // Initial buffer (can be stack-allocated)
    unsigned long long bufSize,    // Size of initial buffer
    unsigned long long allocSize,  // Size for subsequent allocations
    CorAllocErrorHook  errorHook,  // Optional error callback (NULL if not needed)
    const char*        name        // Debug name for this allocator
);
```

Initializes the corAlloc allocator. Must be called before any allocations.

#### corAlloc

```c
char* corAlloc(CorAlloc* kaP, unsigned long long size);
```

Allocates `size` bytes from the buffer. Returns pointer to allocated memory, or NULL on failure.

**Fast path**: If current buffer has space, simply advances pointer (O(1)).
**Slow path**: If buffer is full, allocates new buffer via `calloc()`.

#### corAllocStrdup

```c
char* corAllocStrdup(CorAlloc* kaP, const char* s);
```

String duplication from corAlloc buffer (like `strdup()` but using corAlloc).

#### corAllocRealloc

```c
char* corAllocRealloc(
    CorAlloc*          kaP,
    char*              origBuf,
    unsigned long long origSize,
    unsigned long long newSize
);
```

Resizes an allocation. Allocates new buffer, copies data, returns new pointer. Note: Original buffer space is not reclaimed (handled by `corAllocBufferReset()`).

#### corAllocBufferReset

```c
void corAllocBufferReset(CorAlloc* kaP, bool reuse);
```

Deallocates all buffers and resets the allocator.

- `reuse = true`: Reset and reinitialize with original initial buffer
- `reuse = false`: Just free all dynamically allocated buffers

## Building

```bash
make          # Build library
make clean    # Remove build artifacts
make install  # Build (nothing to copy: consumers use -I.. and link from this checkout)
```

## Usage Example

```c
#include "corAlloc/corAlloc.h"

int main(void)
{
    CorAlloc corAlloc;
    char   initBuffer[4096];

    // Initialize with 4KB initial buffer, 8KB subsequent allocations
    corAllocBufferInit(&kalloc, initBuffer, sizeof(initBuffer), 8192, NULL, "main");

    // Fast allocations
    char* buf1 = corAlloc(&kalloc, 256);
    char* str  = corAllocStrdup(&kalloc, "hello world");
    char* buf2 = corAlloc(&kalloc, 1024);

    // Use buffers...

    // Reset for next cycle (reuse initial buffer)
    corAllocBufferReset(&kalloc, true);

    // Allocate again for next request...
    char* buf3 = corAlloc(&kalloc, 512);

    // Final cleanup
    corAllocBufferReset(&kalloc, false);
    return 0;
}
```

## Performance Characteristics

| Operation | Time Complexity | Notes |
|-----------|-----------------|-------|
| corAlloc (in-buffer) | O(1) | Simple pointer arithmetic |
| corAlloc (new buffer) | O(1)* | *Excludes calloc overhead |
| corAllocStrdup | O(n) | n = string length |
| corAllocRealloc | O(n) | n = data size to copy |
| corAllocBufferReset | O(m) | m = number of allocated buffers |

## Memory Layout

```
[Initial Buffer]
├─ [alloc 1] [alloc 2] [alloc 3] [unused...]
│
[Dynamic Buffer 1] (allocated when initial runs out)
├─ [metadata] [alloc 4] [alloc 5] [unused...]
│
[Dynamic Buffer 2]
└─ [metadata] [alloc 6] [unused...]
```

## Dependencies

- [kbase](../kbase) - the library-internal log macros (KLOG_*)

## License

[Apache 2.0](LICENSE) &copy; 2017-2025 Ken Zangelin
