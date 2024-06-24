#include "basicblock.h"
#include "array.h"
#include "global_constants.h"
#include "instruction.h"
#include <assert.h>
#include <stdlib.h>
static size_t CURRENT_BASIC_BLOCK_NO = 0;

void BasicBlock_init (struct BasicBlock *basic_block, enum BasicBlockType type)
{
        basic_block->type = type;

        assert (CURRENT_BASIC_BLOCK_NO < MAX_BASIC_BLOCK_COUNT);

        basic_block->block_no = CURRENT_BASIC_BLOCK_NO++;
        basic_block->left = NULL;
        basic_block->right = NULL;

        Array_init (&basic_block->preds);
        Array_init (&basic_block->values);
}

void BasicBlock_free (struct BasicBlock *basic_block)
{
        Array_free (&basic_block->preds);
        Array_free (&basic_block->values);
}

size_t BasicBlock_get_Instruction_count (struct BasicBlock *basic_block)
{
        return Array_length (&basic_block->values);
}

void BasicBlock_set_left_child (struct BasicBlock *basic_block, struct BasicBlock *left_child)
{
        basic_block->left = left_child;

        Array_push (&left_child->preds, basic_block);
}

void BasicBlock_set_right_child (struct BasicBlock *basic_block, struct BasicBlock *right_child)
{
        basic_block->right = right_child;
        Array_push (&right_child->preds, basic_block);
}

void BasicBlock_add_Instruction (struct BasicBlock *basic_block, struct Instruction *instruction)
{
        Array_push (&basic_block->values, instruction);
        instruction->parent = basic_block;
}

struct BasicBlock *BasicBlock_preds_iter (struct BasicBlock *basic_block, size_t *iter_count)
{
        return Array_iter (&basic_block->preds, iter_count);
}

struct BasicBlock *BasicBlock_successors_iter (struct BasicBlock *basic_block, size_t *iter_count)
{
        if (*iter_count == 0) {
                (*iter_count)++;
                return basic_block->left;
        } else if (*iter_count == 1) {
                (*iter_count)++;
                return basic_block->right;
        } else {
                *iter_count = 0;
                return NULL;
        }
}

struct Instruction *BasicBlock_Instruction_iter (struct BasicBlock *basic_block, size_t *iter_count)
{
        if (Array_length (&basic_block->values) == *iter_count) {
                *iter_count = 0;
                return NULL;
        }

        struct Instruction *instruction = Array_get_index (&basic_block->values, *iter_count);

        (*iter_count)++;

        return instruction;
}

struct Instruction *BasicBlock_Instruction_ReverseIter (struct BasicBlock *basic_block, size_t *iter_count)
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