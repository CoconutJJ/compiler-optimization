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
        struct Array preds;

        struct Function *parent;

        struct BasicBlock *left;
        struct BasicBlock *right;
        struct BasicBlock *next;
};

struct BasicBlock *BasicBlockCreate (enum BasicBlockType type);
void BasicBlockInit (struct BasicBlock *basic_block, enum BasicBlockType type);
void BasicBlockSetLeftChild (struct BasicBlock *basic_block, struct BasicBlock *left_child);
void BasicBlockSetRightChild (struct BasicBlock *basic_block, struct BasicBlock *right_child);
struct BasicBlock *BasicBlockPredsIter (struct BasicBlock *basic_block, size_t *iter_count);
void BasicBlockAddInstruction (struct BasicBlock *basic_block, struct Instruction *instruction);
size_t BasicBlockGetInstructionCount (struct BasicBlock *basic_block);
struct Instruction *BasicBlockLastInstruction (struct BasicBlock *basic_block);
struct Instruction *BasicBlockInstructionIter (struct BasicBlock *basic_block, size_t *iter_count);
struct BasicBlock *BasicBlockSuccessorsIter (struct BasicBlock *basic_block, size_t *iter_count);
struct Instruction *BasicBlockInstructionReverseIter (struct BasicBlock *basic_block, size_t *iter_count);
void BasicBlockFree (struct BasicBlock *basic_block);
void BasicBlockPrependInstruction (struct BasicBlock *basic_block, struct Instruction *instruction);
bool BasicBlockRemoveInstruction (struct BasicBlock *basic_block, struct Instruction *inst);