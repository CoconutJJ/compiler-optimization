#include "value.h"
#include "expression.h"
#include "map.h"
#include "mem.h"
#include "variable.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void init_ValueMap (ValueMap *map)
{
        init_map (&map->expr_to_value_no, 10);
        init_map (&map->value_no_to_var, 10);
        init_map (&map->var_to_value_no, 10);
        map->value_no = 1;
        return;
}

uint32_t hash_expr (ValueNoExpr expr)
{
        uint32_t fst = expr.fst, snd = expr.snd;
        switch (expr.op) {
        case PLUS:
        case MULT:
                // if operation is commutative, sort based on ascending value number
                if (fst > snd) {
                        uint32_t tmp = fst;
                        fst = snd;
                        snd = tmp;
                }
                break;
        default: break;
        }

        return map_hash3 (expr.fst, expr.snd, (uint32_t)expr.op);
}

uint32_t variable_to_value_no (ValueMap *map, Variable *v)
{
        void *value_ptr = map_find (&map->var_to_value_no, v->id);

        uint32_t value;

        if (!value_ptr) {
                value = map->value_no++;

                map_insert (&map->var_to_value_no, v->id, (void *)value);
                map_insert (&map->value_no_to_var, value, v);
        } else {
                value = (uint32_t)value_ptr;
        }

        return value;
}

ValueNoExpr _expr_to_value_no_expr (ValueMap *map, Expression *expr)
{
        return (ValueNoExpr){ .fst = variable_to_value_no (map, expr->fst),
                              .snd = variable_to_value_no (map, expr->snd),
                              .op = expr->op };
}
uint32_t expr_to_value_no (ValueMap *map, Expression *expr)
{
        ValueNoExpr value_expr = _expr_to_value_no_expr (map, expr);

        uint32_t hash = hash_expr (value_expr);

        uint32_t value = (uint32_t)map_find (&map->expr_to_value_no, hash);

        if (!value) {
                value = map->value_no++;
                map_insert (&map->expr_to_value_no, hash, (void *)value);
        }

        return value;
}

Variable *value_no_to_variable (ValueMap *map, uint32_t value)
{
        Variable *v = (uint32_t)map_find (&map->value_no_to_var, value);

        return v;
}
