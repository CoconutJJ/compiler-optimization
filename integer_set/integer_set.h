#ifndef integer_set_h
#define integer_set_h

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
        uint64_t *set;
        int64_t count;

} IntegerSet;
IntegerSet *set_create ();
void set_copy(IntegerSet *dest, IntegerSet *src);
void set_init (IntegerSet *set);
bool set_equal (IntegerSet *a, IntegerSet *b);
IntegerSet *universal_set ();
bool is_universal_set (IntegerSet *set);
IntegerSet *set_add (IntegerSet *set, uint64_t v);
bool set_has (IntegerSet *set, uint64_t v);
void set_destroy (IntegerSet *set);
bool set_iter (IntegerSet *set, int64_t *bit_no);
void set_union (IntegerSet *dest, IntegerSet *src);
void set_intersection (IntegerSet *dest, IntegerSet *src);
void set_subtraction (IntegerSet *dest, IntegerSet *src);
#endif
