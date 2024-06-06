#include "dfa.h"
#include "array.h"
#include "threeaddr.h"
#include <stdint.h>
#include <stdlib.h>
#define MAX(a, b) ((a < b) ? (b) : (a))

#define UINT64_BITMAP_SET_BIT(map, bit_no)    map[(bit_no + 1) / 64] |= (1 << ((bit_no) % 64))
#define UINT64_BITMAP_UNSET_BIT(map, bit_no)  map[(bit_no + 1) / 64] &= ~(1 << ((bit_no) % 64))
#define UINT64_BITMAP_BIT_IS_SET(map, bit_no) ((map[(bit_no + 1) / 64] & (1 << ((bit_no) % 64))) > 0)

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
