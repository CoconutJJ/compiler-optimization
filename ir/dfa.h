#pragma once

#include "map.h"
#include "instruction.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#define MAX(a, b) ((a < b) ? (b) : (a))

#define UINT64_BITMAP_SET_BIT(map, bit_no)    map[(bit_no + 1) / 64] |= (1 << ((bit_no) % 64))
#define UINT64_BITMAP_UNSET_BIT(map, bit_no)  map[(bit_no + 1) / 64] &= ~(1 << ((bit_no) % 64))
#define UINT64_BITMAP_BIT_IS_SET(map, bit_no) ((map[(bit_no + 1) / 64] & (1 << ((bit_no) % 64))) > 0)


struct DFABitMap {
        uint64_t *map;
        size_t size;
};

typedef void (*MeetOp) (struct DFABitMap *accum, struct DFABitMap *item);
typedef void (*TransferFunction) (struct DFABitMap *in, struct Instruction *instruction);

struct DFAConfiguration {
        MeetOp meet;
        TransferFunction transfer;
        struct DFABitMap top;
        HashTable in_set_inits;
        HashTable out_set_inits;
};

struct DFAResult {
        HashTable in_sets;
        HashTable out_sets;
};

void DFABitMap_init (struct DFABitMap *map, size_t num_bits);
struct DFABitMap *DFABitMap_Complement (struct DFABitMap *a, struct DFABitMap *dest);
struct DFABitMap *DFABitMap_Intersect (struct DFABitMap *a, struct DFABitMap *b, struct DFABitMap *dest);
struct DFABitMap *DFABitMap_Union (struct DFABitMap *a, struct DFABitMap *b, struct DFABitMap *dest);
struct DFABitMap *DFABitMap_inplace_Complement (struct DFABitMap *a);
struct DFABitMap *DFABitMap_inplace_Intersect (struct DFABitMap *dest, struct DFABitMap *a);
struct DFABitMap *DFABitMap_inplace_Union (struct DFABitMap *dest, struct DFABitMap *a);