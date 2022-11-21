#include "variable.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t variable_id = 0;
static Variable *variables = NULL;
static size_t variables_size = 0;

Variable *create_variable (char name[10])
{
        variables = DYNAMIC_ARRAY_RESIZE (
                variables, Variable, variables_size, variable_id);

        Variable *var = variables + variable_id;

        var->id = variable_id++;
        strcpy (var->name, name);

        return var;
}
Variable *get_variable_id (uint32_t id)
{
        if (id >= variable_id)
                return NULL;

        return variables + id;
}

Reference *create_reference (Variable *variable)
{
        Reference *ref = compiler_malloc (sizeof (Reference));

        if (!ref) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        ref->variable = variable;
        ref->index_type = NIL;
        ref->value_no = -1;
        return ref;
}