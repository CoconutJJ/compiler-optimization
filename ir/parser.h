#pragma once

#include "basicblock.h"
#include "function.h"
#include "global_constants.h"
#include "instruction.h"
#include "lexer.h"
#include "map.h"
#include <stdio.h>
#include <stdlib.h>

struct BackPatch {
    struct Instruction *instruction;
    int operand_no;
    size_t variable_no;
};


struct Parser {
    HashTable value_table;
    HashTable label_table;
    struct Array back_patches;
};



void PrintFunction (struct Function *function);
struct Function *ParseIR (char *ir_source);
