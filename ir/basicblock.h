#pragma once

#include "array.h"
#include "instruction.h"
#include <stdlib.h>

struct BasicBlock {
        size_t block_no;

        struct Array values;

        struct Function *parent;

        struct Array preds;
        struct BasicBlock *left;
        struct BasicBlock *right;
};

void BasicBlock_init (struct BasicBlock *basic_block);
void BasicBlock_set_left_child (struct BasicBlock *basic_block, struct BasicBlock *left_child);
void BasicBlock_set_right_child (struct BasicBlock *basic_block, struct BasicBlock *right_child);
struct BasicBlock *BasicBlock_preds_iter (struct BasicBlock *basic_block, size_t *iter_count);
void BasicBlock_add_Instruction (struct BasicBlock *basic_block, struct Instruction *instruction);
size_t BasicBlock_get_Instruction_count (struct BasicBlock *basic_block);
struct Instruction *BasicBlock_Instruction_iter (struct BasicBlock *basic_block, size_t *iter_count);