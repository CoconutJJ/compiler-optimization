#pragma once

#include "basicblock.h"
#include "bitmap.h"
#include "function.h"
#include "instruction.h"
#include "map.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#define MAX(a, b) ((a < b) ? (b) : (a))

struct DFAConfiguration;

enum DomainValueType { DOMAIN_INSTRUCTION, DOMAIN_BASIC_BLOCK };

enum DFAFlowDirection { DFA_FORWARD, DFA_BACKWARD };

typedef void (*MeetOp) (struct BitMap *accum, struct BitMap *item);
typedef void (*TransferFunction) (struct BitMap *in, void *domain_value);
typedef struct BitMap *(*BasicBlockDirectionalIter) (struct DFAConfiguration *config,
                                                     struct BasicBlock *curr,
                                                     size_t *iter_count);
typedef struct Instruction *(*InstructionIter) (struct BasicBlock *curr_basic_block, size_t *iter_count);

struct DFAConfiguration {
        enum DomainValueType domain_value_type;
        struct BitMap top;
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

struct Array postorder (struct BasicBlock *entry);
struct Array reverse_postorder (struct BasicBlock *entry);
struct DFAResult run_DFA (struct DFAConfiguration *config, struct Function *function);
struct BitMap *BitMap_BasicBlock_pred_iter (struct DFAConfiguration *config,
                                               struct BasicBlock *curr_basic_block,
                                               size_t *iter_count);

struct BitMap *BitMap_BasicBlock_successor_iter (struct DFAConfiguration *config,
                                                    struct BasicBlock *curr_basic_block,
                                                    size_t *iter_count);