#include "integer_set.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
int main ()
{
        IntegerSet *set = set_create ();

        for (int i = 0; i < 120; i++) {
                set_add (set, (uint64_t)i);
        }

        int64_t N = -1;
        for (int j = 0; j < 120; j++) {
                if (!set_iter (set, &N))
                        return 1;

                if (N != j)
                        return 1;
        }

        set_destroy (set);

        return 0;
}