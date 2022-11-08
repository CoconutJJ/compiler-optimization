#ifndef variable_h
#define variable_h

#include <stdio.h>
#include <stdlib.h>

typedef struct Variable Variable;

typedef enum { NIL, VAR, LITERAL } Type;

typedef struct Variable {
        uint32_t id;
        char name[10];
        uint32_t value_no;
} Variable;

typedef struct Reference {
        Variable *variable;
        Type index_type;
        union {
                uint32_t index;
                Variable *index_var;
        };
} Reference;


Variable *create_variable (char name[10]);
Variable *get_variable_id (uint32_t id);
Reference *create_reference (Variable *variable);

#endif