#ifndef block_h
#define block_h
#include "definition.h"
#include "integer_set.h"
typedef struct Block Block;

typedef struct Block {
        uint32_t id;
        Definition *definitions;
        size_t count;
        Block *jump;
        Block *fallthrough;
        IntegerSet predecessors;
} Block;

#endif