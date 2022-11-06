#ifndef integer_set_h
#define integer_set_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {

    uint64_t *set;
    size_t count;

} IntegerSet;

#endif

