#ifndef definition_h
#define definition_h

#include "expression.h"
#include "variable.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
        uint32_t def_no;
        Reference *dest;
        Expression *expr;
} Definition;

Definition *create_definition (Reference *dest, Expression *expr);

Definition *get_definition_no (uint32_t n);
#endif
