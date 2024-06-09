#pragma once
#include "array.h"
#include "threeaddr_parser.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "constants.h"

enum ValueType { VALUE_ARGUMENT, VALUE_INST, VALUE_CONST };
enum InstType { INST_UNARY, INST_BINARY, INST_NIL, INST_BRANCH };
enum OpCode { OPCODE_ADD, OPCODE_SUB, OPCODE_MUL, OPCODE_DIV, OPCODE_JMP, OPCODE_JMPIF, OPCODE_NIL };

struct Value {
        enum ValueType value_type;
        size_t value_no;

        struct Use *uses;
        size_t uses_count;
        size_t uses_size;
};

struct Use {
        struct Value *usee;
        struct Value *user;
        int operand_no;
};

struct BasicBlock {
        size_t block_no;

        struct Array values;

        // struct Value *values;
        // size_t values_count;
        // size_t values_size;

        struct Function *parent;

        struct Array preds;
        struct BasicBlock *left;
        struct BasicBlock *right;
};

struct Argument {
        struct Value value;
        struct Function *parent;
};

struct Function {
        struct BasicBlock *entry_basic_block;
        char fn_name[MAX_IDENTIFIER_LEN + 1];
        struct Argument arguments[MAX_FN_ARG_COUNT];
        size_t arguments_count;
};

struct Instruction {
        struct Value value;
        struct BasicBlock *parent;
        enum InstType inst_type;
        enum OpCode op_code;

        struct {
                struct Value *first;
                struct Value *second;
        } operands;
};

struct Constant {
        struct Value value;
        int constant;
};

#define AS_INST(inst)           ((struct Instruction *)(inst))
#define AS_VALUE(val)           ((struct Value *)(val))
#define AS_CONST(constant)      ((struct Constant *)(constant))
#define VALUE_IS_INST(value)    ((value)->value_type == VALUE_INST)
#define INST_IS_BINARY_OP(inst) ((inst)->inst_type == INST_BINARY)
#define INST_IS_BRANCH(inst)    ((inst)->inst_type == INST_BRANCH)

void dynarr_init (void **buffer, size_t *count, size_t *size, size_t item_size);
void *dynarr_insert (void **buffer, size_t *count, size_t *size, void *item, size_t item_size, size_t insert_index);
void *dynarr_push (void **buffer, size_t *count, size_t *size, void *item, size_t item_size);
void dynarr_pop (void **buffer, size_t *count, size_t *size, void *ret, size_t item_size);
void dynarr_delete (void **buffer, size_t *count, size_t *size, size_t item_size, size_t delete_index);

void Value_init (struct Value *value);
void Use_init (struct Use *use);
void Instruction_init (struct Instruction *instruction);
void BasicBlock_init (struct BasicBlock *basic_block);
void BasicBlock_set_left_child (struct BasicBlock *basic_block, struct BasicBlock *left_child);
void BasicBlock_set_right_child (struct BasicBlock *basic_block, struct BasicBlock *right_child);
void Instruction_set_operand (struct Instruction *instruction, struct Value *operand, int operand_index);
void Function_init (struct Function *function);
bool Instruction_contains (struct Instruction *instruction, struct Value *value);
struct Instruction *Instruction_create (enum OpCode op);
void BasicBlock_add_Instruction(struct BasicBlock *basic_block, struct Instruction *instruction);
size_t BasicBlock_get_Instruction_count (struct BasicBlock *basic_block);
struct Instruction *BasicBlock_Instruction_iter (struct BasicBlock *basic_block, size_t *iter_count);
struct Use *Instruction_create_use (struct Instruction *instruction);
struct Value *Instruction_get_operand (struct Instruction *instruction, int operand_index);
struct Argument *Function_create_argument (struct Function *function);
char *Token_to_str (struct Token t);
void Constant_init (struct Constant *constant, int constant_value);