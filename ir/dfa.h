#pragma once

#include "basicblock.h"
#include "function.h"
#include "instruction.h"
#include "map.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#define MAX(a, b) ((a < b) ? (b) : (a))

#define UINT64_BITMAP_SET_BIT(map, bit_no)    map[(bit_no) / 64] |= (1ULL << ((bit_no) % 64))
#define UINT64_BITMAP_UNSET_BIT(map, bit_no)  map[(bit_no) / 64] &= ~(1ULL << ((bit_no) % 64))
#define UINT64_BITMAP_BIT_IS_SET(map, bit_no) ((map[(bit_no) / 64] & (1ULL << ((bit_no) % 64))) > 0)

struct DFAConfiguration;

enum DomainValueType { DOMAIN_INSTRUCTION, DOMAIN_BASIC_BLOCK };

enum DFAFlowDirection { DFA_FORWARD, DFA_BACKWARD };

struct DFABitMap {
        uint64_t *map;
        size_t size;
};

typedef void (*MeetOp) (struct DFABitMap *accum, struct DFABitMap *item);
typedef void (*TransferFunction) (struct DFABitMap *in, void *domain_value);
typedef struct DFABitMap *(*BasicBlockDirectionalIter) (struct DFAConfiguration *config,
                                                        struct BasicBlock *curr,
                                                        size_t *iter_count);
typedef struct Instruction *(*InstructionIter) (struct BasicBlock *curr_basic_block, size_t *iter_count);

struct DFAConfiguration {
        enum DomainValueType domain_value_type;
        struct DFABitMap top;
        enum DFAFlowDirection direction;
        MeetOp Meet;
        TransferFunction Transfer;
        struct HashTable in_set_inits;
        HashTable out_set_inits;
};

struct DFAResult {
        HashTable in_sets;
        HashTable out_sets;
};

void DFABitMap_init (struct DFABitMap *map, size_t num_bits);
struct DFABitMap *DFABitMap_create (size_t num_bits);
struct DFABitMap *DFABitMap_Complement (struct DFABitMap *a, struct DFABitMap *dest);
struct DFABitMap *DFABitMap_Intersect (struct DFABitMap *a, struct DFABitMap *b, struct DFABitMap *dest);
struct DFABitMap *DFABitMap_Union (struct DFABitMap *a, struct DFABitMap *b, struct DFABitMap *dest);
struct DFABitMap *DFABitMap_inplace_Complement (struct DFABitMap *a);
struct DFABitMap *DFABitMap_inplace_Intersect (struct DFABitMap *dest, struct DFABitMap *a);
struct DFABitMap *DFABitMap_inplace_Union (struct DFABitMap *dest, struct DFABitMap *a);
struct DFABitMap *DFABitMap_setbit (struct DFABitMap *map, size_t bit_no);
struct BasicBlock *DFABitMap_BasicBlock_iter (struct Function *function, struct DFABitMap *map, size_t *iter_count);
bool DFABitMap_BitIsSet (struct DFABitMap *a, size_t bit_no);
int64_t DFABitMap_iter (struct DFABitMap *a, size_t *iter_count);
struct Array postorder (struct BasicBlock *entry);
struct Array reverse_postorder (struct BasicBlock *entry);
void DFABitMap_fill (struct DFABitMap *a);
void DFABitMap_empty (struct DFABitMap *a);
bool DFABitMap_compare (struct DFABitMap *a, struct DFABitMap *b);
void DFABitMap_UnsetBit (struct DFABitMap *a, size_t bit_no);
void DFABitMap_SetBit (struct DFABitMap *a, size_t bit_no);
void DFABitMap_copy (struct DFABitMap *src, struct DFABitMap *dest);
void DFABitMap_free (struct DFABitMap *map);
struct DFAResult run_DFA (struct DFAConfiguration *config, struct Function *function);
struct DFABitMap *DFABitMap_BasicBlock_pred_iter (struct DFAConfiguration *config,
                                                  struct BasicBlock *curr_basic_block,
                                                  size_t *iter_count);

struct DFABitMap *DFABitMap_BasicBlock_successor_iter (struct DFAConfiguration *config,
                                                       struct BasicBlock *curr_basic_block,
                                                       size_t *iter_count);