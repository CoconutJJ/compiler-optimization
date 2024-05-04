#include "variable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char **argv)
{
        Variable *var = create_variable ("a");

        if (strcmp (var->name, "a") != 0)
                return 1;

        if (get_variable_id (var->id) != var)
                return 1;

        return 0;
}