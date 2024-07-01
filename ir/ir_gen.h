#pragma once
#include "basicblock.h"
void EmitIR (struct BasicBlock *basic_block);
void EmitFunction (struct Function *function);
void EmitInit (void);