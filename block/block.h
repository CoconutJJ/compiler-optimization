#ifndef block_h
#define block_h
#include "definition.h"

typedef struct Block Block;

typedef struct Block {
        Definition *definitions;
        size_t count;
        Block *jump;
        Block *fallthrough;
} Block;

#endif