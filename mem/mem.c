#include <stdio.h>
#include <stdlib.h>

void *compiler_malloc (size_t sz)
{
        void *m = malloc (sz);

        if (!m) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        return m;
}

void *compiler_realloc (void *ptr, size_t sz)
{
        void *m = realloc (ptr, sz);

        if (!m) {
                perror ("realloc");
                exit (EXIT_FAILURE);
        }
        return m;
}

void *dynamic_array_resize (void *array_ptr,
                            size_t size,
                            size_t *nitems,
                            size_t count)
{
        size_t total_items = *nitems;

        if (count >= total_items) {
                total_items = (total_items == 0) ? 8 : total_items * 2;
                array_ptr = compiler_realloc (array_ptr, total_items * size);
                *nitems = total_items;
        }

        return array_ptr;
}