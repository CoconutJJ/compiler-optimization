#include "constant.h"

void Constant_init (struct Constant *constant, int constant_value)
{
        Value_init (&constant->value);
        constant->value.value_type = VALUE_CONST;
        constant->constant = constant_value;
}
