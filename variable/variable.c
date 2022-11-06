#include "variable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
Variable *create_variable (char name[10])
{
        Variable *var = malloc(sizeof(Variable));

        if (!var) {
                perror("malloc");
                exit(EXIT_FAILURE);
        }

        var->index = -1;
        var->index_type = -1;
        strcpy(var->name, name);
        var->value_no = -1;

        return var;
}