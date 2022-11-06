#ifndef variable_h
#define variable_h

#include <stdio.h>
#include <stdlib.h>

typedef struct Variable Variable;

typedef enum { VAR, LITERAL } Type;

typedef struct Variable {
        char name[10];
        Type index_type;
        union {
                uint32_t index;
                Variable *index_var;
        };
        uint32_t value_no;
} Variable;

#endif