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

void BitMap_init (struct BitMap *map, size_t num_bits);
struct BitMap *BitMap_create (size_t num_bits);
struct BitMap *BitMap_Complement (struct BitMap *a, struct BitMap *dest);
struct BitMap *BitMap_Intersect (struct BitMap *a, struct BitMap *b, struct BitMap *dest);
struct BitMap *BitMap_Union (struct BitMap *a, struct BitMap *b, struct BitMap *dest);
struct BitMap *BitMap_inplace_Complement (struct BitMap *a);
struct BitMap *BitMap_inplace_Intersect (struct BitMap *dest, struct BitMap *a);
struct BitMap *BitMap_inplace_Union (struct BitMap *dest, struct BitMap *a);
struct BitMap *BitMap_setbit (struct BitMap *map, size_t bit_no);
struct BasicBlock *BitMap_BasicBlock_iter (struct Function *function, struct BitMap *map, size_t *iter_count);
bool BitMap_BitIsSet (struct BitMap *a, size_t bit_no);
int64_t BitMap_iter (struct BitMap *a, size_t *iter_count);
void BitMap_fill (struct BitMap *a);
void BitMap_empty (struct BitMap *a);
bool BitMap_compare (struct BitMap *a, struct BitMap *b);
void BitMap_UnsetBit (struct BitMap *a, size_t bit_no);
void BitMap_SetBit (struct BitMap *a, size_t bit_no);
void BitMap_copy (struct BitMap *src, struct BitMap *dest);
void BitMap_free (struct BitMap *map);