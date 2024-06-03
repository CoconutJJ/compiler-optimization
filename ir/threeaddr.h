#pragma once
#include "threeaddr_parser.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#define DYNARR_INIT_SIZE_CNT 10
#define MAX_FN_ARG_COUNT     10
enum ValueType { VALUE_ARGUMENT, VALUE_INST, VALUE_CONST };

enum InstType { INST_UNARY, INST_BINARY, INST_NIL };

enum OpCode { OPCODE_ADD, OPCODE_SUB, OPCODE_MUL, OPCODE_DIV, OPCODE_NIL };

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
        struct Value *values;
        size_t values_count;
        size_t values_size;

        struct Function *parent;

        struct BasicBlock *left;
        struct BasicBlock *right;
};

struct Argument {
        struct Value value;
        struct Function * parent;
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

#define AS_BYTE_BUFFER(buf) ((uint8_t *)(buf))
#define AS_INST(inst)       ((struct Instruction *)(inst))
#define AS_VALUE(val)       ((struct Value *)(val))

#define DYNARR_INIT(buffer, count, size, item_size)                                                                    \
        dynarr_init ((void **)&(buffer), (void *)&(count), (void *)&(size), (item_size))
#define DYNARR_PUSH(buffer, count, size, item)                                                                         \
        dynarr_push ((void **)&(buffer), (void *)&(count), (void *)&(size), item, sizeof (item))
#define DYNARR_ALLOC(buffer, count, size, item_size)                                                                   \
        dynarr_push ((void **)&(buffer), (void *)&(count), (void *)&(size), NULL, item_size)
#define DYNARR_POP(buffer, count, size, item)                                                                          \
        dynarr_pop ((void **)&(buffer), (void *)&(count), (void *)&(size), sizeof (item))
#define DYNARR_INSERT(buffer, count, size, item, item_size, index)                                                     \
        dynarr_insert ((void **)&(buffer), (void *)&(count), (void *)&(size), item, sizeof (item), (index));
#define VALUE_IS_INST(value)    ((value)->value_type == VALUE_INST)
#define INST_IS_BINARY_OP(inst) ((inst)->inst_type == INST_BINARY)

void dynarr_init (void **buffer, size_t *count, size_t *size, size_t item_size);
void *dynarr_insert (void **buffer, size_t *count, size_t *size, void *item, size_t item_size, size_t insert_index);
void *dynarr_push (void **buffer, size_t *count, size_t *size, void *item, size_t item_size);
void dynarr_pop (void **buffer, size_t *count, size_t *size, void *pop_addr, size_t item_size);
void Value_init (struct Value *value);
void Use_init (struct Use *use);
void Instruction_init (struct Instruction *instruction);
void BasicBlock_init (struct BasicBlock *basic_block);
void Instruction_set_operand (struct Instruction *instruction, struct Value *operand, int operand_index);
void Function_init (struct Function *function);
bool Instruction_contains (struct Instruction *instruction, struct Value *value);
struct Instruction *BasicBlock_create_Instruction (struct BasicBlock *basic_block);
struct Use *Instruction_create_use (struct Instruction *instruction);
struct Value *Instruction_get_operand (struct Instruction *instruction, int operand_index);
struct Argument *Function_create_argument (struct Function *function);
char *Token_to_str (struct Token t);
void Constant_init (struct Constant *constant, int constant_value);