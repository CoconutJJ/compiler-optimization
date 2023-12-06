#include "expression.h"
#include "mem.h"
#include "variable.h"
#include <stdbool.h>

static uint32_t expression_id = 0;
static Expression *expressions = NULL;
static size_t expressions_size = 0;

Expression *create_expression (Variable *fst, Operator op, Variable *snd)
{
        expressions = DYNAMIC_ARRAY_RESIZE (expressions, Expression, expressions_size, expression_id);
        
        Expression *expr = expressions + expression_id;
        
        expr->fst = fst;
        expr->snd = snd;
        expr->op = op;
        expr->id = expression_id++;

        return expr;
}

Expression *get_expression_id (uint32_t id)
{
        if (id >= expression_id)
                return NULL;

        return expressions + id;
}

bool equal_expression (Expression *a, Expression *b)
{
        if (a->op != b->op)
                return false;

        bool is_equal = equal_variable (a->fst, b->fst) && equal_variable (a->snd, b->snd);

        switch (a->op) {
        case PLUS:
        case MULT: is_equal = is_equal && equal_variable (a->fst, b->snd) && equal_variable (a->snd, b->fst); break;
        case MINUS:
        case DIV: break;
        default: break;
        }

        return is_equal;
}
