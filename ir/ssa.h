#pragma once

#include "map.h"

struct SSAFrame {

    HashTable variable_map;
    struct SSAFrame *next;
    struct SSAFrame *prev;

};