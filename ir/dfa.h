#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
struct DFABitMap {
        uint64_t *map;
        size_t size;
};


void DFABitMap_init (struct DFABitMap *map, size_t num_bits);
struct DFABitMap *DFABitMap_Complement (struct DFABitMap *a, struct DFABitMap *dest);
struct DFABitMap *DFABitMap_Intersect (struct DFABitMap *a, struct DFABitMap *b, struct DFABitMap *dest);
struct DFABitMap *DFABitMap_Union (struct DFABitMap *a, struct DFABitMap *b, struct DFABitMap *dest);
struct DFABitMap *DFABitMap_inplace_Complement (struct DFABitMap *a);
struct DFABitMap *DFABitMap_inplace_Intersect (struct DFABitMap *dest, struct DFABitMap *a);
struct DFABitMap *DFABitMap_inplace_Union (struct DFABitMap *dest, struct DFABitMap *a);