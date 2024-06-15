#pragma once

#include "global_constants.h"
#include "threeaddr_parser.h"
#include "instruction.h"
#include <stdio.h>
#include <stdlib.h>

struct BasicBlock *BasicBlock_create (enum BasicBlockType type);
struct Function *Function_create ();
struct Constant *Constant_create (struct Token constant_token);
struct Value *value_table_find (size_t index);
void parse_operand (struct Instruction *instruction, int operand_index);
void parse_binary_operator_operands (struct Instruction *instruction);
void display_function (struct Function *function);
struct Instruction *parse_instruction ();
struct Function *parse_function ();
struct Function *parse_ir (char *ir_source);