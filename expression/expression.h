#pragma once
#include "variable.h"

typedef enum { PLUS, MINUS, MULT, DIV } Operator;

typedef struct Expression {
        int id;
        Variable *fst;
        Operator op;
        Variable *snd;
} Expression;

 bool equal_expression (Expression *a, Expression *b);
