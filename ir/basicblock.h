#pragma once

#include "array.h"
#include "dfa.h"
#include "instruction.h"
#include <stdlib.h>

#define BASICBLOCK_IS_ENTRY(bb_ptr) ((bb_ptr)->type == BASICBLOCK_ENTRY)
#define BASICBLOCK_IS_EXIT(bb_ptr)  ((bb_ptr)->type == BASICBLOCK_EXIT)

enum BasicBlockType { BASICBLOCK_ENTRY, BASICBLOCK_NORMAL, BASICBLOCK_EXIT };

struct BasicBlock {
        size_t block_no;
        enum BasicBlockType type;
        struct Array values;

        struct Function *parent;

        struct Array preds;
        struct BasicBlock *left;
        struct BasicBlock *right;
};

void BasicBlock_init (struct BasicBlock *basic_block, enum BasicBlockType type);
void BasicBlock_set_left_child (struct BasicBlock *basic_block, struct BasicBlock *left_child);
void BasicBlock_set_right_child (struct BasicBlock *basic_block, struct BasicBlock *right_child);
struct BasicBlock *BasicBlock_preds_iter (struct BasicBlock *basic_block, size_t *iter_count);
void BasicBlock_add_Instruction (struct BasicBlock *basic_block, struct Instruction *instruction);
size_t BasicBlock_get_Instruction_count (struct BasicBlock *basic_block);
struct Instruction *BasicBlock_Instruction_iter (struct BasicBlock *basic_block, size_t *iter_count);
struct BasicBlock *BasicBlock_successors_iter (struct BasicBlock *basic_block, size_t *iter_count);
struct Instruction *BasicBlock_Instruction_ReverseIter (struct BasicBlock *basic_block, size_t *iter_count);
void BasicBlock_free (struct BasicBlock *basic_block);