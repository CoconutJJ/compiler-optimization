#include "ir_gen.h"
#include "array.h"
#include "basicblock.h"
#include "bitmap.h"
#include "constant.h"
#include "function.h"
#include "global_constants.h"
#include "instruction.h"
#include "map.h"
#include "value.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int basic_block_labels[MAX_BASIC_BLOCK_COUNT] = { 0 };
int current_label_no = 1;
struct BitMap visited;

static void EmitOperand (struct Value *value)
{
        if (VALUE_IS_INST (value) || VALUE_IS_ARG (value)) {
                printf ("%%%zu", value->value_no);
        } else {
                printf ("%d", AS_CONST (value)->constant);
        }
}

static void EmitConst (struct Value *value)
{
        assert (VALUE_IS_CONST (value));

        printf ("%d", AS_CONST (value)->constant);
}

static void EmitComma ()
{
        fputs (", ", stdout);
}

static void EmitNewLine ()
{
        fputc ('\n', stdout);
}

static int FindBasicBlockLabel (struct BasicBlock *block)
{
        int label = basic_block_labels[block->block_no];

        if (label == 0) {
                basic_block_labels[block->block_no] = current_label_no++;
        }

        return basic_block_labels[block->block_no];
}

void EmitIR (struct BasicBlock *basic_block)
{
#define EMIT_BINARY_OP(inst_name)                                                                                      \
        {                                                                                                              \
                struct Value *fst = Instruction_get_operand (inst, 0);                                                 \
                struct Value *snd = Instruction_get_operand (inst, 1);                                                 \
                printf (inst_name);                                                                                    \
                EmitOperand (AS_VALUE (inst));                                                                         \
                EmitComma ();                                                                                          \
                EmitOperand (fst);                                                                                     \
                EmitComma ();                                                                                          \
                EmitOperand (snd);                                                                                     \
        }

        struct Instruction *inst;
        size_t iter_count = 0;

        while ((inst = BasicBlock_Instruction_iter (basic_block, &iter_count)) != NULL) {
                printf ("    ");
                switch (inst->op_code) {
                case OPCODE_ADD: {
                        EMIT_BINARY_OP ("add ");
                        break;
                }
                case OPCODE_SUB: {
                        EMIT_BINARY_OP ("sub ");
                        break;
                }
                case OPCODE_MUL: {
                        EMIT_BINARY_OP ("mul ");
                        break;
                }
                case OPCODE_DIV: {
                        EMIT_BINARY_OP ("div ");
                        break;
                }
                case OPCODE_CMP: {
                        EMIT_BINARY_OP ("cmp ")
                        break;
                }
                case OPCODE_JUMP: {
                        printf ("jump %d", FindBasicBlockLabel (basic_block->right));
                        EmitNewLine ();
                        break;
                }
                case OPCODE_JUMPIF: {
                        struct Value *cond = Instruction_get_operand (inst, 1);
                        printf ("jumpif %d", FindBasicBlockLabel (basic_block->right));
                        EmitComma ();
                        EmitOperand (cond);
                        break;
                };
                case OPCODE_PHI: {
                        struct Value *curr;
                        size_t iter_count = 0;
                        printf ("phi ");
                        EmitOperand (AS_VALUE (inst));
                        EmitComma ();
                        while ((curr = Array_iter (&inst->operand_list, &iter_count)) != NULL) {
                                EmitOperand (curr);

                                if (iter_count < Array_length (&inst->operand_list)) {
                                        EmitComma ();
                                }
                        }
                        break;
                };
                case OPCODE_STORE: {
                        struct Value *alloca_inst = Instruction_get_operand (inst, 0);
                        struct Value *store_value = Instruction_get_operand (inst, 1);

                        printf ("store ");
                        EmitOperand (AS_VALUE (alloca_inst));
                        EmitComma ();
                        EmitOperand (AS_VALUE (store_value));

                        break;
                };
                case OPCODE_LOAD: {
                        struct Value *source = Instruction_get_operand (inst, 0);

                        printf ("load ");
                        EmitOperand (AS_VALUE (inst));
                        EmitComma ();
                        EmitOperand (AS_VALUE (source));
                        EmitNewLine ();

                        break;
                };
                case OPCODE_ALLOCA: {
                        struct Value *size = Instruction_get_operand (inst, 0);

                        printf ("alloca ");

                        EmitOperand (AS_VALUE (inst));
                        EmitComma ();
                        EmitConst (size);

                        break;
                }
                case OPCODE_NIL: {
                        printf ("nil");
                        break;
                }
                }

                EmitNewLine ();
        }
}

static void EmitBasicBlock (struct BasicBlock *block)
{
        if (BitMap_BitIsSet (&visited, block->block_no))
                return;

        BitMap_setbit (&visited, block->block_no);

        if (Array_length (&block->preds) > 0) {
                printf ("\n%d:\n", FindBasicBlockLabel (block));
                EmitIR (block);
        }

        if (block->left) {
                EmitBasicBlock (block->left);
        }

        if (block->right) {
                EmitBasicBlock (block->right);
        }
}

void EmitInit ()
{
        BitMap_init (&visited, MAX_BASIC_BLOCK_COUNT);
}

void EmitFunction (struct Function *function)
{
        printf ("fn %s (", function->fn_name);

        struct Argument *arg;
        size_t iter_count = 0;

        while ((arg = Array_iter (&function->arguments, &iter_count)) != NULL) {
                EmitOperand (AS_VALUE (arg));

                if (iter_count < Array_length (&function->arguments))
                        EmitComma ();
        }

        printf (") {\n");
        EmitBasicBlock (function->entry_basic_block);
        printf ("}\n");
}