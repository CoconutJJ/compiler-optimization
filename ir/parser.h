#pragma once

#include "basicblock.h"
#include "global_constants.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

void PrintFunction (struct Function *function);
struct Function *ParseIR (char *ir_source);
