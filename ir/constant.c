#include "constant.h"
#include "map.h"
#include "mem.h"
#include "value.h"
#include <stdint.h>

void ConstantInit (struct Constant *constant, int constant_value)
{
        Value_init (&constant->value);
        constant->value.value_type = VALUE_CONST;
        constant->constant = constant_value;
}

struct Constant *ConstantCreate (int64_t constant)
{
        struct Constant *const_inst = ir_malloc (sizeof (struct Constant));

        Value_init (&const_inst->value);
        const_inst->value.value_type = VALUE_CONST;
        const_inst->constant = constant;

        return const_inst;
}