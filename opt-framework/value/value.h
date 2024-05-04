#ifndef value_h
#define value_h
#include "definition.h"
#include "map.h"
#include "variable.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct {
        uint32_t fst;
        uint32_t snd;
        uint32_t op;

} ValueNoExpr;
typedef struct {
        IntegerMap expr_to_value_no;
        IntegerMap value_no_to_var;
        IntegerMap var_to_value_no;
        uint32_t value_no;
} ValueMap;

void init_ValueMap (ValueMap *map);
uint32_t variable_to_value_no (ValueMap *map, Variable *v);
uint32_t expr_to_value_no (ValueMap *map, Expression *expr);
Variable *value_no_to_variable (ValueMap *map, uint32_t value);
#endif
