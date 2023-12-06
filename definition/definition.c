#include "definition.h"
#include "mem.h"
#include "variable.h"
#include <stdio.h>
#include <stdlib.h>

static uint32_t definition_no = 0;
static Definition *definitions = NULL;
static size_t definitions_size = 0;

Definition *create_definition (Reference *dest, Expression *expr)
{
        DYNAMIC_ARRAY_RESIZE (definitions, Definition, definitions_size, definition_no);

        Definition *def = definitions + definition_no;

        *def = (Definition){ .def_no = definition_no++, .dest = dest, .expr = expr };

        return def;
}

Definition *get_definition_no (uint32_t n)
{
        if (n >= definition_no)
                return NULL;

        return definitions + n;
}
