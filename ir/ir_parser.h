#include "threeaddr.h"
#include "threeaddr_parser.h"
#include <stdio.h>
#include <stdlib.h>
#define VALUE_TABLE_SIZE 1024
#define LABEL_TABLE_SIZE 1024

struct BasicBlock *BasicBlock_create ();
struct Function *Function_create ();
struct Constant *Constant_create (int constant_value);
void Value_table_init ();
void Value_table_clear ();
void value_table_insert (struct Value *value, size_t index);
struct Value *value_table_find (size_t index);
void parse_operand (struct Instruction *instruction, int operand_index);
void parse_binary_operator_operands (struct Instruction *instruction);
void display_function (struct Function *function);
struct Instruction * parse_instruction (struct BasicBlock *basic_block);
struct Function *parse_function ();
struct Function *parse_ir (char *ir_source);