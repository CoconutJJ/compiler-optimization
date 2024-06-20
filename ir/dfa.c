#include "dfa.h"
#include "array.h"
#include "basicblock.h"
#include "function.h"
#include "global_constants.h"
#include "instruction.h"
#include "map.h"
#include "mem.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
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

struct Array reverse_postorder_iter (struct BasicBlock *entry)
{
        struct Array basic_block_order, stack;
        Array_init (&basic_block_order);
        Array_init (&stack);

        uint64_t visited[MAX_BASIC_BLOCK_COUNT / 64 + 1] = { 0 };
        Array_push (&stack, entry);
        UINT64_BITMAP_SET_BIT (visited, entry->block_no);

        while (Array_length (&stack) > 0) {
                struct BasicBlock *curr = Array_top (&stack);

                if (curr->left && !UINT64_BITMAP_BIT_IS_SET (visited, curr->left->block_no)) {
                        Array_push (&stack, curr->left);
                        UINT64_BITMAP_SET_BIT (visited, curr->left->block_no);
                        continue;
                }
                if (curr->right && !UINT64_BITMAP_BIT_IS_SET (visited, curr->right->block_no)) {
                        Array_push (&stack, curr->right);
                        UINT64_BITMAP_SET_BIT (visited, curr->right->block_no);
                        continue;
                }

                Array_push (&basic_block_order, curr);
                Array_pop (&stack);
        }

        Array_reverse (&basic_block_order);
        Array_free (&stack);

        return basic_block_order;
}



struct DFAResult run_Forward_DFA (struct DFAConfiguration *config, struct Function *function)
{
        struct Array traversal_order = reverse_postorder_iter (function->entry_basic_block);
        struct DFAResult analysis_result;

        hash_table_init (&analysis_result.in_sets);
        hash_table_init (&analysis_result.out_sets);

        analysis_result.in_sets = config->in_set_inits;
        analysis_result.out_sets = config->out_set_inits;

        bool has_changes;

        do {
                has_changes = false;
                for (size_t i = 0, n = Array_length (&traversal_order); i < n; i++) {
                        struct BasicBlock *curr_basic_block = Array_get_index (&traversal_order, i);

                        if (BASICBLOCK_IS_ENTRY (curr_basic_block))
                                continue;

                        size_t iter_count = 0;

                        struct DFABitMap *curr_in_set = DFABitMap_create (MAX_BASIC_BLOCK_COUNT);
                        DFABitMap_copy (&config->top, curr_in_set);

                        struct BasicBlock *pred = NULL;

                        while ((pred = BasicBlock_preds_iter (curr_basic_block, &iter_count)) != NULL) {
                                struct DFABitMap *pred_out_set =
                                        hash_table_search (&analysis_result.out_sets, pred->block_no);
                                config->meet (curr_in_set, pred_out_set);
                        }

                        iter_count = 0;

                        struct DFABitMap *curr_out_set = DFABitMap_create (MAX_BASIC_BLOCK_COUNT);
                        DFABitMap_copy (curr_in_set, curr_out_set);

                        switch (config->domain_value_type) {
                        case DOMAIN_BASIC_BLOCK: config->transfer (curr_out_set, curr_basic_block); break;
                        case DOMAIN_INSTRUCTION: {
                                struct Instruction *instruction;
                                while ((instruction = BasicBlock_Instruction_iter (curr_basic_block, &iter_count)) !=
                                       NULL)
                                        config->transfer (curr_out_set, instruction);
                        }
                        }

                        struct DFABitMap *old_in_set =
                                hash_table_find_and_delete (&analysis_result.in_sets, curr_basic_block->block_no);
                        struct DFABitMap *old_out_set =
                                hash_table_find_and_delete (&analysis_result.out_sets, curr_basic_block->block_no);

                        if (!DFABitMap_compare (old_in_set, curr_in_set) ||
                            !DFABitMap_compare (old_out_set, curr_out_set))
                                has_changes = true;

                        // free the old in and out set
                        DFABitMap_free (old_in_set);
                        DFABitMap_free (old_out_set);

                        hash_table_insert (&analysis_result.in_sets, curr_basic_block->block_no, curr_in_set);
                        hash_table_insert (&analysis_result.out_sets, curr_basic_block->block_no, curr_out_set);
                }
        } while (has_changes);

        Array_free (&traversal_order);

        return analysis_result;
}
