#include "variable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char **argv)
{
        Variable *var = create_variable ("a");

        Reference *ref = create_reference (var);

        if (ref->variable != var)
                return 1;

        if (ref->index_type != NIL)
                return 1;

        return 0;
}