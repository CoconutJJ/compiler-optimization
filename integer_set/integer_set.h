#ifndef integer_set_h
#define integer_set_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {

    uint64_t *set;
    size_t count;

} IntegerSet;
IntegerSet *set_create ();
void set_add (IntegerSet *set, uint64_t v);
bool set_has (IntegerSet *set, uint64_t v);
void set_destroy (IntegerSet *set);
bool set_iter (IntegerSet *set, int64_t *bit_no);
IntegerSet *set_union (IntegerSet *setA, IntegerSet *setB);
IntegerSet *set_intersection (IntegerSet *setA, IntegerSet *setB);
IntegerSet *set_subtraction (IntegerSet *setA, IntegerSet *setB);
#endif

