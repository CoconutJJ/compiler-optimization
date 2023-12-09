#ifndef variable_h
#define variable_h

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct Variable Variable;

typedef enum { NIL, VAR, LITERAL } Type;

typedef struct Variable {
        uint32_t id;
        Type t;
        union {
                uint32_t value;
                char name[10];
        };

} Variable;

Variable *create_variable (char name[10]);
Variable *get_variable_id (uint32_t id);
bool equal_variable (Variable *a, Variable *b);
#endif
