```c
#include "mm.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    char *a = mm_malloc(32);

    strcpy(a, "allocator works");
    puts(a);

    int *b = mm_calloc(100, sizeof(int));

    b = mm_realloc(
        b,
        200 * sizeof(int)
    );

    mm_free(a);
    mm_free(b);

    mm_dump();

    return 0;
}
```
