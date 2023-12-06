#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void *dynamic_array_resize (void *array_ptr, size_t size, size_t *nitems, size_t count, bool zero)
{
        size_t total_items = *nitems;

        if (count >= total_items) {
                size_t end_index = total_items;

                if (total_items == 0) {
                        total_items++;
                }

                while (total_items <= count) {
                        total_items *= 2;
                }
                size_t added_space = total_items - end_index;

                array_ptr = compiler_realloc (array_ptr, total_items * size);
                *nitems = total_items;

                if (zero) {
                        memset (array_ptr + end_index, 0, added_space * size);
                }
        }

        return array_ptr;
}
