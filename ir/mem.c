#include "array.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void **allocations = NULL;
static size_t allocations_size = 0;
static size_t allocations_count = 0;

void _ir_mem_autoresize ()
{
        if (allocations_count == allocations_size) {
                allocations_size *= 2;
                allocations = realloc (allocations, allocations_size * sizeof (void *));

        } else if (allocations_count < allocations_size / 4) {
                allocations_size /= 2;
                allocations = realloc (allocations, allocations_size * sizeof (void *));
        }
}

void _ir_add_allocation (void *mem)
{
        if (!allocations) {
                allocations = calloc (DYNARR_INIT_SIZE_CNT, sizeof (void *));
                allocations_size = DYNARR_INIT_SIZE_CNT;
                if (!allocations) {
                        perror ("malloc");
                        exit (EXIT_FAILURE);
                }
        } else {
                _ir_mem_autoresize ();
        }

        allocations[allocations_count++] = mem;
}

void *ir_realloc (void *mem, size_t size)
{
        mem = realloc (mem, size);

        if (!mem) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        return mem;
}

void *ir_malloc (size_t size)
{
        void *mem = malloc (size);

        if (!mem) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        _ir_add_allocation (mem);

        return mem;
}

void *ir_calloc (size_t nmemb, size_t size)
{
        void *mem = calloc (nmemb, size);

        if (!mem) {
                perror ("calloc");
                exit (EXIT_FAILURE);
        }

        _ir_add_allocation (mem);

        return mem;
}

void ir_free (void *mem)
{
        size_t i = 0;
        for (; i < allocations_count; i++) {
                if (allocations[i] == mem)
                        break;
        }

        assert (i != allocations_count);

        free (mem);

        memmove (&allocations[i], &allocations[i + 1], (allocations_count - (i + 1)) * sizeof (void *));
        allocations_count--;
        _ir_mem_autoresize ();
}
