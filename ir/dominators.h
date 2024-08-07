#pragma once
#include "dfa.h"
#include "map.h"

void DominatorMeet (struct BitMap *accum, struct BitMap *item);
void DominatorTransfer (struct BitMap *in, void *basic_block);
HashTable ComputeDominanceFrontier (struct Function *function);
struct DFAConfiguration DominatorDFAConfiguration (struct Function *function);
HashTable ComputePostDominanceFrontier (struct Function *function, struct HashTable *dominator_tree);
struct HashTable ComputePostDominatorTree (struct Function *function, struct DFAConfiguration *result);
struct DFAConfiguration PostDominatorDFAConfiguration (struct Function *function);
HashTable ComputeTranspose (struct Function *function, HashTable *graph);