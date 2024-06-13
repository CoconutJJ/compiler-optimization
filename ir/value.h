#pragma once
#include "threeaddr_parser.h"
#include <stdlib.h>
enum ValueType { VALUE_ARGUMENT, VALUE_INST, VALUE_CONST };

struct Value {
        enum ValueType value_type;
        size_t value_no;

        struct Use *uses;
        size_t uses_count;
        size_t uses_size;

        struct Token token;
};

struct Use {
        struct Value *usee;
        struct Value *user;
        int operand_no;
};

void Value_init (struct Value *value);
void Value_set_token(struct Value *value, struct Token token);
void Use_init (struct Use *use);
struct Use *Value_create_use (struct Value *value);