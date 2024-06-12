#include "threeaddr.h"
#include "array.h"
#include "constants.h"
#include "mem.h"
#include "threeaddr_parser.h"
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static size_t CURRENT_VALUE_NO = 0;
static size_t CURRENT_BASIC_BLOCK_NO = 0;

static char Token_buffer[MAX_IDENTIFIER_LEN + 1];

char *Token_to_str (struct Token t)
{
        switch (TOKEN_TYPE (t)) {
        case VARIABLE: {
                strcpy (Token_buffer, t.str_value);
                return Token_buffer;
        };
        case INSTRUCTION_ALLOCA: return "alloca";
        case INSTRUCTION_ADD: return "add";
        case INSTRUCTION_SUB: return "sub";
        case INSTRUCTION_MUL: return "mul";
        case INSTRUCTION_DIV: return "div";
        case INSTRUCTION_STORE: return "store";
        case INSTRUCTION_LOAD: return "load";
        case INSTRUCTION_JUMP: return "jump";
        case INSTRUCTION_JUMPIF: return "jumpif"; break;
        case INSTRUCTION_PHI: return "phi";
        case COMMA: return ",";
        case FN: return "fn";
        case LPAREN: return "(";
        case RPAREN: return ")";
        case INTEGER: {
                sprintf (Token_buffer, "%d", t.value);
                return Token_buffer;
        }
        case LABEL: {
                sprintf (Token_buffer, "%d:", t.value);
                return Token_buffer;
        }
        case STR: {
                strcpy (Token_buffer, t.str_value);
                return Token_buffer;
        }
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
        assert (CURRENT_VALUE_NO < VALUE_TABLE_SIZE);

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
        assert (CURRENT_BASIC_BLOCK_NO < MAX_BASIC_BLOCK_COUNT);
        basic_block->block_no = CURRENT_BASIC_BLOCK_NO++;
        basic_block->left = NULL;
        basic_block->right = NULL;

        Array_init (&basic_block->values, sizeof (struct Instruction *));

        Array_init (&basic_block->preds, sizeof (struct BasicBlock *));
}

size_t BasicBlock_get_Instruction_count (struct BasicBlock *basic_block)
{
        return Array_length (&basic_block->values);
}

void BasicBlock_set_left_child (struct BasicBlock *basic_block, struct BasicBlock *left_child)
{
        basic_block->left = left_child;

        Array_push (&left_child->preds, &basic_block);
}

void BasicBlock_set_right_child (struct BasicBlock *basic_block, struct BasicBlock *right_child)
{
        basic_block->right = right_child;

        Array_push (&right_child->preds, &right_child);
}

void BasicBlock_add_Instruction (struct BasicBlock *basic_block, struct Instruction *instruction)
{
        Array_push (&basic_block->values, &instruction);
        instruction->parent = basic_block;
}

struct BasicBlock *BasicBlock_preds_iter (struct BasicBlock *basic_block, size_t *iter_count)
{
        if (*iter_count >= Array_length (&basic_block->preds))
                return NULL;

        return Array_get_index (&basic_block->preds, (*iter_count)++);
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

struct Instruction *BasicBlock_Instruction_iter (struct BasicBlock *basic_block, size_t *iter_count)
{
        if (Array_length (&basic_block->values) == *iter_count)
                return NULL;

        struct Instruction *instruction = *(struct Instruction **)Array_get_index (&basic_block->values, *iter_count);

        (*iter_count)++;

        return instruction;
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
