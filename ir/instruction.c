#include "instruction.h"
#include "array.h"
#include "basicblock.h"
#include "global_constants.h"
#include "lexer.h"
#include "mem.h"
#include "utils.h"
#include "value.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static struct SSAOperand *SSAOperand_create (struct Value *operand, struct BasicBlock *pred)
{
        struct SSAOperand *ssa_operand = ir_malloc (sizeof (struct SSAOperand));

        ssa_operand->operand = operand;
        ssa_operand->pred_block = pred;

        return ssa_operand;
}

static void SSAOperand_destroy (struct SSAOperand *operand)
{
        ir_free (operand);
}

void Instruction_init (struct Instruction *instruction)
{
        Value_init (&instruction->value);

        instruction->value.value_type = VALUE_INST;
        instruction->inst_type = INST_NIL;
        instruction->op_code = OPCODE_NIL;

        instruction->operands.first = NULL;
        instruction->operands.second = NULL;
}

void Instruction_free (struct Instruction *instruction)
{
        if (INST_ISA (instruction, OPCODE_PHI)) {
                Array_apply (&instruction->operand_list, (ArrayApplyFn)SSAOperand_destroy);
                Array_free (&instruction->operand_list);
        }

        Value_free (&instruction->value);
}

void Instruction_destroy (struct Instruction *instruction)
{
        Instruction_free (instruction);
        ir_free (instruction);
}

struct Value *Instruction_get_operand (struct Instruction *instruction, int operand_index)
{
        if (INST_ISA (instruction, OPCODE_PHI)) {
                struct SSAOperand *op = Array_get_index (&instruction->operand_list, operand_index);
                return op->operand;
        }

        switch (operand_index) {
        case 0: return instruction->operands.first;
        case 1:
                return instruction->operands.second;
                /* fall through if condition fails */
        default: return NULL;
        }
}

void Instruction_set_operand (struct Instruction *instruction, struct Value *operand, int operand_index)
{
        // back patches will set operand to NULL at first during parsing, this will be called again
        // in the patching step with a non null value
        if (operand)
                Use_link (AS_VALUE (instruction), operand, operand_index);

        if (INST_ISA (instruction, OPCODE_PHI)) {
                struct SSAOperand *op = Array_get_index (&instruction->operand_list, operand_index);
                op->operand = operand;
                return;
        }

        switch (operand_index) {
        case 0: instruction->operands.first = operand; break;
        case 1: instruction->operands.second = operand; break;
        default: UNREACHABLE ("Invalid operand index!");
        }
}

bool Instruction_Remove_From_Parent (struct Instruction *instruction)
{
        struct BasicBlock *parent = instruction->parent;

        return BasicBlockRemoveInstruction (parent, instruction);
}

struct Value *Instruction_Load_From_Operand (struct Instruction *instruction)
{
        ASSERT (INST_ISA (instruction, OPCODE_LOAD), "Must be a load instruction!");

        return Instruction_get_operand (instruction, 0);
}

struct Value *Instruction_Store_To_Operand (struct Instruction *instruction)
{
        ASSERT (INST_ISA (instruction, OPCODE_STORE), "Must be a store instruction!");

        return Instruction_get_operand (instruction, 0);
}

struct Value *Instruction_Store_From_Operand (struct Instruction *instruction)
{
        ASSERT (INST_ISA (instruction, OPCODE_STORE), "Must be a store instruction!");
        return Instruction_get_operand (instruction, 1);
}

void Instruction_push_phi_operand_list (struct Instruction *instruction, struct Value *operand, struct BasicBlock *pred)
{
        Array_push (&instruction->operand_list, SSAOperand_create (operand, pred));

        Use_link (AS_VALUE (instruction), operand, Array_length (&instruction->operand_list) - 1);
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

        while ((curr = BasicBlockInstructionIter (basic_block, &index)) != NULL) {
                if (curr == before)
                        break;
        }

        assert (curr != NULL && "Instruction_InsertBefore: instruction not found!\n");

        before->parent = basic_block;

        Array_insert (&basic_block->values, index, before);
}

struct Instruction *Instruction_create (enum OpCode op, struct Token dest_token)
{
        struct Instruction *instruction = ir_malloc (sizeof (struct Instruction));

        Instruction_init (instruction);
        instruction->op_code = op;

        Value_set_token (AS_VALUE (instruction), dest_token);

        switch (op) {
        case OPCODE_CMP:
        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_MUL:
        case OPCODE_DIV: instruction->inst_type = INST_BINARY; break;
        case OPCODE_JUMP:
        case OPCODE_JUMPIF: instruction->inst_type = INST_BRANCH; break;
        case OPCODE_LOAD:
        case OPCODE_STORE:
        case OPCODE_ALLOCA: instruction->inst_type = INST_MEM; break;
        case OPCODE_PHI: {
                instruction->inst_type = INST_MEM;
                Array_init (&instruction->operand_list);
                break;
        }

        default:
                error (dest_token, "Invalid instruction: %s", Token_to_str (dest_token));
                exit (EXIT_FAILURE);
                break;
        }

        return instruction;
}