#pragma once
#include "basicblock.h"
void EmitIR (struct BasicBlock *basic_block);
void EmitBasicBlock (struct BasicBlock *block);
void EmitFunction (struct Function *function);
void EmitInit (void);