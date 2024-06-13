#include "array.h"
#include <stdio.h>
#include <stdlib.h>


void *ir_malloc (size_t size)
{
        void *mem = malloc (size);

        if (!mem) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        

        return mem;
}

void *ir_calloc (size_t nmemb, size_t size)
{
        void *mem = calloc (nmemb, size);

        if (!mem) {
                perror ("calloc");
                exit (EXIT_FAILURE);
        }

        return mem;
}

