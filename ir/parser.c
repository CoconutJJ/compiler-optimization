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
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Parser parser;

// static size_t ParserAllocateLabel (struct Parser *parser)
// {
//         return parser->current_label++;
// }

// static struct BasicBlock *ParserFindBlock (struct Parser *parser, size_t new_label)
// {
//         struct BasicBlock *block = hash_table_search (&parser->function->block_number_map, new_label);

//         if (!block) {
//                 block = BasicBlockCreate (BASICBLOCK_NORMAL);
//                 hash_table_insert (&parser->function->block_number_map, new_label, block);
//         }

//         return block;
// }

// static size_t ParserGetBlockLabel (struct BasicBlock *block)
// {
//         return block->block_no;
// }

static void BackPatchDestroy (struct BackPatch *patch)
{
        ir_free (patch);
}

static struct BasicBlock *ParserGetBlockByLabel (struct Parser *parser, size_t label)
{
        struct BasicBlock *block = hash_table_search (&parser->label_table, label);

        if (!block) {
                block = BasicBlockCreate (BASICBLOCK_NORMAL);
                hash_table_insert (&parser->label_table, label, block);
        }

        return block;
}

static struct ValueRecord *ParserFindValue (struct Parser *parser, uint64_t variable_no)
{
        struct ValueRecord *record = hash_table_search (&parser->value_table, variable_no);

        if (!record)
                return NULL;

        return record;
}

static void ParserFinalizeValue (struct Parser *parser, uint64_t variable_no)
{
        struct ValueRecord *record = hash_table_search (&parser->value_table, variable_no);

        record->finalized = true;
}

static void ParserInstructionSetOperand (struct Parser *parser,
                                         struct Instruction *instruction,
                                         uint64_t variable_no,
                                         size_t operand_no)
{
        struct ValueRecord *op = ParserFindValue (parser, variable_no);

        if (!op) {
                struct BackPatch *patch = ir_malloc (sizeof (struct BackPatch));

                patch->instruction = instruction;
                patch->operand_no = operand_no;
                patch->variable_no = variable_no;

                Array_push (&parser->back_patches, patch);
                InstructionSetOperand (instruction, NULL, operand_no);
                return;
        }

        if (!op->finalized) {
                error (op->value->token,
                       "Attempting to use partially defined variable %%%zu defined here",
                       variable_no);
                exit (EXIT_FAILURE);
        }

        InstructionSetOperand (instruction, op->value, operand_no);
}

static void ParserInstructionPushPhiOperand (struct Parser *parser,
                                             struct Instruction *instruction,
                                             uint64_t variable_no,
                                             struct BasicBlock *pred)
{
        struct ValueRecord *op = ParserFindValue (parser, variable_no);

        Instruction_push_phi_operand_list (instruction, op->value, pred);

        if (!op) {
                struct BackPatch *patch = ir_malloc (sizeof (struct BackPatch));

                patch->instruction = instruction;
                patch->operand_no = Array_length (&instruction->operand_list) - 1;
                patch->variable_no = variable_no;

                Array_push (&parser->back_patches, patch);
        }
}

static bool ParserInsertValue (struct Parser *parser, size_t variable_no, struct Value *value)
{
        if (hash_table_search (&parser->value_table, variable_no)) {
                return false;
        }

        struct ValueRecord *record = ir_malloc (sizeof (struct ValueRecord));
        record->value = value;
        record->finalized = false;

        hash_table_insert (&parser->value_table, variable_no, record);

        return true;
}

static void ParserInit (struct Parser *parser)
{
        Array_init (&parser->back_patches);
        hash_table_init (&parser->value_table);
        hash_table_init (&parser->label_table);
}

static void ParserFree (struct Parser *parser)
{
        Array_apply (&parser->back_patches, (ArrayApplyFn)BackPatchDestroy);
        Array_free (&parser->back_patches);
        hash_table_free (&parser->value_table);
        hash_table_free (&parser->label_table);
}

static void ParserCheckValidAssignmentTarget (struct Token dest_token, char *error_message, ...)
{
        struct Value *value = hash_table_search (&parser.value_table, dest_token.value);

        if (!value)
                return;

        va_list args;
        va_start (args, error_message);
        va_error (dest_token, "error", error_message, args);
        va_end (args);

        if (VALUE_IS_INST (value)) {
                error (value->token, "%%%d is already defined here", dest_token.value);
        }

        exit (EXIT_FAILURE);
}

static struct Function *FunctionCreate ()
{
        struct Function *function = ir_malloc (sizeof (struct Function));

        FunctionInit (function);

        return function;
}

static struct Argument *ArgumentCreate ()
{
        struct Argument *argument = ir_malloc (sizeof (struct Argument));

        ArgumentInit (argument);

        return argument;
}

static void FunctionSetName (struct Function *function, char *name)
{
        assert (strlen (name) <= MAX_IDENTIFIER_LEN);

        strcpy (function->fn_name, name);
}

static struct Constant *ConstantCreateFromToken (struct Token constant_token)
{
        return ConstantCreate (constant_token.value);
}

static void ParseOperand (struct Instruction *instruction, int operand_index)
{
        struct Token token = peek_token ();
        if (match_token (INTEGER)) {
                struct Constant *op = ConstantCreateFromToken (token);
                InstructionSetOperand (instruction, AS_VALUE (op), operand_index);
        } else {
                struct Token fst_op =
                        consume_token (VARIABLE, "Expected variable or constant as %d operand!", operand_index + 1);

                ParserInstructionSetOperand (&parser, instruction, fst_op.value, operand_index);
        }
}

static void ParseBinaryOperatorOperands (struct Instruction *instruction)
{
        struct Token token = consume_token (VARIABLE, "Expected destination operand to be a variable!\n");

        ParserCheckValidAssignmentTarget (token, "Invalid assignment target %s", Token_to_str (token));

        if (!ParserInsertValue (&parser, token.value, AS_VALUE (instruction))) {
                error (token, "Redefinition of variable %%%llu", token.value);
                struct ValueRecord *prev = ParserFindValue (&parser, token.value);
                error (prev->value->token, "Previously was defined here");
                exit (EXIT_FAILURE);
        }

        consume_token (COMMA, "Expected ',' after destination operand\n");
        ParseOperand (instruction, 0);
        consume_token (COMMA, "Expected ',' after first operand\n");
        ParseOperand (instruction, 1);

        ParserFinalizeValue (&parser, token.value);
}

static void ParseBranchOperand (struct Instruction *instruction)
{
        consume_token (LABEL_LITERAL,
                       "%s instruction requires `label` literal prefix before specifying label",
                       Token_to_str (instruction->value.token));

        struct Token token = consume_token (INTEGER, "Expected label value for branch instruction argument!\n");

        InstructionSetOperand (instruction, AS_VALUE (ConstantCreateFromToken (token)), 0);

        if (instruction->op_code == OPCODE_JUMPIF) {
                consume_token (COMMA, "Expected `, <condition>` after jumpif target label\n");
                ParseOperand (instruction, 1);
        }
}

static void ParsePhiInstruction (struct Instruction *phi_instruction)
{
        struct Token dest_token = consume_token (VARIABLE,
                                                 "Expected destination operand as first argument for PHI "
                                                 "instruction, got %s instead\n",
                                                 Token_to_str (peek_token ()));

        ParserInsertValue (&parser, dest_token.value, AS_VALUE (phi_instruction));
        do {
                consume_token (LBRACKET, "Expected opening '[' for PHI instruction operand pair [operand, label].");
                struct Token token = peek_token ();

                struct Value *operand = NULL;

                if (match_token (VARIABLE)) {
                        consume_token (COMMA, "Expected ',' after PHI instruction operand");

                        struct Token label_token =
                                consume_token (INTEGER, "Expected label value after phi function operand.");

                        consume_token (RBRACKET, "Expected closing ']' after PHI instruction operand pair.");

                        ParserInstructionPushPhiOperand (&parser,
                                                         phi_instruction,
                                                         token.value,
                                                         ParserGetBlockByLabel (&parser, label_token.value));
                } else if (match_token (INTEGER)) {
                        operand = AS_VALUE (ConstantCreateFromToken (token));
                        consume_token (COMMA, "Expected ',' after PHI instruction operand");

                        struct Token label_token =
                                consume_token (INTEGER, "Expected label value after phi function operand.");

                        consume_token (RBRACKET, "Expected closing ']' after PHI instruction operand pair.");

                        Instruction_push_phi_operand_list (
                                phi_instruction, operand, ParserGetBlockByLabel (&parser, label_token.value));
                } else {
                        error (peek_token (),
                               "Expected either a variable or integer constant as PHI instruction operand, got %s instead\n",
                               Token_to_str (peek_token ()));
                        exit (EXIT_FAILURE);
                }

        } while (match_token (COMMA));
        ParserFinalizeValue (&parser, dest_token.value);
}

static void ParseAllocaInstruction (struct Instruction *instruction)
{
        struct Token dest = consume_token (VARIABLE,
                                           "Expected target variable after `alloca` instruction, found %s instead",
                                           Token_to_str (peek_token ()));

        ParserInsertValue (&parser, dest.value, AS_VALUE (instruction));
        consume_token (COMMA,
                       "Expected `,` after destination operand %s, found %s instead",
                       Token_to_str (dest),
                       Token_to_str (peek_token ()));

        struct Token size = consume_token (
                INTEGER, "Expected `alloca` integer size argument, found %s instead", Token_to_str (peek_token ()));

        InstructionSetOperand (instruction, AS_VALUE (ConstantCreateFromToken (size)), 0);
        ParserFinalizeValue (&parser, dest.value);
}

static void ParseLoadInstruction (struct Instruction *instruction)
{
        struct Token dest = consume_token (VARIABLE,
                                           "Expected target variable after `load` instruction, found %s instead",
                                           Token_to_str (peek_token ()));

        ParserCheckValidAssignmentTarget (dest, "Invalid assignment target %s", Token_to_str (dest));
        ParserInsertValue (&parser, dest.value, AS_VALUE (instruction));

        consume_token (COMMA,
                       "Expected `,` after destination operand %s, found %s instead",
                       Token_to_str (dest),
                       Token_to_str (peek_token ()));

        struct Token address = consume_token (
                VARIABLE, "Expected alloca address target variable, found %s instead", Token_to_str (peek_token ()));

        ParserInstructionSetOperand (&parser, instruction, address.value, 0);
        ParserFinalizeValue (&parser, dest.value);
}

static void ParseStoreInstruction (struct Instruction *instruction)
{
        struct Token address = consume_token (VARIABLE,
                                              "Expected target address after `store` instruction, found %s instead",
                                              Token_to_str (peek_token ()));

        ParserInstructionSetOperand (&parser, instruction, address.value, 0);

        consume_token (COMMA,
                       "Expected `,` after target address %s, found %s instead",
                       Token_to_str (address),
                       Token_to_str (peek_token ()));

        struct Token src = peek_token ();

        if (match_token (VARIABLE)) {
                ParserInstructionSetOperand (&parser, instruction, src.value, 1);

        } else if (match_token (INTEGER)) {
                struct Value *const_src = AS_VALUE (ConstantCreateFromToken (src));

                InstructionSetOperand (instruction, const_src, 1);

        } else {
                error (src,
                       "Expected source variable after `,` for `store` instruction, found %s instead",
                       Token_to_str (peek_token ()));
                exit (EXIT_FAILURE);
        }
}

static void ParseRetInstruction (struct Instruction *instruction)
{
        struct Token arg = peek_token ();
        struct Value *value;
        if (match_token (VARIABLE)) {
                ParserInstructionSetOperand (&parser, instruction, arg.value, 0);
        } else if (match_token (INTEGER)) {
                value = AS_VALUE (ConstantCreate (arg.value));
                InstructionSetOperand (instruction, value, 0);
        }

        // return value is optional.

        return;
}

static struct Instruction *ParseInstruction ()
{
        struct Instruction *new_instruction;

        struct Token inst_token = peek_token ();

        switch (TOKEN_TYPE (inst_token)) {
        case INSTRUCTION_ADD: {
                new_instruction = Instruction_create (OPCODE_ADD, inst_token);
                advance_token ();
                ParseBinaryOperatorOperands (new_instruction);

                break;
        }
        case INSTRUCTION_SUB: {
                new_instruction = Instruction_create (OPCODE_SUB, inst_token);
                advance_token ();

                ParseBinaryOperatorOperands (new_instruction);

                break;
        }
        case INSTRUCTION_MUL: {
                new_instruction = Instruction_create (OPCODE_MUL, inst_token);
                advance_token ();

                ParseBinaryOperatorOperands (new_instruction);

                break;
        }
        case INSTRUCTION_DIV: {
                new_instruction = Instruction_create (OPCODE_DIV, inst_token);
                advance_token ();

                ParseBinaryOperatorOperands (new_instruction);

                break;
        }
        case INSTRUCTION_CMP: {
                new_instruction = Instruction_create (OPCODE_CMP, inst_token);
                advance_token ();

                ParseBinaryOperatorOperands (new_instruction);
                break;
        }
        case INSTRUCTION_JUMP: {
                new_instruction = Instruction_create (OPCODE_JUMP, inst_token);
                advance_token ();

                ParseBranchOperand (new_instruction);
                break;
        }
        case INSTRUCTION_JUMPIF: {
                new_instruction = Instruction_create (OPCODE_JUMPIF, inst_token);
                advance_token ();

                ParseBranchOperand (new_instruction);
                break;
        }
        case INSTRUCTION_ALLOCA: {
                new_instruction = Instruction_create (OPCODE_ALLOCA, inst_token);
                advance_token ();

                ParseAllocaInstruction (new_instruction);
                break;
        }
        case INSTRUCTION_PHI: {
                new_instruction = Instruction_create (OPCODE_PHI, inst_token);
                advance_token ();

                ParsePhiInstruction (new_instruction);
                break;
        }
        case INSTRUCTION_LOAD: {
                new_instruction = Instruction_create (OPCODE_LOAD, inst_token);
                advance_token ();

                ParseLoadInstruction (new_instruction);
                break;
        }
        case INSTRUCTION_STORE: {
                new_instruction = Instruction_create (OPCODE_STORE, inst_token);
                advance_token ();

                ParseStoreInstruction (new_instruction);
                break;
        }
        case INSTRUCTION_RET: {
                new_instruction = Instruction_create (OPCODE_RET, inst_token);
                advance_token ();
                ParseRetInstruction (new_instruction);
                break;
        }

        default: return NULL;
        }

        return new_instruction;
}

static struct BasicBlock *ParseBasicBlock ()
{
        struct BasicBlock *basic_block;
        struct Token label = peek_token ();
        bool has_progress = false;

        if (match_token (LABEL)) {
                basic_block = ParserGetBlockByLabel (&parser, label.value);
                has_progress = true;
        } else {
                basic_block = BasicBlockCreate (BASICBLOCK_NORMAL);
        }

        struct Instruction *inst;

        while ((inst = ParseInstruction ()) != NULL) {
                has_progress = true;
                BasicBlockAddInstruction (basic_block, inst);

                if (INST_IS_BRANCH (inst)) {
                        struct Constant *jump_location = AS_CONST (InstructionGetOperand (inst, 0));
                        BasicBlockSetRightChild (basic_block, ParserGetBlockByLabel (&parser, jump_location->constant));
                        return basic_block;
                }
        }

        if (!has_progress) {
                BasicBlockFree (basic_block);
                return NULL;
        }

        return basic_block;
}

static struct BasicBlock *AddEntryAndExitBlocks (struct BasicBlock *root)
{
        struct BasicBlock *entry = BasicBlockCreate (BASICBLOCK_ENTRY);
        struct BasicBlock *exit = BasicBlockCreate (BASICBLOCK_EXIT);

        struct Array stack;
        Array_init (&stack);
        Array_push (&stack, root);

        while (Array_length (&stack) > 0) {
                struct BasicBlock *curr = Array_pop (&stack);

                // add the left and right child
                if (!curr->left) {
                        BasicBlockSetLeftChild (curr, exit);

                        Array_push (&stack, curr->right);
                }

                if (!curr->right) {
                        BasicBlockSetRightChild (curr, exit);

                        Array_push (&stack, curr->left);
                }
        }

        BasicBlockSetLeftChild (entry, root);
        entry->next = root;
        Array_free (&stack);

        return entry;
}

static struct BasicBlock *ParseBlock (struct Token function_name)
{
        consume_token (LCURLY, "Expected opening `{` after function %s", Token_to_str (function_name));
        struct BasicBlock *curr_block = NULL, *root = NULL;

        while (!match_token (RCURLY)) {
                struct BasicBlock *target_block = ParseBasicBlock ();

                if (!target_block) {
                        error (peek_token (),
                               "Expected closing '}', but found %s instead",
                               Token_to_str (peek_token ()));
                        exit (EXIT_FAILURE);
                }

                if (curr_block) {
                        struct Instruction *last_inst = BasicBlockLastInstruction (curr_block);

                        if (!INST_ISA (last_inst, OPCODE_JUMP))
                                BasicBlockSetLeftChild (curr_block, target_block);

                        curr_block->next = target_block;

                } else {
                        root = target_block;
                }
                curr_block = target_block;
        }

        return root;
}

static void ResolveBackPatches (struct Parser *parser)
{
        struct BackPatch *patch = NULL;
        size_t iter_count = 0;

        while ((patch = Array_iter (&parser->back_patches, &iter_count)) != NULL) {
                struct ValueRecord *value = ParserFindValue (parser, patch->variable_no);

                if (!value) {
                        error (patch->instruction->value.token,
                               "Variable %%%zu is used but undefined!\n",
                               patch->variable_no);
                        exit (EXIT_FAILURE);
                }

                ASSERT (InstructionGetOperand (patch->instruction, patch->operand_no) == NULL,
                        "Invalid backpatch! Value has already been patched!");

                InstructionSetOperand (patch->instruction, value->value, patch->operand_no);
        }
}

static struct Function *ParseFunction ()
{
        consume_token (FN, "Expected `fn` keyword for function.\n");

        struct Function *function = FunctionCreate ();

        struct Token fn_name = consume_token (STR, "Expected function name after `fn` keyword.\n");

        FunctionSetName (function, fn_name.str_value);

        consume_token (LPAREN, "Expected opening '(' after function name declaration.\n");

        if (!match_token (RPAREN)) {
                // parse function argument list
                while (1) {
                        struct Token var = consume_token (VARIABLE,
                                                          "Expected argument in function argument list, got %s instead",
                                                          Token_to_str (peek_token ()));

                        struct Argument *arg = ArgumentCreate ();

                        FunctionAddArgument (function, arg);

                        ParserInsertValue (&parser, var.value, AS_VALUE (arg));
                        ParserFinalizeValue (&parser, var.value);
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

        struct BasicBlock *root = ParseBlock (fn_name);

        ResolveBackPatches (&parser);

        function->entry_basic_block = AddEntryAndExitBlocks (root);

        FunctionComputeBlockNumberMapping (function);

        return function;
}

static void PrintBasicBlock (struct BasicBlock *basic_block, struct HashTable *visited)
{
        if (hash_table_search (visited, basic_block->block_no) != NULL)
                return;
        else
                hash_table_insert (visited, basic_block->block_no, basic_block);

        size_t instruction_count = BasicBlockGetInstructionCount (basic_block);

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
                PrintBasicBlock (basic_block->left, visited);

        if (basic_block->right)
                PrintBasicBlock (basic_block->right, visited);
}

void PrintFunction (struct Function *function)
{
        struct HashTable visited;
        hash_table_init (&visited);
        PrintBasicBlock (function->entry_basic_block, &visited);
}

struct Function *ParseIR (char *ir_source)
{
        threeaddr_init_parser (ir_source);

        ParserInit (&parser);

        struct Function *fn = ParseFunction ();

        ParserFree (&parser);

        return fn;
}
