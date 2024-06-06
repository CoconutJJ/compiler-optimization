#include "threeaddr.h"
#include "threeaddr_parser.h"
#include "array.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t CURRENT_VALUE_NO = 0;
static size_t CURRENT_BASIC_BLOCK_NO = 0;


char *Token_to_str (struct Token t)
{
        switch (TOKEN_TYPE (t)) {
        case VARIABLE: return "Variable";
        case INSTRUCTION_ADD: return "Instruction::Add";
        case INSTRUCTION_SUB: return "Instruction::Sub";
        case INSTRUCTION_MUL: return "Instruction::Mul";
        case INSTRUCTION_DIV: return "Instruction::Div";
        case INSTRUCTION_STORE: return "Instruction::Store";
        case INSTRUCTION_JUMP: return "Instruction::Jump";
        case INSTRUCTION_JUMPIF: return "Instruction::JUMPIF"; break;
        case COMMA: return ",";
        case FN: return "fn";
        case LPAREN: return "(";
        case RPAREN: return ")";
        case INTEGER: return "Integer";
        case STR: return "Str";
        case COLON: return ":";
        case AT: return "@";
        case LCURLY: return "{";
        case RCURLY: return "}";
        case END: return "END";
        }

        return NULL;
}

void Value_init (struct Value *value)
{
        value->value_no = CURRENT_VALUE_NO++;
        value->uses = NULL;
        value->uses_count = 0;
        value->uses_size = 0;
        DYNARR_INIT (value->uses, value->uses_count, value->uses_size, sizeof (struct Use));
}

void Use_init (struct Use *use)
{
        use->operand_no = -1;
        use->usee = NULL;
        use->user = NULL;
}

void Argument_init (struct Argument *argument)
{
        argument->value.value_type = VALUE_ARGUMENT;
        Value_init (&argument->value);
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

void BasicBlock_init (struct BasicBlock *basic_block)
{
        basic_block->block_no = CURRENT_BASIC_BLOCK_NO++;
        basic_block->values = NULL;
        basic_block->values_count = 0;
        basic_block->values_size = 0;

        DYNARR_INIT (
                basic_block->values, basic_block->values_count, basic_block->values_size, sizeof (struct Instruction));
}

size_t BasicBlock_get_Instruction_count(struct BasicBlock *basic_block)
{
        return basic_block->values_count / sizeof(struct Instruction);
}

struct Use *Value_create_use (struct Value *value)
{
        struct Use *new_use = DYNARR_ALLOC (value->uses, value->uses_count, value->uses_size, sizeof (struct Use));

        Use_init (new_use);

        new_use->usee = value;

        return new_use;
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
        case 1:
                if (INST_IS_BINARY_OP (instruction)) {
                        instruction->operands.second = operand;
                } else {
                        fprintf (stderr, "Invalid operand index! Cannot set operand 1 for non-binary operator!\n");
                        exit (EXIT_FAILURE);
                }

                break;
        default: fprintf (stderr, "Invalid operand index!\n"); exit (EXIT_FAILURE);
        }

        struct Use *use = Value_create_use (operand);
        use->operand_no = operand_index;
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

struct Instruction *BasicBlock_Instruction_iter (struct BasicBlock *basic_block, size_t *iter_count)
{
        struct Instruction *instruction =
                AS_INST (AS_BYTE_BUFFER (basic_block->values) + *iter_count * sizeof (struct Instruction));

        (*iter_count)++;

        return instruction;
}

struct Instruction *Instruction_InsertBefore (struct BasicBlock *basic_block, struct Instruction *before)
{
        size_t index = 0;

        struct Instruction *curr;

        while ((curr = BasicBlock_Instruction_iter (basic_block, &index)) != NULL) {
                if (curr == before)
                        break;
        }

        assert (curr != NULL && "Instruction_InsertBefore: instruction not found!\n");

        return DYNARR_INSERT (basic_block->values,
                              basic_block->values_count,
                              basic_block->values_size,
                              NULL,
                              sizeof (struct Instruction),
                              index);
}

struct Instruction *BasicBlock_create_Instruction (struct BasicBlock *basic_block)
{
        struct Instruction *instruction = DYNARR_ALLOC (
                basic_block->values, basic_block->values_count, basic_block->values_size, sizeof (struct Instruction));

        Instruction_init (instruction);

        instruction->parent = basic_block;

        return instruction;
}

void Function_init (struct Function *function)
{
        function->arguments_count = 0;
        function->entry_basic_block = NULL;
}

struct Argument *Function_create_argument (struct Function *function)
{
        assert (function->arguments_count < MAX_FN_ARG_COUNT);

        struct Argument *new_argument = function->arguments + function->arguments_count;

        Argument_init (new_argument);

        function->arguments_count++;

        return new_argument;
}

void Constant_init (struct Constant *constant, int constant_value)
{
        Value_init (&constant->value);
        constant->value.value_type = VALUE_CONST;
        constant->constant = constant_value;
}