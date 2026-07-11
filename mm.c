```c
#define _DEFAULT_SOURCE

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct block {
    size_t size;
    int free;
    struct block *next;
} block;

static block *head = NULL;
static pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
static size_t arena_size;

static size_t align16(size_t n)
{
    return (n + 15u) & ~15u;
}

static int init(void)
{
    if (head) {
        return 0;
    }

    arena_size = 1u << 20;

    void *p = mmap(
        NULL,
        arena_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (p == MAP_FAILED) {
        return -1;
    }

    head = p;
    head->size = arena_size - sizeof(block);
    head->free = 1;
    head->next = NULL;

    return 0;
}

void *mm_malloc(size_t n)
{
    if (!n) {
        return NULL;
    }

    pthread_mutex_lock(&mu);

    if (init() != 0) {
        pthread_mutex_unlock(&mu);
        return NULL;
    }

    n = align16(n);

    for (block *b = head; b; b = b->next) {
        if (b->free && b->size >= n) {
            if (b->size >= n + sizeof(block) + 16) {
                block *x = (void *)((char *)(b + 1) + n);

                x->size = b->size - n - sizeof(block);
                x->free = 1;
                x->next = b->next;

                b->next = x;
                b->size = n;
            }

            b->free = 0;

            pthread_mutex_unlock(&mu);

            return b + 1;
        }
    }

    pthread_mutex_unlock(&mu);

    return NULL;
}

static void coalesce(void)
{
    for (block *b = head; b && b->next;) {
        if (b->free && b->next->free) {
            b->size += sizeof(block) + b->next->size;
            b->next = b->next->next;
        } else {
            b = b->next;
        }
    }
}

void mm_free(void *p)
{
    if (!p) {
        return;
    }

    pthread_mutex_lock(&mu);

    block *b = (block *)p - 1;

    b->free = 1;

    coalesce();

    pthread_mutex_unlock(&mu);
}

void *mm_calloc(size_t count, size_t size)
{
    if (count && size > SIZE_MAX / count) {
        return NULL;
    }

    size_t total_size = count * size;
    void *p = mm_malloc(total_size);

    if (p) {
        memset(p, 0, total_size);
    }

    return p;
}

void *mm_realloc(void *p, size_t n)
{
    if (!p) {
        return mm_malloc(n);
    }

    if (!n) {
        mm_free(p);
        return NULL;
    }

    block *b = (block *)p - 1;

    if (b->size >= n) {
        return p;
    }

    void *q = mm_malloc(n);

    if (q) {
        memcpy(q, p, b->size);
        mm_free(p);
    }

    return q;
}

void mm_dump(void)
{
    pthread_mutex_lock(&mu);

    for (block *b = head; b; b = b->next) {
        printf(
            "block size=%zu free=%d\n",
            b->size,
            b->free
        );
    }

    pthread_mutex_unlock(&mu);
}
```
