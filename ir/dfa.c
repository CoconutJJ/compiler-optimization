#include "dfa.h"
#include "array.h"
#include "basicblock.h"
#include "function.h"
#include "global_constants.h"
#include "instruction.h"
#include "map.h"
#include "mem.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void DFABitMap_init (struct DFABitMap *map, size_t num_bits)
{
        size_t map_size = num_bits / 64;

        if (num_bits % 64 > 0)
                map_size++;

        map->map = calloc (map_size, sizeof (uint64_t));
        map->size = map_size;
        if (!map->map) {
                perror ("calloc");
                exit (EXIT_FAILURE);
        }
}

void DFABitMap_free (struct DFABitMap *map)
{
        free (map->map);
}

void DFABitMap_copy (struct DFABitMap *src, struct DFABitMap *dest)
{
        if (dest->map)
                dest->map = ir_realloc (dest->map, src->size);
        else
                dest->map = ir_calloc (src->size, sizeof (uint64_t));

        dest->size = src->size;

        memcpy (dest->map, src->map, src->size * sizeof (uint64_t));
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

struct Array reverse_postorder_iter (struct BasicBlock *entry)
{
        struct Array basic_block_order, stack;
        Array_init (&basic_block_order, sizeof (struct BasicBlock *));
        Array_init (&stack, sizeof (struct BasicBlock *));

        uint64_t visited[MAX_BASIC_BLOCK_COUNT / 64 + 1] = { 0 };
        Array_push (&stack, &entry);
        UINT64_BITMAP_SET_BIT (visited, entry->block_no);

        while (Array_length (&stack) > 0) {
                struct BasicBlock *curr = *(struct BasicBlock **)Array_top (&stack);

                if (curr->left && !UINT64_BITMAP_BIT_IS_SET (visited, curr->left->block_no)) {
                        Array_push (&stack, &curr->left);
                        UINT64_BITMAP_SET_BIT (visited, curr->left->block_no);
                        continue;
                }
                if (curr->right && !UINT64_BITMAP_BIT_IS_SET (visited, curr->right->block_no)) {
                        Array_push (&stack, &curr->right);
                        UINT64_BITMAP_SET_BIT (visited, curr->right->block_no);
                        continue;
                }

                Array_push (&basic_block_order, curr);
                Array_pop (&stack, false);
        }

        Array_reverse (&basic_block_order);
        Array_free (&stack);

        return basic_block_order;
}
struct DFAResult run_Forward_DFA (struct DFAConfiguration *config, struct Function *function)
{
        struct Array traversal_order = reverse_postorder_iter (function->entry_basic_block);
        struct DFAResult analysis_result;

        analysis_result.in_sets = config->in_set_inits;
        analysis_result.out_sets = config->out_set_inits;

        for (size_t i = 0, n = Array_length (&traversal_order); i < n; i++) {
                size_t iter_count = 0;
                struct BasicBlock *curr_basic_block = Array_get_index (&traversal_order, i);

                struct DFABitMap *curr_in_set = NULL;

                struct BasicBlock *pred = NULL;
                while ((pred = BasicBlock_preds_iter (curr_basic_block, &iter_count)) != NULL) {
                        struct DFABitMap *pred_out_set = hash_table_search (&analysis_result.out_sets, pred->block_no);

                        config->meet (curr_in_set, pred_out_set);
                }
                iter_count = 0;

                struct DFABitMap *curr_out_set = ir_malloc (sizeof (struct DFABitMap));
                DFABitMap_init (curr_out_set, MAX_BASIC_BLOCK_COUNT);

                DFABitMap_copy (curr_in_set, curr_out_set);

                switch (config->domain_value_type) {
                case DOMAIN_BASIC_BLOCK: config->transfer (curr_out_set, curr_basic_block); break;
                case DOMAIN_INSTRUCTION: {
                        struct Instruction *instruction;
                        while ((instruction = BasicBlock_Instruction_iter (curr_basic_block, &iter_count)) != NULL)
                                config->transfer (curr_out_set, instruction);
                }
                }

                // free the old in and out set
                DFABitMap_free (hash_table_find_and_delete (&analysis_result.in_sets, curr_basic_block->block_no));
                DFABitMap_free (hash_table_find_and_delete (&analysis_result.out_sets, curr_basic_block->block_no));

                hash_table_insert (&analysis_result.in_sets, curr_basic_block->block_no, curr_in_set);
                hash_table_insert (&analysis_result.out_sets, curr_basic_block->block_no, curr_out_set);
        }

        return analysis_result;
}
