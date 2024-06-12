#pragma once

#include "array.h"
#include "value.h"
enum InstType { INST_UNARY, INST_BINARY, INST_NIL, INST_BRANCH, INST_MEM };
enum OpCode {
        OPCODE_ADD,
        OPCODE_SUB,
        OPCODE_MUL,
        OPCODE_DIV,
        OPCODE_JUMP,
        OPCODE_JUMPIF,
        OPCODE_NIL,
        OPCODE_PHI,
        OPCODE_STORE,
        OPCODE_LOAD,
        OPCODE_ALLOCA
};

struct Instruction {
        struct Value value;
        struct BasicBlock *parent;
        enum InstType inst_type;
        enum OpCode op_code;

        union {
                struct {
                        struct Value *first;
                        struct Value *second;
                } operands;
                struct Array operand_list;
        };
};

#define INST_IS_BINARY_OP(inst) ((inst)->inst_type == INST_BINARY)
#define INST_IS_BRANCH(inst)    ((inst)->inst_type == INST_BRANCH)

void Instruction_init (struct Instruction *instruction);
void Instruction_set_operand (struct Instruction *instruction, struct Value *operand, int operand_index);
bool Instruction_contains (struct Instruction *instruction, struct Value *value);
struct Instruction *Instruction_create (enum OpCode op);
struct Use *Instruction_create_use (struct Instruction *instruction);
struct Value *Instruction_get_operand (struct Instruction *instruction, int operand_index);
void Instruction_push_phi_operand_list (struct Instruction *instruction, struct Value *operand);