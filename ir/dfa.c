#include "dfa.h"
#include "array.h"
#include "basicblock.h"
#include "bitmap.h"
#include "function.h"
#include "global_constants.h"
#include "instruction.h"
#include "map.h"
#include "mem.h"
#include "utils.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BitMap *
BitMap_BasicBlock_pred_iter (struct DFAConfiguration *config, struct BasicBlock *curr_basic_block, size_t *iter_count)
{
        struct BasicBlock *pred = BasicBlockPredsIter (curr_basic_block, iter_count);

        if (!pred)
                return NULL;

        return hash_table_search_ptr (&config->out_sets, pred->block_no);
}

struct BasicBlock *BitMap_BasicBlock_iter (struct Function *function, struct BitMap *map, size_t *iter_count)
{
        int64_t bit_no = BitMapIter (map, iter_count);

        if (bit_no == -1LL) {
                *iter_count = 0;
                return NULL;
        }

        struct BasicBlock *block = hash_table_search_ptr (&function->block_number_map, bit_no);

        assert (block != NULL);

        return block;
}

struct BitMap *BitMap_BasicBlock_successor_iter (struct DFAConfiguration *config,
                                                 struct BasicBlock *curr_basic_block,
                                                 size_t *iter_count)
{
        struct BasicBlock *succ = BasicBlockSuccessorsIter (curr_basic_block, iter_count);

        if (!succ)
                return NULL;

        return hash_table_search_ptr (&config->in_sets, succ->block_no);
}

struct Array preorder (struct BasicBlock *entry)
{
        struct Array basic_block_order, stack;
        Array_init (&stack);
        Array_push (&stack, entry);

        struct BitMap visited;
        BitMapInit (&visited, MAX_BASIC_BLOCK_COUNT);
        BitMapSetBit (&visited, entry->block_no);

        Array_init (&basic_block_order);
        while (Array_length (&stack) > 0) {
                struct BasicBlock *curr = Array_pop (&stack);

                Array_push (&basic_block_order, curr);

                // check if left child has been visited
                if (curr->left && !BitMapIsSet (&visited, curr->left->block_no)) {
                        Array_push (&stack, curr->left);
                        BitMapSetBit (&visited, curr->left->block_no);
                        continue;
                }

                // check if right child has been visited
                if (curr->right && !BitMapIsSet (&visited, curr->right->block_no)) {
                        Array_push (&stack, curr->right);
                        BitMapSetBit (&visited, curr->right->block_no);
                        continue;
                }
        }

        Array_free (&stack);

        return basic_block_order;
}

struct Array postorder (struct BasicBlock *entry)
{
        struct Array basic_block_order, stack;

        Array_init (&stack);
        Array_push (&stack, entry);

        struct BitMap visited;
        BitMapInit (&visited, MAX_BASIC_BLOCK_COUNT);
        BitMapSetBit (&visited, entry->block_no);

        Array_init (&basic_block_order);
        while (Array_length (&stack) > 0) {
                struct BasicBlock *curr = Array_top (&stack);

                // check if left child has been visited
                if (curr->left && !BitMapIsSet (&visited, curr->left->block_no)) {
                        Array_push (&stack, curr->left);
                        BitMapSetBit (&visited, curr->left->block_no);
                        continue;
                }

                // check if right child has been visited
                if (curr->right && !BitMapIsSet (&visited, curr->right->block_no)) {
                        Array_push (&stack, curr->right);
                        BitMapSetBit (&visited, curr->right->block_no);
                        continue;
                }

                Array_push (&basic_block_order, curr);
                Array_pop (&stack);
        }

        Array_free (&stack);

        return basic_block_order;
}

struct Array reverse_postorder (struct BasicBlock *entry)
{
        struct Array postorder_traversal = postorder (entry);

        Array_reverse (&postorder_traversal);

        return postorder_traversal;
}

static struct BitMap *compute_Meet_from_Operands (struct DFAConfiguration *config, struct BasicBlock *curr_basic_block)
{
        size_t iter_count = 0;

        struct BitMap *curr_in_set = BitMapCreate (MAX_BASIC_BLOCK_COUNT);
        BitMapCopy (&config->top, curr_in_set);

        BasicBlockDirectionalIter OperandIter;

        // Select the appropriate operand iterator based on the flow direction.
        // For forward analysis, merge the block predecessors. For backward
        // analysis, merge the block successors.
        switch (config->direction) {
        case DFA_FORWARD: OperandIter = BitMap_BasicBlock_pred_iter; break;
        case DFA_BACKWARD: OperandIter = BitMap_BasicBlock_successor_iter; break;
        default: UNREACHABLE ("Invalid dataflow direction!");
        }

        // The meet operator (^) is associative, here we are just doing ((a ^ b)
        // ^ c) ^ ... where a, b, c, ... are the meet operands
        struct BitMap *pred = NULL;
        while ((pred = OperandIter (config, curr_basic_block, &iter_count)) != NULL) {
                config->Meet (curr_in_set, pred);
        }

        return curr_in_set;
}

static struct BitMap *compute_Transfer (struct DFAConfiguration *config, struct BasicBlock *curr_basic_block)
{
        // the transfer function maps IN sets into OUT sets or vica versa
        // depending on flow direction
        struct BitMap *curr_out_set, *curr_in_set;

        struct BitMap *return_set = BitMapCreate (MAX_BASIC_BLOCK_COUNT);

        if (config->direction == DFA_FORWARD) {
                curr_in_set = hash_table_search_ptr (&config->in_sets, curr_basic_block->block_no);
                BitMapCopy (curr_in_set, return_set);
        } else if (config->direction == DFA_BACKWARD) {
                curr_out_set = hash_table_search_ptr (&config->out_sets, curr_basic_block->block_no);
                BitMapCopy (curr_out_set, return_set);
        } else {
                UNREACHABLE ("Invalid dataflow direction!");
        }

        switch (config->domain_value_type) {
        case DOMAIN_BASIC_BLOCK: {
                config->Transfer (return_set, curr_basic_block);
                break;
        }
        case DOMAIN_INSTRUCTION: {
                InstructionIter InstIter = NULL;
                if (config->direction == DFA_FORWARD)
                        InstIter = BasicBlockInstructionIter;
                else if (config->direction == DFA_BACKWARD)
                        InstIter = BasicBlockInstructionReverseIter;
                else
                        UNREACHABLE ("Invalid dataflow direction!");

                size_t iter_count = 0;
                struct Instruction *instruction;
                while ((instruction = InstIter (curr_basic_block, &iter_count)) != NULL)
                        config->Transfer (return_set, instruction);
        }
        }

        return return_set;
}

void RunDFA (struct DFAConfiguration *config, struct Function *function)
{
        struct Array traversal_order = reverse_postorder (function->entry_block);

        bool has_changes;

        do {
                has_changes = false;
                for (size_t i = 0, n = Array_length (&traversal_order); i < n; i++) {
                        struct BasicBlock *curr_basic_block = Array_get_index (&traversal_order, i);

                        // skip entry and exit blocks, these two block will be
                        // initialized with boundary values,
                        if (BASICBLOCK_IS_ENTRY (curr_basic_block))
                                continue;

                        if (BASICBLOCK_IS_EXIT (curr_basic_block))
                                continue;

                        struct BitMap *curr_in_set, *curr_out_set, *old_in_set, *old_out_set;

                        if (config->direction == DFA_FORWARD) {
                                curr_in_set = compute_Meet_from_Operands (config, curr_basic_block);

                                old_in_set = hash_table_find_and_delete (&config->in_sets, curr_basic_block->block_no);

                                hash_table_insert (&config->in_sets, curr_basic_block->block_no, curr_in_set);

                                if (!BitMapCompare (old_in_set, curr_in_set))
                                        has_changes = true;

                                curr_out_set = compute_Transfer (config, curr_basic_block);

                                old_out_set =
                                        hash_table_find_and_delete (&config->out_sets, curr_basic_block->block_no);

                                hash_table_insert (&config->out_sets, curr_basic_block->block_no, curr_out_set);

                                if (!BitMapCompare (old_out_set, curr_out_set))
                                        has_changes = true;

                        } else if (config->direction == DFA_BACKWARD) {
                                curr_out_set = compute_Meet_from_Operands (config, curr_basic_block);
                                old_out_set =
                                        hash_table_find_and_delete (&config->out_sets, curr_basic_block->block_no);

                                hash_table_insert (&config->out_sets, curr_basic_block->block_no, curr_out_set);

                                if (!BitMapCompare (old_out_set, curr_out_set))
                                        has_changes = true;
                                curr_in_set = compute_Transfer (config, curr_basic_block);

                                old_in_set = hash_table_find_and_delete (&config->in_sets, curr_basic_block->block_no);

                                hash_table_insert (&config->in_sets, curr_basic_block->block_no, curr_in_set);

                                if (!BitMapCompare (old_in_set, curr_in_set))
                                        has_changes = true;
                        } else {
                                UNREACHABLE ("Invalid dataflow direction!");
                        }

                        // free the old in and out set
                        BitMapDestroy (old_in_set);
                        BitMapDestroy (old_out_set);
                }
        } while (has_changes);

        Array_free (&traversal_order);
}
