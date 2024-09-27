#include "bitmap.h"
#include "mem.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>
void BitMapInit (struct BitMap *map, size_t num_bits)
{
        size_t map_size = num_bits / 64;

        if (num_bits % 64 > 0)
                map_size++;

        map->map = ir_calloc (map_size, sizeof (uint64_t));
        map->size = map_size;
}

struct BitMap *BitMapCreate (size_t num_bits)
{
        struct BitMap *map = ir_malloc (sizeof (struct BitMap));
        BitMapInit (map, num_bits);

        return map;
}

void BitMapFree (struct BitMap *map)
{
        ir_free (map->map);
}

void BitMapDestroy (struct BitMap *map)
{
        BitMapFree (map);
        ir_free (map);
}

void BitMapCopy (struct BitMap *src, struct BitMap *dest)
{
        if (dest->map)
                dest->map = ir_realloc (dest->map, src->size * sizeof (uint64_t));
        else
                dest->map = ir_calloc (src->size, sizeof (uint64_t));

        dest->size = src->size;

        memcpy (dest->map, src->map, src->size * sizeof (uint64_t));
}

bool BitMapIsSet (struct BitMap *a, size_t bit_no)
{
        return UINT64_BITMAP_BIT_IS_SET (a->map, bit_no);
}

void BitMapSetBit (struct BitMap *a, size_t bit_no)
{
        UINT64_BITMAP_SET_BIT (a->map, bit_no);
}

void BitMapUnsetBit (struct BitMap *a, size_t bit_no)
{
        UINT64_BITMAP_UNSET_BIT (a->map, bit_no);
}

void BitMapEmpty (struct BitMap *a)
{
        memset (a->map, 0, sizeof (uint64_t) * a->size);
}

void BitMapFill (struct BitMap *a)
{
        memset (a->map, 0xFF, sizeof (uint64_t) * a->size);
}

int64_t BitMapIter (struct BitMap *a, size_t *iter_count)
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

size_t BitMapCount (struct BitMap *a)
{
        size_t total_set_bits = 0;

        for (size_t i = 0; i < a->size; i++) {
                total_set_bits += __builtin_popcountll (a->map[i]);
        }

        return total_set_bits;
}

struct BitMap *BitMapComplement (struct BitMap *a, struct BitMap *dest)
{
        assert (a->size == dest->size);

        for (size_t i = 0; i < a->size; i++) {
                dest->map[i] = ~(a->map[i]);
        }

        return dest;
}

struct BitMap *BitMapIntersect (struct BitMap *a, struct BitMap *b, struct BitMap *dest)
{
        assert (a->size == b->size);
        assert (a->size == dest->size);
        for (size_t i = 0; i < a->size; i++) {
                dest->map[i] = a->map[i] & b->map[i];
        }
        return dest;
}

struct BitMap *BitMapUnion (struct BitMap *a, struct BitMap *b, struct BitMap *dest)
{
        assert (a->size == b->size);
        assert (a->size == dest->size);
        for (size_t i = 0; i < a->size; i++) {
                dest->map[i] = a->map[i] | b->map[i];
        }
        return dest;
}

struct BitMap *BitMapInPlaceComplement (struct BitMap *a)
{
        return BitMapComplement (a, a);
}

struct BitMap *BitMapInPlaceIntersect (struct BitMap *dest, struct BitMap *a)
{
        return BitMapIntersect (dest, a, dest);
}

struct BitMap *BitMapInPlaceUnion (struct BitMap *dest, struct BitMap *a)
{
        return BitMapUnion (dest, a, dest);
}

bool BitMapCompare (struct BitMap *a, struct BitMap *b)
{
        assert (a->size == b->size);

        return memcmp (a->map, b->map, sizeof (uint64_t) * a->size) == 0;
}
