#ifndef mem_h
#define mem_h
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define DYNAMIC_ARRAY_RESIZE(array, elemtype, size, count)                                                             \
        dynamic_array_resize (array, sizeof (elemtype), &size, count, 0)

#define DYNAMIC_ARRAY_RESIZE_ZERO(array, elemtype, size, count)                                                        \
        dynamic_array_resize (array, sizeof (elemtype), &size, count, 1)

void *compiler_malloc (size_t sz);
void *compiler_realloc (void *ptr, size_t sz);
void *dynamic_array_resize (void *array_ptr, size_t size, size_t *nitems, size_t count, bool zero);

#endif
