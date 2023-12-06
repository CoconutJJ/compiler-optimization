#include "value.h"
#include "expression.h"
#include "mem.h"
#include "variable.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void init_ValueMap (ValueMap *map)
{
        map->value_no = 1;
        map->map_size = 0;
        map->value_to_var_size = 0;
        map->var_to_value_size = 0;

        map->value_no_to_var = NULL;
        map->var_to_value_no = NULL;
        map->map = NULL;

        return;
}

uint32_t allocate_value_no (ValueMap *map)
{
        return map->value_no++;
}

void label_value (ValueMap *map, Variable *v, uint32_t value_no)
{
        // the var_to_value_no mapping is not contiguous since it uses the variable id as the key value
        DYNAMIC_ARRAY_RESIZE_ZERO (map->var_to_value_no, uint32_t, map->var_to_value_size, v->id);
        map->var_to_value_no[v->id] = value_no;

        DYNAMIC_ARRAY_RESIZE_ZERO (map->value_no_to_var, Variable *, map->value_to_var_size, value_no);
        map->value_no_to_var[value_no] = v;
}

uint32_t variable_to_value_no (ValueMap *map, Variable *v)
{
        if (v->id >= map->var_to_value_size || map->var_to_value_no[v->id] == 0) {
                uint32_t value_no = allocate_value_no (map);
                label_value (map, v, value_no);
        }

        return map->var_to_value_no[v->id];
}

ValueNoExpr _expr_to_value_no_expr (ValueMap *map, Expression *expr)
{
        return (ValueNoExpr){ .fst = variable_to_value_no (map, expr->fst),
                              .snd = variable_to_value_no (map, expr->snd),
                              .op = expr->op };
}

uint32_t expr_to_value_no (ValueMap *map, Expression *expr)
{
        uint32_t value = allocate_value_no (map);

        ValueNoExpr value_no_expr = _expr_to_value_no_expr (map, expr);

        // TODO: need to find the equivalent expression in the map

        DYNAMIC_ARRAY_RESIZE_ZERO (map->map, ValueNoExpr, map->map_size, value);

        map->map[value] = value_no_expr;

        return value;
}

Variable *find_value (ValueMap *map, uint32_t value_no)
{
        if (value_no >= map->value_to_var_size) {
                return NULL;
        }

        return map->value_no_to_var[value_no];
}
