#pragma once

#include "function.h"
#include "map.h"

struct SSAFrame {
        HashTable variable_map;
        struct BasicBlock *block;
        struct SSAFrame *next;
        struct SSAFrame *prev;
};

void SSATranslation (struct Function *function);
