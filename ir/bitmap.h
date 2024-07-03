#pragma once
#include "function.h"
#include <stdbool.h>
#include <stdlib.h>

#define UINT64_BITMAP_SET_BIT(map, bit_no)    map[(bit_no) / 64] |= (1ULL << ((bit_no) % 64))
#define UINT64_BITMAP_UNSET_BIT(map, bit_no)  map[(bit_no) / 64] &= ~(1ULL << ((bit_no) % 64))
#define UINT64_BITMAP_BIT_IS_SET(map, bit_no) ((map[(bit_no) / 64] & (1ULL << ((bit_no) % 64))) > 0)

struct BitMap {
        uint64_t *map;
        size_t size;
};

void BitMapInit (struct BitMap *map, size_t num_bits);
struct BitMap *BitMapCreate (size_t num_bits);
struct BitMap *BitMapComplement (struct BitMap *a, struct BitMap *dest);
struct BitMap *BitMapIntersect (struct BitMap *a, struct BitMap *b, struct BitMap *dest);
struct BitMap *BitMapUnion (struct BitMap *a, struct BitMap *b, struct BitMap *dest);
struct BitMap *BitMapInPlaceComplement (struct BitMap *a);
struct BitMap *BitMapInPlaceIntersect (struct BitMap *dest, struct BitMap *a);
struct BitMap *BitMapInPlaceUnion (struct BitMap *dest, struct BitMap *a);
struct BasicBlock *BitMap_BasicBlock_iter (struct Function *function, struct BitMap *map, size_t *iter_count);
bool BitMapIsSet (struct BitMap *a, size_t bit_no);
int64_t BitMapIter (struct BitMap *a, size_t *iter_count);
size_t BitMapCount (struct BitMap *a);
void BitMapFill (struct BitMap *a);
void BitMapEmpty (struct BitMap *a);
bool BitMapCompare (struct BitMap *a, struct BitMap *b);
void BitMapUnsetBit (struct BitMap *a, size_t bit_no);
void BitMapSetBit (struct BitMap *a, size_t bit_no);
void BitMapCopy (struct BitMap *src, struct BitMap *dest);
void BitMapFree (struct BitMap *map);
void BitMapDestroy (struct BitMap *map);