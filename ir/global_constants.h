#pragma once

#define DYNARR_INIT_SIZE_CNT  10
#define VALUE_TABLE_SIZE      1024
#define MAX_IDENTIFIER_LEN    80
#define MAX_FN_ARG_COUNT      10
#define MAX_BASIC_BLOCK_COUNT 1024

#define AS_INST(inst)           ((struct Instruction *)(inst))
#define AS_VALUE(val)           ((struct Value *)(val))
#define AS_CONST(constant)      ((struct Constant *)(constant))
#define VALUE_IS_INST(value)    ((value)->value_type == VALUE_INST)
#define VALUE_IS_ARG(value)     ((value)->value_type == VALUE_ARGUMENT)