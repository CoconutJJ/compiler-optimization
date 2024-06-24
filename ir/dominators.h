#pragma once
#include "dfa.h"
#include "map.h"

void DominatorMeet (struct BitMap *accum, struct BitMap *item);
void DominatorTransfer (struct BitMap *in, void *basic_block);
HashTable ComputeDominanceFrontier (struct Function *function);
struct DFAConfiguration DominatorDFAConfiguration (struct Function *function);
