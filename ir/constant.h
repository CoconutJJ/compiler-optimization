#pragma once
#include "value.h"

struct Constant {
        struct Value value;
        int constant;
};

void Constant_init (struct Constant *constant, int constant_value);
