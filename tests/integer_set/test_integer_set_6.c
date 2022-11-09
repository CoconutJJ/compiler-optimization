#include "integer_set.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main ()
{
        IntegerSet *setA = set_create ();
        IntegerSet *setB = universal_set ();

        set_add (setA, 1);
        set_add (setA, 7);

        if (!is_universal_set (set_union (setA, setB)))
                return 1;

        set_destroy (setA);
        set_destroy (setB);


        // try switching order of arguments, shouldn't matter.
        setA = set_create ();
        setB = universal_set ();

        if (!is_universal_set (set_union (setB, setA)))
                return 1;

        set_destroy(setA);
        set_destroy(setB);

        return 0;
}