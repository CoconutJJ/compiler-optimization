#include "parser.h"
#include "array.h"
#include "basicblock.h"
#include "constant.h"
#include "function.h"
#include "global_constants.h"
#include "instruction.h"
#include "lexer.h"
#include "map.h"
#include "mem.h"
#include "utils.h"
#include "value.h"
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static HashTable value_table;
static HashTable label_table;

static void check_valid_assignment_target (struct Token dest_token, char *error_message, ...)
{
        struct Value *value = hash_table_search (&value_table, dest_token.value);

        if (!value)
                return;

        va_list args;
        va_start (args, error_message);
        _va_error (dest_token, "error", error_message, args);
        va_end (args);

        if (VALUE_IS_INST (value)) {
                error (value->token, "%%%d is already defined here", value->token.value);
        }

        exit (EXIT_FAILURE);
}

static struct BasicBlock *BasicBlock_create (enum BasicBlockType type)
{
        struct BasicBlock *basic_block = ir_malloc (sizeof (struct BasicBlock));

        BasicBlock_init (basic_block, type);

        return basic_block;
}

static struct Function *Function_create ()
{
        struct Function *function = ir_malloc (sizeof (struct Function));

        Function_init (function);

        return function;
}

static struct Argument *Argument_create ()
{
        struct Argument *argument = ir_malloc (sizeof (struct Argument));

        Argument_init (argument);

        return argument;
}

static void Function_set_name (struct Function *function, char *name)
{
        assert (strlen (name) <= MAX_IDENTIFIER_LEN);

        strcpy (function->fn_name, name);
}

static struct Constant *Constant_create (struct Token constant_token)
{
        struct Constant *constant = ir_malloc (sizeof (struct Constant));

        Constant_init (constant, constant_token.value);

        Value_set_token (AS_VALUE (constant), constant_token);

        return constant;
}

static struct Value *find_Value (uint64_t value_no)
{
        struct Value *value = hash_table_search (&value_table, value_no);

        return value;
}

static void parse_operand (struct Instruction *instruction, int operand_index)
{
        struct Token token = peek_token ();
        if (match_token (INTEGER)) {
                struct Constant *op = Constant_create (token);
                Instruction_set_operand (instruction, AS_VALUE (op), operand_index);
        } else {
                struct Token fst_op =
                        consume_token (VARIABLE, "Expected variable or constant as %d operand!", operand_index + 1);

                struct Value *op = find_Value (fst_op.value);

                Instruction_set_operand (instruction, op, operand_index);
        }
}

static void parse_binary_operator_operands (struct Instruction *instruction)
{
        struct Token token = consume_token (VARIABLE, "Expected destination operand to be a variable!\n");

        check_valid_assignment_target (token, "Invalid assignment target %s", Token_to_str (token));

        hash_table_insert (&value_table, token.value, AS_VALUE (instruction));

        consume_token (COMMA, "Expected ',' after destination operand\n");

        parse_operand (instruction, 0);

        consume_token (COMMA, "Expected ',' after first operand\n");

        parse_operand (instruction, 1);
}

static void parse_branch_operand (struct Instruction *instruction)
{
        struct Token token = consume_token (INTEGER, "Expected label value for branch instruction argument!\n");

        Instruction_set_operand (instruction, AS_VALUE (Constant_create (token)), 0);

        if (instruction->op_code == OPCODE_JUMPIF) {
                consume_token (COMMA, "Expected `, <condition>` after jumpif target label\n");
                parse_operand (instruction, 1);
        }
}

static struct BasicBlock *find_BasicBlock (uint64_t label_no)
{
        struct BasicBlock *basic_block = hash_table_search (&label_table, label_no);

        if (!basic_block) {
                // if the block doesn't exist yet, create it. This is usually
                // because we are jumping to a label we have not yet parsed.
                basic_block = BasicBlock_create (BASICBLOCK_NORMAL);
                hash_table_insert (&label_table, label_no, basic_block);
        }

        return basic_block;
}

static void parse_phi_instruction (struct Instruction *phi_instruction)
{
        struct Token dest_token = consume_token (VARIABLE,
                                                 "Expected destination operand as first argument for PHI "
                                                 "instruction, got %s instead\n",
                                                 Token_to_str (peek_token ()));

        hash_table_insert (&value_table, dest_token.value, phi_instruction);

        do {
                struct Token token = peek_token ();

                if (match_token (VARIABLE)) {
                        Instruction_push_phi_operand_list (phi_instruction, find_Value (token.value));
                } else if (match_token (INTEGER)) {
                        Instruction_push_phi_operand_list (phi_instruction, AS_VALUE (Constant_create (token)));
                } else {
                        error (peek_token (),
                               "Expected either a variable or integer constant as PHI instruction operand, got %s instead\n",
                               Token_to_str (peek_token ()));
                        exit (EXIT_FAILURE);
                }
        } while (match_token (COMMA));
}

static void parse_alloca_instruction (struct Instruction *instruction)
{
        struct Token dest = consume_token (VARIABLE,
                                           "Expected target variable after `alloca` instruction, found %s instead",
                                           Token_to_str (peek_token ()));

        consume_token (COMMA,
                       "Expected `,` after destination operand %s, found %s instead",
                       Token_to_str (dest),
                       Token_to_str (peek_token ()));

        struct Token size = consume_token (
                INTEGER, "Expected `alloca` integer size argument, found %s instead", Token_to_str (peek_token ()));

        hash_table_insert (&value_table, dest.value, instruction);

        Instruction_set_operand (instruction, AS_VALUE (Constant_create (size)), 0);
}

static void parse_load_instruction (struct Instruction *instruction)
{
        struct Token dest = consume_token (VARIABLE,
                                           "Expected target variable after `load` instruction, found %s instead",
                                           Token_to_str (peek_token ()));

        check_valid_assignment_target (dest, "Invalid assignment target %s", Token_to_str (dest));

        hash_table_insert (&value_table, dest.value, instruction);

        consume_token (COMMA,
                       "Expected `,` after destination operand %s, found %s instead",
                       Token_to_str (dest),
                       Token_to_str (peek_token ()));

        struct Token address = consume_token (
                VARIABLE, "Expected alloca address target variable, found %s instead", Token_to_str (peek_token ()));

        struct Value *alloca_value = find_Value (address.value);

        if (!VALUE_IS_INST (alloca_value) || !INST_ISA (AS_INST (alloca_value), OPCODE_ALLOCA)) {
                error (address, "Expected address variable to be defined as target of `alloca` instruction.");
                error (alloca_value->token, "Definition of %s", Token_to_str (alloca_value->token));
                exit (EXIT_FAILURE);
        }

        Instruction_set_operand (instruction, alloca_value, 0);
}

static void parse_store_instruction (struct Instruction *instruction)
{
        struct Token address = consume_token (VARIABLE,
                                              "Expected target address after `store` instruction, found %s instead",
                                              Token_to_str (peek_token ()));

        struct Value *alloca_value = find_Value (address.value);

        if (!VALUE_IS_INST (alloca_value) || !INST_ISA (AS_INST (alloca_value), OPCODE_ALLOCA)) {
                error (address, "Expected address variable to be defined as target of `alloca` instruction.");
                error (alloca_value->token, "Definition of %s", Token_to_str (alloca_value->token));
                exit (EXIT_FAILURE);
        }

        Instruction_set_operand (instruction, alloca_value, 0);
        consume_token (COMMA,
                       "Expected `,` after target address %s, found %s instead",
                       Token_to_str (address),
                       Token_to_str (peek_token ()));

        struct Token src = peek_token ();

        if (match_token (VARIABLE)) {
                struct Value *src_value = find_Value (src.value);
                if (!src_value) {
                        error (src,
                               "No definition found for source variable %s in store instruction",
                               Token_to_str (src));
                        exit (EXIT_FAILURE);
                }
                Instruction_set_operand (instruction, src_value, 1);

        } else if (match_token (INTEGER)) {
                struct Value *const_src = AS_VALUE (Constant_create (src));

                Instruction_set_operand (instruction, const_src, 1);

        } else {
                error (src,
                       "Expected source variable after `,` for `store` instruction, found %s instead",
                       Token_to_str (peek_token ()));
                exit (EXIT_FAILURE);
        }
}

static struct Instruction *parse_instruction ()
{
        struct Instruction *new_instruction;

        struct Token inst_token = peek_token ();

        switch (TOKEN_TYPE (inst_token)) {
        case INSTRUCTION_ADD: {
                new_instruction = Instruction_create (OPCODE_ADD, inst_token);
                advance_token ();
                parse_binary_operator_operands (new_instruction);

                break;
        }
        case INSTRUCTION_SUB: {
                new_instruction = Instruction_create (OPCODE_SUB, inst_token);
                advance_token ();

                parse_binary_operator_operands (new_instruction);

                break;
        }
        case INSTRUCTION_MUL: {
                new_instruction = Instruction_create (OPCODE_MUL, inst_token);
                advance_token ();

                parse_binary_operator_operands (new_instruction);

                break;
        }
        case INSTRUCTION_DIV: {
                new_instruction = Instruction_create (OPCODE_DIV, inst_token);
                advance_token ();

                parse_binary_operator_operands (new_instruction);

                break;
        }
        case INSTRUCTION_CMP: {
                new_instruction = Instruction_create (OPCODE_CMP, inst_token);
                advance_token ();

                parse_binary_operator_operands (new_instruction);
                break;
        }
        case INSTRUCTION_JUMP: {
                new_instruction = Instruction_create (OPCODE_JUMP, inst_token);
                advance_token ();

                parse_branch_operand (new_instruction);
                break;
        }
        case INSTRUCTION_JUMPIF: {
                new_instruction = Instruction_create (OPCODE_JUMPIF, inst_token);
                advance_token ();

                parse_branch_operand (new_instruction);
                break;
        }
        case INSTRUCTION_ALLOCA: {
                new_instruction = Instruction_create (OPCODE_ALLOCA, inst_token);
                advance_token ();

                parse_alloca_instruction (new_instruction);
                break;
        }
        case INSTRUCTION_PHI: {
                new_instruction = Instruction_create (OPCODE_PHI, inst_token);
                advance_token ();

                parse_phi_instruction (new_instruction);
                break;
        }
        case INSTRUCTION_LOAD: {
                new_instruction = Instruction_create (OPCODE_LOAD, inst_token);
                advance_token ();

                parse_load_instruction (new_instruction);
                break;
        }
        case INSTRUCTION_STORE: {
                new_instruction = Instruction_create (OPCODE_STORE, inst_token);
                advance_token ();

                parse_store_instruction (new_instruction);
                break;
        }
        default: return NULL;
        }

        return new_instruction;
}

static struct BasicBlock *parse_basic_block ()
{
        struct BasicBlock *basic_block;
        struct Token label = peek_token ();
        bool has_progress = false;

        if (match_token (LABEL)) {
                basic_block = find_BasicBlock (label.value);
                has_progress = true;
        } else {
                basic_block = BasicBlock_create (BASICBLOCK_NORMAL);
        }

        struct Instruction *inst;

        while ((inst = parse_instruction ()) != NULL) {
                has_progress = true;
                BasicBlock_add_Instruction (basic_block, inst);

                if (INST_IS_BRANCH (inst)) {
                        struct Constant *jump_location = AS_CONST (Instruction_get_operand (inst, 0));
                        BasicBlock_set_right_child (basic_block, find_BasicBlock (jump_location->constant));
                        return basic_block;
                }
        }

        if (!has_progress) {
                BasicBlock_free (basic_block);
                return NULL;
        }

        return basic_block;
}

static struct BasicBlock *add_entry_and_exit_blocks (struct BasicBlock *root)
{
        struct BasicBlock *entry = BasicBlock_create (BASICBLOCK_ENTRY);
        struct BasicBlock *exit = BasicBlock_create (BASICBLOCK_EXIT);

        struct Array stack;
        Array_init (&stack);
        Array_push (&stack, root);

        while (Array_length (&stack) > 0) {
                struct BasicBlock *curr = Array_pop (&stack);

                // add the left and right child
                if (!curr->left) {
                        BasicBlock_set_left_child (curr, exit);

                        Array_push (&stack, curr->right);
                }

                if (!curr->right) {
                        BasicBlock_set_right_child (curr, exit);

                        Array_push (&stack, curr->left);
                }
        }

        BasicBlock_set_left_child (entry, root);

        Array_free (&stack);

        return entry;
}

static struct BasicBlock *parse_block (struct Token function_name)
{
        consume_token (LCURLY, "Expected opening `{` after function %s", Token_to_str (function_name));
        struct BasicBlock *curr_block = NULL, *root = NULL;

        while (!match_token (RCURLY)) {
                struct BasicBlock *target_block = parse_basic_block ();

                if (!target_block) {
                        error (peek_token (),
                               "Expected closing '}', but found %s instead",
                               Token_to_str (peek_token ()));
                        exit (EXIT_FAILURE);
                }

                if (curr_block)
                        BasicBlock_set_left_child (curr_block, target_block);
                else
                        root = target_block;

                curr_block = target_block;
        }

        return root;
}

static struct Function *parse_function ()
{
        hash_table_empty (&value_table);
        hash_table_empty (&label_table);

        consume_token (FN, "Expected `fn` keyword for function.\n");

        struct Function *function = Function_create ();

        struct Token fn_name = consume_token (STR, "Expected function name after `fn` keyword.\n");

        Function_set_name (function, fn_name.str_value);

        consume_token (LPAREN, "Expected opening '(' after function name declaration.\n");

        if (!match_token (RPAREN)) {
                // parse function argument list
                while (1) {
                        struct Token var = consume_token (VARIABLE,
                                                          "Expected argument in function argument list, got %s instead",
                                                          Token_to_str (peek_token ()));

                        struct Argument *arg = Argument_create ();

                        Function_add_argument (function, arg);

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

        struct BasicBlock *root = parse_block (fn_name);

        function->entry_basic_block = add_entry_and_exit_blocks (root);

        Function_update_block_number_mapping (function);

        return function;
}

static void display_basic_block (struct BasicBlock *basic_block, struct HashTable *visited)
{
        if (hash_table_search (visited, basic_block->block_no) != NULL)
                return;
        else
                hash_table_insert (visited, basic_block->block_no, basic_block);

        size_t instruction_count = BasicBlock_get_Instruction_count (basic_block);

        int left_child_no = -1, right_child_no = -1;

        if (basic_block->left)
                left_child_no = basic_block->left->block_no;

        if (basic_block->right)
                right_child_no = basic_block->right->block_no;

        if (BASICBLOCK_IS_ENTRY (basic_block))
                printf ("ENTRY\n");
        else if (BASICBLOCK_IS_EXIT (basic_block))
                printf ("EXIT\n");

        printf ("+------------------+\n"
                "| Basic Block: %4ld|\n"
                "+------------------+\n"
                "| Inst. Count: %4ld|\n"
                "| Left Child:  %4d|\n"
                "| Right Child: %4d|\n"
                "+------------------+\n\n",
                basic_block->block_no,
                instruction_count,
                left_child_no,
                right_child_no);

        if (basic_block->left)
                display_basic_block (basic_block->left, visited);

        if (basic_block->right)
                display_basic_block (basic_block->right, visited);
}

void display_function (struct Function *function)
{
        struct HashTable visited;
        hash_table_init (&visited);
        display_basic_block (function->entry_basic_block, &visited);
}

struct Function *parse_ir (char *ir_source)
{
        threeaddr_init_parser (ir_source);

        hash_table_init (&value_table);
        hash_table_init (&label_table);

        return parse_function ();
}
