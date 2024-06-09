#include "ir_parser.h"
#include "constants.h"
#include "map.h"
#include "mem.h"
#include "threeaddr.h"
#include "threeaddr_parser.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
static struct HashTable value_table;
static struct HashTable label_table;

struct BasicBlock *BasicBlock_create ()
{
        struct BasicBlock *basic_block = ir_malloc (sizeof (struct BasicBlock));

        BasicBlock_init (basic_block);

        return basic_block;
}

struct Function *Function_create ()
{
        struct Function *function = ir_malloc (sizeof (struct Function));

        Function_init (function);

        return function;
}

struct Constant *Constant_create (int constant_value)
{
        struct Constant *constant = ir_malloc (sizeof (struct Constant));

        Constant_init (constant, constant_value);

        return constant;
}

struct Value *find_Value(uint64_t value_no) {

        struct Value *value = hash_table_search (&value_table, value_no);

        assert(value != NULL);

        return value;

}

void parse_operand (struct Instruction *instruction, int operand_index)
{
        struct Token token = peek_token ();
        if (match_token (INTEGER)) {
                struct Constant *op = Constant_create (token.value);
                Instruction_set_operand (instruction, AS_VALUE (op), operand_index);
        } else {
                struct Token fst_op =
                        consume_token (VARIABLE, "Expected variable or constant as %d operand!", operand_index + 1);

                struct Value *op = find_Value (fst_op.value);

                Instruction_set_operand (instruction, op, operand_index);
        }
}

void parse_binary_operator_operands (struct Instruction *instruction)
{
        struct Token token = consume_token (VARIABLE, "Expected destination operand to be a variable!\n");

        hash_table_insert (&value_table, token.value, AS_VALUE (instruction));

        consume_token (COMMA, "Expected ',' after destination operand\n");

        parse_operand (instruction, 0);

        consume_token (COMMA, "Expected ',' after first operand\n");

        parse_operand (instruction, 1);
}

void parse_branch_operand (struct Instruction *instruction)
{
        struct Token token = consume_token (INTEGER, "Expected label value for branch instruction argument!\n");

        Instruction_set_operand (instruction, AS_VALUE (Constant_create (token.value)), 0);

        if (instruction->op_code == OPCODE_JMPIF) {
                consume_token (COMMA, "Expected `, <condition>` after jumpif target label\n");
                parse_operand (instruction, 1);
        }
}

struct BasicBlock *find_BasicBlock (uint64_t label_no)
{
        struct BasicBlock *basic_block = hash_table_search (&label_table, label_no);

        if (!basic_block) {
                basic_block = BasicBlock_create ();
                hash_table_insert (&label_table, label_no, basic_block);
        }

        return basic_block;
}

struct Instruction *parse_instruction ()
{
        enum OpCode op;

        switch (TOKEN_TYPE (peek_token ())) {
        case INSTRUCTION_ADD: {
                op = OPCODE_ADD;
                break;
        }
        case INSTRUCTION_SUB: {
                op = OPCODE_SUB;
                break;
        }
        case INSTRUCTION_MUL: {
                op = OPCODE_MUL;
                break;
        }
        case INSTRUCTION_DIV: {
                op = OPCODE_DIV;
                break;
        }
        case INSTRUCTION_JUMP: {
                op = OPCODE_JMP;
                break;
        }
        case INSTRUCTION_JUMPIF: {
                op = OPCODE_JMPIF;
                break;
        }
        default:
                fprintf (stderr, "Expected instruction! Found %s instead", Token_to_str (peek_token ()));
                exit (EXIT_FAILURE);
                break;
        }

        advance_token ();

        struct Instruction *new_instruction = Instruction_create (op);

        if (INST_IS_BINARY_OP (new_instruction))
                parse_binary_operator_operands (new_instruction);
        else if (INST_IS_BRANCH (new_instruction))
                parse_branch_operand (new_instruction);

        return new_instruction;
}

bool parse_basic_block (struct BasicBlock *basic_block)
{
        while (1) {
                switch (TOKEN_TYPE (peek_token ())) {
                case LABEL: return true;
                case FN:
                case END: return false;
                default: {
                        struct Instruction *inst = parse_instruction ();

                        BasicBlock_add_Instruction (basic_block, inst);

                        if (INST_IS_BRANCH (inst)) {
                                struct Constant *jump_location = AS_CONST (Instruction_get_operand (inst, 0));

                                BasicBlock_set_right_child (basic_block, find_BasicBlock (jump_location->constant));

                                return true;
                        }
                }
                }
        }
}

struct Function *parse_function ()
{
        hash_table_empty (&value_table);

        hash_table_empty (&label_table);

        consume_token (FN, "Expected `fn` keyword for function.\n");

        struct Function *function = Function_create ();

        struct Token fn_name = consume_token (STR, "Expected function name after `fn` keyword.\n");

        consume_token (LPAREN, "Expected opening '(' after function name declaration.\n");

        if (!match_token (RPAREN)) {
                while (1) {
                        struct Token var = consume_token (VARIABLE,
                                                          "Expected argument in function argument list, got %s instead",
                                                          Token_to_str (peek_token ()));

                        struct Argument *arg = Function_create_argument (function);

                        hash_table_insert (&value_table, var.value, arg);

                        if (match_token (COMMA)) {
                                continue;
                        } else {
                                consume_token (RPAREN,
                                               "Expected closing ')' after argument list, found %s instead",
                                               Token_to_str (peek_token ()));
                                break;
                        }
                }
        }

        consume_token (COLON, "Expected `:` after function argument list\n");

        struct BasicBlock *current_basic_block = NULL;
        while (1) {
                struct Token label = peek_token ();

                if (match_token (LABEL)) {
                        if (current_basic_block) {
                                BasicBlock_set_left_child (current_basic_block, find_BasicBlock (label.value));
                                current_basic_block = current_basic_block->left;
                        } else {
                                current_basic_block = find_BasicBlock (label.value);
                                function->entry_basic_block = current_basic_block;
                        }

                } else {
                        if (current_basic_block) {
                                BasicBlock_set_left_child (current_basic_block, BasicBlock_create ());
                                current_basic_block = current_basic_block->left;
                        } else {
                                function->entry_basic_block = BasicBlock_create ();
                                current_basic_block = function->entry_basic_block;
                        }
                }
                if (!parse_basic_block (current_basic_block))
                        break;
        }

        return function;
}

void display_basic_block (struct BasicBlock *basic_block)
{
        size_t instruction_count = BasicBlock_get_Instruction_count (basic_block);

        printf ("+-----------------+\n"
                "| Basic Block %ld |\n"
                "+-----------------+\n"
                "| Inst: %ld       |\n"
                "+-----------------+\n",
                basic_block->block_no,
                instruction_count);

        if (basic_block->left)
                display_basic_block (basic_block->left);

        if (basic_block->right)
                display_basic_block (basic_block->right);
}

void display_function (struct Function *function)
{
        display_basic_block (function->entry_basic_block);
}

struct Function *parse_ir (char *ir_source)
{
        threeaddr_init_parser (ir_source);

        hash_table_init (&value_table, MAX_BASIC_BLOCK_COUNT * 5);
        hash_table_init (&label_table, MAX_BASIC_BLOCK_COUNT);

        return parse_function ();
}
