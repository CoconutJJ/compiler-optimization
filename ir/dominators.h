#pragma once
#include "dfa.h"


struct DFAConfiguration DominatorDFAConfiguration ();
void DominatorMeet (struct DFABitMap *accum, struct DFABitMap *item);
void DominatorTransfer (struct DFABitMap *in, void *basic_block);