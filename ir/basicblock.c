#include "basicblock.h"
#include "array.h"
#include "global_constants.h"
#include "instruction.h"
#include "mem.h"
#include "utils.h"
#include "value.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
static size_t CURRENT_BASIC_BLOCK_NO = 0;

void BasicBlockInit (struct BasicBlock *basic_block, enum BasicBlockType type)
{
        basic_block->type = type;

        assert (CURRENT_BASIC_BLOCK_NO < MAX_BASIC_BLOCK_COUNT);

        basic_block->block_no = CURRENT_BASIC_BLOCK_NO++;
        basic_block->left = NULL;
        basic_block->right = NULL;
        basic_block->next = NULL;
        Array_init (&basic_block->preds);
        Array_init (&basic_block->values);
}

void BasicBlockFree (struct BasicBlock *basic_block)
{
        Array_free (&basic_block->preds);
        Array_free (&basic_block->values);
}

size_t BasicBlockGetInstructionCount (struct BasicBlock *basic_block)
{
        return Array_length (&basic_block->values);
}

void BasicBlockSetLeftChild (struct BasicBlock *basic_block, struct BasicBlock *left_child)
{
        basic_block->left = left_child;

        Array_push (&left_child->preds, basic_block);
}

void BasicBlockSetRightChild (struct BasicBlock *basic_block, struct BasicBlock *right_child)
{
        basic_block->right = right_child;
        Array_push (&right_child->preds, basic_block);
}

void BasicBlockPrependInstruction (struct BasicBlock *basic_block, struct Instruction *instruction)
{
        Array_insert (&basic_block->values, 0, instruction);
        instruction->parent = basic_block;
}

void BasicBlockAddInstruction (struct BasicBlock *basic_block, struct Instruction *instruction)
{
        Array_push (&basic_block->values, instruction);
        instruction->parent = basic_block;
}

bool BasicBlockRemoveInstruction (struct BasicBlock *basic_block, struct Instruction *inst)
{
        // TODO: remove uses from operands
        size_t n = InstructionGetOperandCount (inst);
        for (size_t i = 0; i < n; i++) {
                struct Value *operand = InstructionGetOperand (inst, i);
                Use_unlink (AS_VALUE (inst), operand, i);
        }

        for (size_t i = 0; i < Array_length (&basic_block->values); i++) {
                struct Instruction *curr = Array_get_index (&basic_block->values, i);

                if (curr == inst) {
                        Array_delete (&basic_block->values, i);
                        return true;
                }
        }

        return false;
}

struct Instruction *BasicBlockLastInstruction (struct BasicBlock *basic_block)
{
        return Array_get_index (&basic_block->values, Array_length (&basic_block->values) - 1);
}

struct BasicBlock *BasicBlockPredsIter (struct BasicBlock *basic_block, size_t *iter_count)
{
        return Array_iter (&basic_block->preds, iter_count);
}

struct BasicBlock *BasicBlockSuccessorsIter (struct BasicBlock *basic_block, size_t *iter_count)
{
        if (*iter_count == 0) {
                (*iter_count)++;

                if (basic_block->left)
                        return basic_block->left;
        }

        if (*iter_count == 1) {
                (*iter_count)++;

                if (basic_block->right)
                        return basic_block->right;
        }

        *iter_count = 0;

        return NULL;
}

size_t BasicBlockSuccessorCount (struct BasicBlock *basic_block)
{
        size_t count = 0;

        if (basic_block->left)
                count++;

        if (basic_block->right)
                count++;

        return count;
}

struct Instruction *BasicBlockInstructionIter (struct BasicBlock *basic_block, size_t *iter_count)
{
        if (Array_length (&basic_block->values) == *iter_count) {
                *iter_count = 0;
                return NULL;
        }

        struct Instruction *instruction = Array_get_index (&basic_block->values, *iter_count);

        (*iter_count)++;

        return instruction;
}

struct Instruction *BasicBlockInstructionReverseIter (struct BasicBlock *basic_block, size_t *iter_count)
{
        size_t n = Array_length (&basic_block->values);

        if (*iter_count == n) {
                *iter_count = 0;
                return NULL;
        }

        size_t index = n - *iter_count - 1;

        (*iter_count)++;

        return Array_get_index (&basic_block->values, index);
}

struct BasicBlock *BasicBlockCreate (enum BasicBlockType type)
{
        struct BasicBlock *basic_block = ir_malloc (sizeof (struct BasicBlock));

        BasicBlockInit (basic_block, type);

        return basic_block;
}
