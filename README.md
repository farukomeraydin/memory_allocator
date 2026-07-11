# Diagnostic Memory Allocator

A thread-safe fixed-arena allocator implemented with `mmap`. It supports aligned allocation, block splitting, coalescing, `calloc`, `realloc`, and an allocator-state dump.

## Build and test
```bash
make
./test_allocator
```

Sanitizer build:
```bash
make clean
make sanitize
./test_allocator
```

Use scenario: allocate multiple differently sized objects, free them in a different order, and inspect whether adjacent free blocks are coalesced in `mm_dump` output.
