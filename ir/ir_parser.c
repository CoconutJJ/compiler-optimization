#include "ir_parser.h"
#include "threeaddr.h"
#include "threeaddr_parser.h"
#include <stdio.h>
#include <stdlib.h>

static struct Value **value_table = NULL;
static struct BasicBlock **label_table = NULL;

struct BasicBlock *BasicBlock_create ()
{
        struct BasicBlock *basic_block = malloc (sizeof (struct BasicBlock));

        if (!basic_block) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        BasicBlock_init (basic_block);

        return basic_block;
}

struct Function *Function_create ()
{
        struct Function *function = malloc (sizeof (struct Function));

        if (!function) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        Function_init (function);

        return function;
}

struct Constant *Constant_create (int constant_value)
{
        struct Constant *constant = malloc (sizeof (struct Constant));

        if (!constant) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        Constant_init (constant, constant_value);

        return constant;
}

void Value_table_init ()
{
        value_table = calloc (VALUE_TABLE_SIZE, sizeof (struct Value *));

        if (!value_table) {
                perror ("calloc");
                exit (EXIT_FAILURE);
        }
}

void Value_table_clear ()
{
        memset (value_table, 0, VALUE_TABLE_SIZE * sizeof (struct Value *));
}

void Label_table_init ()
{
        label_table = calloc (LABEL_TABLE_SIZE, sizeof (struct BasicBlock *));

        if (!label_table) {
                perror ("calloc");
                exit (EXIT_FAILURE);
        }
}

void Label_table_clear ()
{
        memset (label_table, 0, VALUE_TABLE_SIZE * sizeof (struct Value *));
}

void value_table_insert (struct Value *value, size_t index)
{
        if (value_table[index]) {
                fprintf (stderr, "Value table already has entry for index %ld\n", index);
                exit (EXIT_FAILURE);
        }

        value_table[index] = value;
}

struct Value *value_table_find (size_t index)
{
        if (!value_table[index]) {
                fprintf (stderr, "Value table does not have an entry for index %ld\n", index);
                exit (EXIT_FAILURE);
        }

        return value_table[index];
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

                struct Value *op = value_table_find (fst_op.value);

                Instruction_set_operand (instruction, op, operand_index);
        }
}

void parse_binary_operator_operands (struct Instruction *instruction)
{
        struct Token token = consume_token (VARIABLE, "Expected destination operand to be a variable!\n");

        value_table_insert (AS_VALUE (instruction), token.value);

        consume_token (COMMA, "Expected ',' after destination operand\n");

        parse_operand (instruction, 0);

        consume_token (COMMA, "Expected ',' after first operand\n");

        parse_operand (instruction, 1);
}

void parse_instruction (struct BasicBlock *basic_block)
{
        struct Instruction *new_instruction = BasicBlock_create_Instruction (basic_block);

        switch (TOKEN_TYPE (peek_token ())) {
        case INSTRUCTION_ADD: {
                new_instruction->inst_type = INST_BINARY;
                new_instruction->op_code = OPCODE_ADD;
                break;
        }
        case INSTRUCTION_SUB: {
                new_instruction->inst_type = INST_BINARY;
                new_instruction->op_code = OPCODE_SUB;
                break;
        }
        case INSTRUCTION_MUL: {
                new_instruction->inst_type = INST_BINARY;
                new_instruction->op_code = OPCODE_MUL;
                break;
        }
        case INSTRUCTION_DIV: {
                new_instruction->inst_type = INST_BINARY;
                new_instruction->op_code = OPCODE_DIV;
                break;
        }
        default: fprintf (stderr, "Expected instruction! Found %s instead", Token_to_str (peek_token ())); break;
        }
        advance_token ();
        parse_binary_operator_operands (new_instruction);
}

struct Function *parse_function ()
{
        Value_table_clear ();
        Label_table_clear ();
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

                        value_table_insert (AS_VALUE (arg), var.value);

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
                if (match_token (INTEGER)) {
                        consume_token (COLON, "Expected `:` after label\n");

                        if (current_basic_block) {
                                current_basic_block->left = BasicBlock_create ();
                                label_table[label.value] = current_basic_block->left;
                                current_basic_block = current_basic_block->left;
                        } else {
                                current_basic_block = BasicBlock_create ();
                                function->entry_basic_block = current_basic_block;
                        }

                } else if (TOKEN_TYPE (peek_token ()) == END || TOKEN_TYPE (peek_token ()) == FN) {
                        break;
                } else {
                        if (!current_basic_block) {
                                current_basic_block = BasicBlock_create ();
                                function->entry_basic_block = current_basic_block;
                        }

                        parse_instruction (current_basic_block);
                }
        }

        return function;
}

void display_basic_block (struct BasicBlock *basic_block)
{
        size_t instruction_count = BasicBlock_get_Instruction_count(basic_block);

        printf ("Basic Block\n"
                "-----------\n"
                "Instructions: %ld\n",
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

        Value_table_init ();
        Label_table_init ();

        return parse_function ();
}
