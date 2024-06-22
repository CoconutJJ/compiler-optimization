#include "dfa.h"
#include "array.h"
#include "basicblock.h"
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

void DFABitMap_init (struct DFABitMap *map, size_t num_bits)
{
        size_t map_size = num_bits / 64;

        if (num_bits % 64 > 0)
                map_size++;

        map->map = ir_calloc (map_size, sizeof (uint64_t));
        map->size = map_size;
        if (!map->map) {
                perror ("calloc");
                exit (EXIT_FAILURE);
        }
}

struct DFABitMap *DFABitMap_create (size_t num_bits)
{
        struct DFABitMap *map = ir_malloc (sizeof (struct DFABitMap));
        DFABitMap_init (map, num_bits);

        return map;
}

void DFABitMap_free (struct DFABitMap *map)
{
        ir_free (map->map);
}

void DFABitMap_copy (struct DFABitMap *src, struct DFABitMap *dest)
{
        if (dest->map)
                dest->map = ir_realloc (dest->map, src->size * sizeof (uint64_t));
        else
                dest->map = ir_calloc (src->size, sizeof (uint64_t));

        dest->size = src->size;

        memcpy (dest->map, src->map, src->size * sizeof (uint64_t));
}

bool DFABitMap_BitIsSet (struct DFABitMap *a, size_t bit_no)
{
        return UINT64_BITMAP_BIT_IS_SET (a->map, bit_no);
}

void DFABitMap_SetBit (struct DFABitMap *a, size_t bit_no)
{
        UINT64_BITMAP_SET_BIT (a->map, bit_no);
}

void DFABitMap_UnsetBit (struct DFABitMap *a, size_t bit_no)
{
        UINT64_BITMAP_UNSET_BIT (a->map, bit_no);
}

void DFABitMap_empty (struct DFABitMap *a)
{
        memset (a->map, 0, sizeof (uint64_t) * a->size);
}

void DFABitMap_fill (struct DFABitMap *a)
{
        memset (a->map, 0xFF, sizeof (uint64_t) * a->size);
}

int64_t DFABitMap_iter (struct DFABitMap *a, size_t *iter_count)
{
        while (*iter_count < a->size * 64) {
                if (*iter_count % 64 == 0) {
                        size_t index = *iter_count / 64;

                        if (a->map[index] == 0ULL) {
                                *iter_count += 64;
                                continue;
                        }
                }

                if (UINT64_BITMAP_BIT_IS_SET (a->map, *iter_count)) {
                        return (*iter_count)++;
                }

                (*iter_count)++;
        }

        return -1LL;
}

struct BasicBlock *DFABitMap_BasicBlock_iter (struct Function *function, struct DFABitMap *map, size_t *iter_count)
{
        int64_t bit_no = DFABitMap_iter (map, iter_count);

        if (bit_no == -1LL) {
                return NULL;
        }

        struct BasicBlock *block = hash_table_search (&function->block_number_map, bit_no);

        assert (block != NULL);

        return block;
}

struct DFABitMap *DFABitMap_Complement (struct DFABitMap *a, struct DFABitMap *dest)
{
        assert (a->size == dest->size);

        for (size_t i = 0; i < a->size; i++) {
                dest->map[i] = ~(a->map[i]);
        }

        return dest;
}

struct DFABitMap *DFABitMap_Intersect (struct DFABitMap *a, struct DFABitMap *b, struct DFABitMap *dest)
{
        assert (a->size == b->size);
        assert (a->size == dest->size);
        for (size_t i = 0; i < a->size; i++) {
                dest->map[i] = a->map[i] & b->map[i];
        }
        return dest;
}

struct DFABitMap *DFABitMap_Union (struct DFABitMap *a, struct DFABitMap *b, struct DFABitMap *dest)
{
        assert (a->size == b->size);
        assert (a->size == dest->size);
        for (size_t i = 0; i < a->size; i++) {
                dest->map[i] = a->map[i] | b->map[i];
        }
        return dest;
}

struct DFABitMap *DFABitMap_inplace_Complement (struct DFABitMap *a)
{
        return DFABitMap_Complement (a, a);
}

struct DFABitMap *DFABitMap_inplace_Intersect (struct DFABitMap *dest, struct DFABitMap *a)
{
        return DFABitMap_Intersect (dest, a, dest);
}

struct DFABitMap *DFABitMap_inplace_Union (struct DFABitMap *dest, struct DFABitMap *a)
{
        return DFABitMap_Union (dest, a, dest);
}

struct DFABitMap *DFABitMap_setbit (struct DFABitMap *map, size_t bit_no)
{
        UINT64_BITMAP_SET_BIT (map->map, bit_no);

        return map;
}

bool DFABitMap_compare (struct DFABitMap *a, struct DFABitMap *b)
{
        assert (a->size == b->size);

        return memcmp (a->map, b->map, sizeof (uint64_t) * a->size) == 0;
}

struct Array postorder (struct BasicBlock *entry)
{
        struct Array basic_block_order, stack;
        Array_init (&basic_block_order);
        Array_init (&stack);

        uint64_t visited[MAX_BASIC_BLOCK_COUNT / 64 + 1] = { 0 };
        Array_push (&stack, entry);
        UINT64_BITMAP_SET_BIT (visited, entry->block_no);

        while (Array_length (&stack) > 0) {
                struct BasicBlock *curr = Array_top (&stack);

                // check if left child has been visited
                if (curr->left && !UINT64_BITMAP_BIT_IS_SET (visited, curr->left->block_no)) {
                        Array_push (&stack, curr->left);
                        UINT64_BITMAP_SET_BIT (visited, curr->left->block_no);
                        continue;
                }

                // check if right child has been visited
                if (curr->right && !UINT64_BITMAP_BIT_IS_SET (visited, curr->right->block_no)) {
                        Array_push (&stack, curr->right);
                        UINT64_BITMAP_SET_BIT (visited, curr->right->block_no);
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

struct DFABitMap *DFABitMap_BasicBlock_pred_iter (struct DFAConfiguration *config,
                                                  struct BasicBlock *curr_basic_block,
                                                  size_t *iter_count)
{
        struct BasicBlock *pred = BasicBlock_preds_iter (curr_basic_block, iter_count);

        if (!pred)
                return NULL;

        return hash_table_search (&config->out_set_inits, pred->block_no);
}

struct DFABitMap *DFABitMap_BasicBlock_successor_iter (struct DFAConfiguration *config,
                                                       struct BasicBlock *curr_basic_block,
                                                       size_t *iter_count)
{
        struct BasicBlock *succ = BasicBlock_successors_iter (curr_basic_block, iter_count);

        if (!succ)
                return NULL;

        return hash_table_search (&config->in_set_inits, succ->block_no);
}

struct DFABitMap *compute_Meet_from_Operands (struct DFAConfiguration *config, struct BasicBlock *curr_basic_block)
{
        size_t iter_count = 0;

        struct DFABitMap *curr_in_set = DFABitMap_create (MAX_BASIC_BLOCK_COUNT);
        DFABitMap_copy (&config->top, curr_in_set);

        BasicBlockDirectionalIter OperandIter;

        // Get the correct operand iterator based on forward of backward flow direction
        // in forward analysis, the block preds are meet'ed together. Whereas in
        // backwards analysis, it is the successors
        switch (config->direction) {
        case DFA_FORWARD: OperandIter = DFABitMap_BasicBlock_pred_iter; break;
        case DFA_BACKWARD: OperandIter = DFABitMap_BasicBlock_successor_iter; break;
        default: UNREACHABLE ("Invalid dataflow direction!");
        }

        // The meet operator (^) is associative, here we are just doing ((a ^ b) ^ c) ^ ...
        // where a, b, c, ... are the meet operands
        struct DFABitMap *pred = NULL;
        while ((pred = OperandIter (config, curr_basic_block, &iter_count)) != NULL) {
                config->Meet (curr_in_set, pred);
        }

        return curr_in_set;
}

struct DFABitMap *compute_Transfer (struct DFAConfiguration *config, struct BasicBlock *curr_basic_block)
{
        // the transfer function maps IN sets into OUT sets or vica versa depending on flow direction

        struct DFABitMap *curr_out_set, *curr_in_set;

        if (config->direction == DFA_FORWARD) {
                curr_out_set = DFABitMap_create (MAX_BASIC_BLOCK_COUNT);
                curr_in_set = hash_table_search (&config->in_set_inits, curr_basic_block->block_no);
                DFABitMap_copy (curr_in_set, curr_out_set);
        } else if (config->direction == DFA_BACKWARD) {
                curr_in_set = DFABitMap_create (MAX_BASIC_BLOCK_COUNT);
                curr_out_set = hash_table_search (&config->out_set_inits, curr_basic_block->block_no);
                DFABitMap_copy (curr_out_set, curr_in_set);
        } else {
                UNREACHABLE ("Invalid dataflow direction!");
        }

        switch (config->domain_value_type) {
        case DOMAIN_BASIC_BLOCK: {
                if (config->direction == DFA_FORWARD) {
                        config->Transfer (curr_out_set, curr_basic_block);
                } else if (config->direction == DFA_BACKWARD) {
                        config->Transfer (curr_in_set, curr_basic_block);
                } else {
                        UNREACHABLE ("Invalid dataflow direction!");
                }

                break;
        }
        case DOMAIN_INSTRUCTION: {
                InstructionIter InstIter = NULL;
                if (config->direction == DFA_FORWARD)
                        InstIter = BasicBlock_Instruction_iter;
                else if (config->direction == DFA_BACKWARD)
                        InstIter = BasicBlock_Instruction_ReverseIter;
                else
                        UNREACHABLE("Invalid dataflow direction!");
                
                size_t iter_count = 0;
                struct Instruction *instruction;
                while ((instruction = InstIter (curr_basic_block, &iter_count)) != NULL)
                        config->Transfer (curr_out_set, instruction);
        }
        }

        return curr_out_set;
}

struct DFAResult run_DFA (struct DFAConfiguration *config, struct Function *function)
{
        struct Array traversal_order = reverse_postorder (function->entry_basic_block);

        bool has_changes;

        do {
                has_changes = false;
                for (size_t i = 0, n = Array_length (&traversal_order); i < n; i++) {
                        struct BasicBlock *curr_basic_block = Array_get_index (&traversal_order, i);

                        // skip entry and exit blocks, these two block will be initialized with boundary values,
                        if (BASICBLOCK_IS_ENTRY (curr_basic_block))
                                continue;

                        if (BASICBLOCK_IS_EXIT (curr_basic_block))
                                continue;

                        struct DFABitMap *curr_in_set, *curr_out_set, *old_in_set, *old_out_set;

                        if (config->direction == DFA_FORWARD) {
                                curr_in_set = compute_Meet_from_Operands (config, curr_basic_block);

                                old_in_set =
                                        hash_table_find_and_delete (&config->in_set_inits, curr_basic_block->block_no);

                                hash_table_insert (&config->in_set_inits, curr_basic_block->block_no, curr_in_set);

                                if (!DFABitMap_compare (old_in_set, curr_in_set))
                                        has_changes = true;

                                curr_out_set = compute_Transfer (config, curr_basic_block);

                                old_out_set =
                                        hash_table_find_and_delete (&config->out_set_inits, curr_basic_block->block_no);

                                hash_table_insert (&config->out_set_inits, curr_basic_block->block_no, curr_out_set);

                                if (!DFABitMap_compare (old_out_set, curr_out_set))
                                        has_changes = true;

                        } else if (config->direction == DFA_BACKWARD) {
                                curr_out_set = compute_Meet_from_Operands (config, curr_basic_block);
                                old_out_set =
                                        hash_table_find_and_delete (&config->out_set_inits, curr_basic_block->block_no);

                                hash_table_insert (&config->out_set_inits, curr_basic_block->block_no, curr_out_set);

                                if (!DFABitMap_compare (old_out_set, curr_out_set))
                                        has_changes = true;
                                curr_in_set = compute_Transfer (config, curr_basic_block);

                                old_in_set =
                                        hash_table_find_and_delete (&config->in_set_inits, curr_basic_block->block_no);

                                hash_table_insert (&config->in_set_inits, curr_basic_block->block_no, curr_in_set);

                                if (!DFABitMap_compare (old_in_set, curr_in_set))
                                        has_changes = true;
                        } else {
                                UNREACHABLE ("Invalid dataflow direction!");
                        }

                        // free the old in and out set
                        DFABitMap_free (old_in_set);
                        DFABitMap_free (old_out_set);

                        ir_free (old_in_set);
                        ir_free (old_out_set);
                }
        } while (has_changes);

        Array_free (&traversal_order);

        struct DFAResult analysis_result = {
                .in_sets = config->in_set_inits,
                .out_sets = config->out_set_inits,
        };

        return analysis_result;
}
