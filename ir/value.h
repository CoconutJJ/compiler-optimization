#pragma once
#include "array.h"
#include "lexer.h"
#include <stdlib.h>
enum ValueType { VALUE_ARGUMENT, VALUE_INST, VALUE_CONST };

struct Value {
        size_t value_no;
        enum ValueType value_type;
        struct Array uses;
        struct Token token;
};

struct Use {
        struct Value *usee;
        struct Value *user;
        int operand_no;
};

void Value_init (struct Value *value);
void Value_set_token (struct Value *value, struct Token token);
void Use_init (struct Use *use);
void Use_link (struct Value *user, struct Value *usee, int usee_operand_no);
struct Value *Value_Use_iter (struct Value *value, size_t *iter_count);
size_t Value_Use_count (struct Value *value);