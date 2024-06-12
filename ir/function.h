#pragma once
#include "global_constants.h"
#include "value.h"
struct Argument {
        struct Value value;
        struct Function *parent;
};

struct Function {
        struct BasicBlock *entry_basic_block;
        char fn_name[MAX_IDENTIFIER_LEN + 1];
        struct Argument arguments[MAX_FN_ARG_COUNT];
        size_t arguments_count;
};

void Function_init (struct Function *function);
struct Argument *Function_create_argument (struct Function *function);