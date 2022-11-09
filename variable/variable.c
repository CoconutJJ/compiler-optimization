#include "variable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t variable_id = 0;
static Variable *variables = NULL;
static size_t variables_size = 0;

Variable *create_variable (char name[10])
{
        if (variable_id == variables_size) {
                
                if (variables_size == 0)
                        variables_size = 8;
                else
                        variables_size *= 2;

                variables =
                        realloc (variables, variables_size * sizeof (Variable));

                if (!variables) {
                        perror ("realloc");
                        exit (EXIT_FAILURE);
                }
        }

        Variable *var = variables + variable_id;

        var->id = variable_id++;
        strcpy (var->name, name);
        var->value_no = -1;

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
        Reference *ref = malloc (sizeof (Reference));

        if (!ref) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        ref->variable = variable;
        ref->index_type = NIL;

        return ref;
}