#include "integer_set.h"
#include <stdio.h>
#include <stdlib.h>

int main ()
{
        IntegerSet *set = set_create ();

        set_add (set, 1);

        if (!set_has (set, 1))
                return 1;

        set_add(set, 69);

        if (!set_has(set, 69))
                return 1;

        set_destroy(set);

        return 0;
}