#include "dfa.h"
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

struct BasicBlock *reverse_postorder_iter (struct BasicBlock *entry, uint64_t visited_set[])
{
        struct BasicBlock **order = NULL;
        size_t order_count = 0;
        size_t order_size = 0;
        DYNARR_INIT (order, order_count, order_size, sizeof (struct BasicBlock *));

        struct BasicBlock **stack = NULL;
        size_t stack_size = 0;
        size_t stack_count = 0;
        DYNARR_INIT (stack, stack_count, stack_size, sizeof (struct BasicBlock *));

        DYNARR_PUSH (stack, stack_count, stack_size, &entry);

        // while (stack_count > 0) {
        //         struct BasicBlock *node = stack[stack_count / sizeof (struct BasicBlock *) - 1];
        // }
}
