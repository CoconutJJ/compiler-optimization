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
                struct Value *fst = InstructionGetOperand (inst, 0);                                                   \
                struct Value *snd = InstructionGetOperand (inst, 1);                                                   \
                printf (inst_name);                                                                                    \
                EmitOperand (AS_VALUE (inst));                                                                         \
                EmitComma ();                                                                                          \
                EmitOperand (fst);                                                                                     \
                EmitComma ();                                                                                          \
                EmitOperand (snd);                                                                                     \
        }

        struct Instruction *inst;
        size_t iter_count = 0;

        while ((inst = BasicBlockInstructionIter (basic_block, &iter_count)) != NULL) {
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
                        printf ("jump label %d", FindBasicBlockLabel (basic_block->right));
                        break;
                }
                case OPCODE_JUMPIF: {
                        struct Value *cond = InstructionGetOperand (inst, 1);
                        printf ("jumpif label %d", FindBasicBlockLabel (basic_block->right));
                        EmitComma ();
                        EmitOperand (cond);
                        break;
                };
                case OPCODE_PHI: {
                        struct SSAOperand *curr;
                        size_t iter_count = 0;
                        printf ("phi ");
                        EmitOperand (AS_VALUE (inst));
                        EmitComma ();

                        while ((curr = Array_iter (&inst->operand_list, &iter_count)) != NULL) {
                                putc ('[', stdout);
                                EmitOperand (curr->operand);
                                EmitComma ();
                                printf ("%d]", FindBasicBlockLabel (curr->pred_block));
                                if (iter_count < Array_length (&inst->operand_list)) {
                                        EmitComma ();
                                }
                        }
                        break;
                };
                case OPCODE_STORE: {
                        struct Value *alloca_inst = InstructionGetOperand (inst, 0);
                        struct Value *store_value = InstructionGetOperand (inst, 1);

                        printf ("store ");
                        EmitOperand (AS_VALUE (alloca_inst));
                        EmitComma ();
                        EmitOperand (AS_VALUE (store_value));

                        break;
                };
                case OPCODE_LOAD: {
                        struct Value *source = InstructionGetOperand (inst, 0);

                        printf ("load ");
                        EmitOperand (AS_VALUE (inst));
                        EmitComma ();
                        EmitOperand (AS_VALUE (source));
                        EmitNewLine ();

                        break;
                };
                case OPCODE_ALLOCA: {
                        struct Value *size = InstructionGetOperand (inst, 0);

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
                printf ("    # uses: %lu", Value_Use_count (AS_VALUE (inst)));

                EmitNewLine ();
        }
}

static void EmitBasicBlock (struct BasicBlock *block)
{
        while (block != NULL) {
                if (BitMapIsSet (&visited, block->block_no))
                        return;

                BitMapSetBit (&visited, block->block_no);

                if (Array_length (&block->preds) > 0) {
                        printf ("\n%d:\n", FindBasicBlockLabel (block));
                        EmitIR (block);
                }
                block = block->next;
        }
}

void EmitInit ()
{
        BitMapInit (&visited, MAX_BASIC_BLOCK_COUNT);
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