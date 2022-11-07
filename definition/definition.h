#ifndef definition_h
#define definition_h

#include "variable.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum { PLUS, MINUS, MULT, DIV } Operator;

typedef struct {
        uint32_t def_no;
        Reference *dest;
        Operator op;
        Reference *fst;
        Reference *snd;
} Definition;

Definition *create_definition (Reference *dest,
                               Operator op,
                               Reference *fst,
                               Reference *snd);

Definition *get_definition_no (uint32_t n);
#endif