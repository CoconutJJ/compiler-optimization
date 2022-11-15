#include "definition.h"
#include "variable.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

static uint32_t definition_no = 0;
static Definition *definitions = NULL;
static size_t definitions_size = 0;

Definition *
create_definition (Reference *dest, Operator op, Reference *fst, Reference *snd)
{
        if (definition_no == definitions_size) {
                if (definitions_size == 0)
                        definitions_size = 8;
                else
                        definitions_size *= 2;

                definitions = compiler_realloc (definitions,
                                       definitions_size * sizeof (Definition));
        }

        Definition *def = definitions + definition_no;

        *def = (Definition){ .def_no = definition_no++,
                             .dest = dest,
                             .fst = fst,
                             .snd = snd,
                             .op = op };

        return def;
}

Definition *get_definition_no (uint32_t n)
{
        if (n >= definition_no)
                return NULL;

        return definitions + n;
}