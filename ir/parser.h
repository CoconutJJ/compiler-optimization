#pragma once

#include "basicblock.h"
#include "global_constants.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

void display_function (struct Function *function);
struct Function *parse_ir (char *ir_source);
