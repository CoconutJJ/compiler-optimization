#include "instruction.h"
#include "global_constants.h"
#include "basicblock.h"
#include "mem.h"
#include "value.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
void Instruction_init (struct Instruction *instruction)
{
        Value_init (&instruction->value);

        instruction->value.value_type = VALUE_INST;
        instruction->inst_type = INST_NIL;
        instruction->op_code = OPCODE_NIL;

        instruction->operands.first = NULL;
        instruction->operands.second = NULL;
}

struct Value *Instruction_get_operand (struct Instruction *instruction, int operand_index)
{
        switch (operand_index) {
        case 0: return instruction->operands.first;
        case 1:
                if (INST_IS_BINARY_OP (instruction))
                        return instruction->operands.second;
                /* fall through if condition fails */
        default: return NULL;
        }
}

void Instruction_set_operand (struct Instruction *instruction, struct Value *operand, int operand_index)
{
        switch (operand_index) {
        case 0: instruction->operands.first = operand; break;
        case 1: instruction->operands.second = operand; break;
        default: fprintf (stderr, "Invalid operand index!\n"); exit (EXIT_FAILURE);
        }

        struct Use *use = Value_create_use (operand);
        use->operand_no = operand_index;
        use->user = AS_VALUE (instruction);
}

void Instruction_push_phi_operand_list (struct Instruction *instruction, struct Value *operand)
{
        Array_push (&instruction->operand_list, operand);

        struct Use *use = Value_create_use (operand);
        use->operand_no = Array_length (&instruction->operand_list) - 1;
        use->user = AS_VALUE (instruction);
}

bool Instruction_contains (struct Instruction *instruction, struct Value *value)
{
        struct Value *op = Instruction_get_operand (instruction, 0);

        if (op == value)
                return true;

        if (!INST_IS_BINARY_OP (instruction))
                return false;

        op = Instruction_get_operand (instruction, 1);

        return op == value;
}


void Instruction_InsertBefore (struct BasicBlock *basic_block, struct Instruction *before)
{
        size_t index = 0;

        struct Instruction *curr = NULL;

        while ((curr = BasicBlock_Instruction_iter (basic_block, &index)) != NULL) {
                if (curr == before)
                        break;
        }

        assert (curr != NULL && "Instruction_InsertBefore: instruction not found!\n");

        before->parent = basic_block;

        Array_insert (&basic_block->values, index, &before);
}

struct Instruction *Instruction_create (enum OpCode op)
{
        struct Instruction *instruction = ir_malloc (sizeof (struct Instruction));

        Instruction_init (instruction);
        instruction->op_code = op;

        switch (op) {
        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_MUL:
        case OPCODE_DIV: instruction->inst_type = INST_BINARY; break;
        case OPCODE_JUMP:
        case OPCODE_JUMPIF: instruction->inst_type = INST_BRANCH; break;
        case OPCODE_LOAD:
        case OPCODE_STORE:
        case OPCODE_ALLOCA: instruction->inst_type = INST_MEM; break;
        case OPCODE_PHI: Array_init (&instruction->operand_list, sizeof (struct Value *)); break;
        default: fprintf (stderr, "Invalid instruction!\n"); break;
        }

        return instruction;
}