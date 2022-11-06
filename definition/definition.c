#include "definition.h"
#include "variable.h"
#include <stdio.h>
#include <stdlib.h>

static uint32_t definition_no = 0;

Definition *
create_definition (Variable *dest, Operator op, Variable *fst, Variable *snd)
{
        Definition *def = malloc (sizeof (Definition));

        if (!def) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        def->def_no = definition_no++;
        def->dest = dest;
        def->fst = fst;
        def->snd = snd;
        def->op = op;

        return def;
}