#ifndef value_h
#define value_h
#include "definition.h"
#include <stdlib.h>
#include <stdlib.h>

typedef struct {
    uint32_t left;
    Operator op;
    uint32_t right;
} ValueRecord;

typedef struct {
    ValueRecord *map;
    size_t count;
    uint32_t value_no;
} ValueMap;


#endif