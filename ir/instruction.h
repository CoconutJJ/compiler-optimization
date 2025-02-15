#pragma once

#include "array.h"
#include "lexer.h"
#include "value.h"

enum InstType { INST_UNARY, INST_BINARY, INST_NIL, INST_BRANCH, INST_MEM };
enum OpCode {
        OPCODE_ADD,
        OPCODE_SUB,
        OPCODE_MUL,
        OPCODE_DIV,
        OPCODE_CMP,
        OPCODE_JUMP,
        OPCODE_JUMPIF,
        OPCODE_NIL,
        OPCODE_PHI,
        OPCODE_RET,
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

struct SSAOperand {
        struct Value *operand;
        struct BasicBlock *pred_block;
};

#define INST_IS_BINARY_OP(inst) ((inst)->inst_type == INST_BINARY)
#define INST_IS_BRANCH(inst)    ((inst)->inst_type == INST_BRANCH)
#define INST_ISA(inst, opcode)  ((inst)->op_code == (opcode))

void Instruction_init (struct Instruction *instruction);
void InstructionSetOperand (struct Instruction *instruction, struct Value *operand, int operand_index);
struct Value *InstructionGetOperand (struct Instruction *instruction, int operand_index);
bool Instruction_contains (struct Instruction *instruction, struct Value *value);
void Instruction_InsertBefore (struct BasicBlock *basic_block, struct Instruction *before);
struct Instruction *Instruction_create (enum OpCode op, struct Token dest_token);
struct Use *Instruction_create_use (struct Instruction *instruction);
struct Value *InstructionGetOperand (struct Instruction *instruction, int operand_index);
void Instruction_push_phi_operand_list (struct Instruction *instruction,
                                        struct Value *operand,
                                        struct BasicBlock *pred);
struct Value *Instruction_Load_From_Operand (struct Instruction *instruction);
struct Value *Instruction_Store_To_Operand (struct Instruction *instruction);
struct Value *Instruction_Store_From_Operand (struct Instruction *instruction);
bool Instruction_Remove_From_Parent (struct Instruction *instruction);
void Instruction_free (struct Instruction *instruction);
void Instruction_destroy (struct Instruction *instruction);
size_t InstructionGetOperandCount (struct Instruction *instruction);