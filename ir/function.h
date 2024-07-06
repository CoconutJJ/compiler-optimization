#pragma once
#include "array.h"
#include "global_constants.h"
#include "map.h"
#include "value.h"
struct Argument {
        struct Value value;
        struct Function *parent;
};

struct Function {
        struct BasicBlock *entry_basic_block;
        char fn_name[MAX_IDENTIFIER_LEN + 1];
        HashTable block_number_map;
        struct Array arguments;
};

void Function_init (struct Function *function);
void Function_add_argument (struct Function *function, struct Argument *argument);
void Argument_init (struct Argument *argument);
void FunctionComputeBlockNumberMapping (struct Function *function);