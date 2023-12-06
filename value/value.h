#ifndef value_h
#define value_h
#include "definition.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct {

        uint32_t fst;
        uint32_t snd;
        uint32_t op;
        
} ValueNoExpr;
typedef struct {
        // variable id to value no map
        uint32_t *var_to_value_no;
        size_t var_to_value_size;
        
        // value no to variable map
        Variable **value_no_to_var;
        size_t value_to_var_size;

        // value number of expressions
        ValueNoExpr *map;
        uint32_t value_no;
        size_t map_size;
} ValueMap;


#endif
