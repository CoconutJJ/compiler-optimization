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
#define MAX(a, b) ((a < b) ? (b) : (a))

#define UINT64_BITMAP_SET_BIT(map, bit_no)    map[(bit_no + 1) / 64] |= (1 << ((bit_no) % 64))
#define UINT64_BITMAP_UNSET_BIT(map, bit_no)  map[(bit_no + 1) / 64] &= ~(1 << ((bit_no) % 64))
#define UINT64_BITMAP_BIT_IS_SET(map, bit_no) ((map[(bit_no + 1) / 64] & (1 << ((bit_no) % 64))) > 0)

typedef void (*MeetOp) (struct DFABitMap *a, struct DFABitMap *b);

typedef void (*TransferFunction) (struct DFABitMap *in, struct Instruction *instruction);

void DFABitMap_init (struct DFABitMap *map, size_t num_bits)
{
        size_t map_size = num_bits / 64;

        if (num_bits % 64 > 0)
                map_size++;

        map->map = calloc (map_size, sizeof (uint64_t));

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

struct Array reverse_postorder_iter (struct BasicBlock *entry)
{
        struct Array basic_block_order, stack;
        Array_init (&basic_block_order, sizeof (struct BasicBlock *));
        Array_init (&stack, sizeof (struct BasicBlock *));

        Array_push (&stack, entry);

        uint64_t visited[MAX_BASIC_BLOCK_COUNT / 64 + 1];

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
                Array_pop (&stack, false);
        }

        Array_reverse (&basic_block_order);
        Array_free (&stack);

        return basic_block_order;
}
struct HashTable *run_Forward_DFA (MeetOp meet_op, TransferFunction transfer, struct Function *function)
{
        struct Array traversal_order = reverse_postorder_iter (function->entry_basic_block);

        struct Array in_sets, out_sets;

        Array_init (&in_sets, sizeof (struct DFABitMap *));
        Array_init (&out_sets, sizeof (struct DFABitMap *));

        for (size_t i = 0, n = Array_length (&traversal_order); i < n; i++) {
                size_t iter_count = 0;
                struct BasicBlock *curr_basic_block = Array_get_index (&traversal_order, i);

                struct DFABitMap *curr_in_set = NULL;
                struct BasicBlock *pred = NULL;
                while ((pred = BasicBlock_preds_iter (curr_basic_block, &iter_count)) != NULL) {
                        struct DFABitMap *pred_out_set = Array_get_index (&out_sets, pred->block_no);

                        if (!curr_in_set) {
                                curr_in_set = ir_malloc (sizeof (struct DFABitMap));
                                DFABitMap_copy (pred_out_set, curr_in_set);
                                continue;
                        }

                        meet_op (curr_in_set, pred_out_set);
                }

                struct DFABitMap *curr_out_set = ir_malloc (sizeof (struct DFABitMap));

                DFABitMap_copy (curr_in_set, curr_out_set);

                struct Instruction *instruction;
                while ((instruction = BasicBlock_Instruction_iter (curr_basic_block, &iter_count)) != NULL)
                        transfer (curr_out_set, instruction);

                // free the old in and out set
                DFABitMap_free (Array_get_index (&in_sets, curr_basic_block->block_no));
                DFABitMap_free (Array_get_index (&out_sets, curr_basic_block->block_no));

                Array_set_index (&in_sets, curr_basic_block->block_no, &curr_in_set);
                Array_set_index (&out_sets, curr_basic_block->block_no, &curr_out_set);
        }

        return hash_table_create (MAX_BASIC_BLOCK_COUNT);
}
