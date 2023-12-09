#ifndef value_h
#define value_h
#include "definition.h"
#include "map.h"
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

#endif
