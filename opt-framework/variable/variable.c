#include "variable.h"
#include "mem.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t variable_id = 0;
static Variable *variables = NULL;
static size_t variables_size = 0;

Variable *create_variable (char name[10])
{
        variables = DYNAMIC_ARRAY_RESIZE (variables, Variable, variables_size, variable_id);

        Variable *var = variables + variable_id;

        var->id = variable_id++;
        var->t = VAR;
        strcpy (var->name, name);

        return var;
}

Variable *create_constant (uint32_t value)
{
        variables = DYNAMIC_ARRAY_RESIZE (variables, Variable, variables_size, variable_id);

        Variable *var = variables + variable_id;

        var->id = variable_id++;
        var->t = LITERAL;
        var->value = value;

        return var;
}

Variable *get_variable_id (uint32_t id)
{
        if (id >= variable_id)
                return NULL;

        return variables + id;
}

bool equal_variable (Variable *a, Variable *b)
{
        return a->id == b->id;
}
