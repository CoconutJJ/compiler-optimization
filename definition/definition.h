#ifndef definition_h
#define definition_h

#include "variable.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum { PLUS, MINUS, MULT, DIV } Operator;

typedef struct {
        uint32_t def_no;
        Variable *dest;
        Operator op;
        Variable *fst;
        Variable *snd;
} Definition;

Definition *
create_definition (Variable *dest, Operator op, Variable *fst, Variable *snd);

#endif