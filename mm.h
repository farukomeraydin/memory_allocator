#ifndef MM_H
#define MM_H

#include <stddef.h>

void*mm_malloc(size_t n);
void*mm_calloc(size_t c,size_t n);
void*mm_realloc(void*p,size_t n);
void mm_free(void*p);
void mm_dump(void);

#endif
