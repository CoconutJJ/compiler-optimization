#ifndef definition_h
#define definition_h

#include "expression.h"
#include "variable.h"
#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
        uint32_t def_no;
        Variable *dest;
        Expression *expr;
        struct list_head *cfg_head;
        
} Definition;

Definition *create_definition (Variable *dest, Expression *expr);

Definition *get_definition_no (uint32_t n);
#endif
