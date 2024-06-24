#include "bitmap.h"
#include "basicblock.h"
#include "map.h"
#include "mem.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void BitMap_init (struct BitMap *map, size_t num_bits)
{
        size_t map_size = num_bits / 64;

        if (num_bits % 64 > 0)
                map_size++;

        map->map = ir_calloc (map_size, sizeof (uint64_t));
        map->size = map_size;
}

struct BitMap *BitMap_create (size_t num_bits)
{
        struct BitMap *map = ir_malloc (sizeof (struct BitMap));
        BitMap_init (map, num_bits);

        return map;
}

void BitMap_free (struct BitMap *map)
{
        ir_free (map->map);
}

void BitMap_copy (struct BitMap *src, struct BitMap *dest)
{
        if (dest->map)
                dest->map = ir_realloc (dest->map, src->size * sizeof (uint64_t));
        else
                dest->map = ir_calloc (src->size, sizeof (uint64_t));

        dest->size = src->size;

        memcpy (dest->map, src->map, src->size * sizeof (uint64_t));
}

bool BitMap_BitIsSet (struct BitMap *a, size_t bit_no)
{
        return UINT64_BITMAP_BIT_IS_SET (a->map, bit_no);
}

void BitMap_SetBit (struct BitMap *a, size_t bit_no)
{
        UINT64_BITMAP_SET_BIT (a->map, bit_no);
}

void BitMap_UnsetBit (struct BitMap *a, size_t bit_no)
{
        UINT64_BITMAP_UNSET_BIT (a->map, bit_no);
}

void BitMap_empty (struct BitMap *a)
{
        memset (a->map, 0, sizeof (uint64_t) * a->size);
}

void BitMap_fill (struct BitMap *a)
{
        memset (a->map, 0xFF, sizeof (uint64_t) * a->size);
}

int64_t BitMap_iter (struct BitMap *a, size_t *iter_count)
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

        *iter_count = 0;
        return -1LL;
}

struct BitMap *BitMap_Complement (struct BitMap *a, struct BitMap *dest)
{
        assert (a->size == dest->size);

        for (size_t i = 0; i < a->size; i++) {
                dest->map[i] = ~(a->map[i]);
        }

        return dest;
}

struct BitMap *BitMap_Intersect (struct BitMap *a, struct BitMap *b, struct BitMap *dest)
{
        assert (a->size == b->size);
        assert (a->size == dest->size);
        for (size_t i = 0; i < a->size; i++) {
                dest->map[i] = a->map[i] & b->map[i];
        }
        return dest;
}

struct BitMap *BitMap_Union (struct BitMap *a, struct BitMap *b, struct BitMap *dest)
{
        assert (a->size == b->size);
        assert (a->size == dest->size);
        for (size_t i = 0; i < a->size; i++) {
                dest->map[i] = a->map[i] | b->map[i];
        }
        return dest;
}

struct BitMap *BitMap_inplace_Complement (struct BitMap *a)
{
        return BitMap_Complement (a, a);
}

struct BitMap *BitMap_inplace_Intersect (struct BitMap *dest, struct BitMap *a)
{
        return BitMap_Intersect (dest, a, dest);
}

struct BitMap *BitMap_inplace_Union (struct BitMap *dest, struct BitMap *a)
{
        return BitMap_Union (dest, a, dest);
}

struct BitMap *BitMap_setbit (struct BitMap *map, size_t bit_no)
{
        UINT64_BITMAP_SET_BIT (map->map, bit_no);

        return map;
}

bool BitMap_compare (struct BitMap *a, struct BitMap *b)
{
        assert (a->size == b->size);

        return memcmp (a->map, b->map, sizeof (uint64_t) * a->size) == 0;
}
