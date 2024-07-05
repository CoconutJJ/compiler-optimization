#pragma once
#include "value.h"

struct Constant {
        struct Value value;
        int constant;
};

void ConstantInit (struct Constant *constant, int constant_value);
struct Constant *ConstantCreate (int64_t constant);