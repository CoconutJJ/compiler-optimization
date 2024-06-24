#pragma once
#include "dfa.h"
#include "map.h"

void DominatorMeet (struct DFABitMap *accum, struct DFABitMap *item);
void DominatorTransfer (struct DFABitMap *in, void *basic_block);
HashTable ComputeDominanceFrontier (struct Function *function);
struct DFAConfiguration DominatorDFAConfiguration (struct Function *function);
