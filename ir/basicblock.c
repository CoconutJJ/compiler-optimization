#include <stdlib.h>
#include <assert.h>
#include "global_constants.h"
#include "array.h"
#include "basicblock.h"
#include "instruction.h"
static size_t CURRENT_BASIC_BLOCK_NO = 0;


void BasicBlock_init (struct BasicBlock *basic_block, enum BasicBlockType type)
{
        basic_block->type = type;

        assert (CURRENT_BASIC_BLOCK_NO < MAX_BASIC_BLOCK_COUNT);
        basic_block->block_no = CURRENT_BASIC_BLOCK_NO++;
        basic_block->left = NULL;
        basic_block->right = NULL;

        Array_init (&basic_block->preds, sizeof (struct BasicBlock *));
        
        Array_init (&basic_block->values, sizeof (struct Instruction *));
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

struct Instruction *BasicBlock_Instruction_iter (struct BasicBlock *basic_block, size_t *iter_count)
{
        if (Array_length (&basic_block->values) == *iter_count)
                return NULL;

        struct Instruction *instruction = *(struct Instruction **)Array_get_index (&basic_block->values, *iter_count);

        (*iter_count)++;

        return instruction;
}
