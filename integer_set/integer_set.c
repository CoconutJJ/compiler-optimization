#include "integer_set.h"
#include "mem.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static IntegerSet UNIVERSAL_SET = { .count = -1LL, .set = NULL };

void set_init (IntegerSet *set)
{
        set->count = 0LL;
        set->set = NULL;
}

IntegerSet *set_create ()
{
        IntegerSet *set = compiler_malloc (sizeof (IntegerSet));

        if (!set) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        set_init (set);
        return set;
}

void make_universal_set (IntegerSet *set)
{
        if (set->set)
                free (set->set);

        *set = UNIVERSAL_SET;
}

IntegerSet *universal_set ()
{
        IntegerSet *set = set_create ();

        make_universal_set (set);

        return set;
}

bool is_universal_set (IntegerSet *set)
{
        return set->count == UNIVERSAL_SET.count && set->set == UNIVERSAL_SET.set;
}

void _set_shallow_free (IntegerSet *set)
{
        if (set->set)
                free (set->set);

        set_init (set);
}

void set_destroy (IntegerSet *set)
{
        _set_shallow_free (set);
        free (set);
}

void set_empty (IntegerSet *set)
{
        _set_shallow_free (set);
}

void set_copy (IntegerSet *dest, IntegerSet *src)
{
        set_empty (dest);

        dest->set = compiler_malloc (src->count * sizeof (uint64_t));

        if (!dest->set) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        memcpy (dest->set, src->set, src->count * sizeof (uint64_t));
        dest->count = src->count;
}

bool set_equal (IntegerSet *a, IntegerSet *b)
{
        size_t min = a->count > b->count ? b->count : a->count;

        size_t max = a->count > b->count ? a->count : b->count;

        for (size_t i = 0; i < min; i++) {
                if (a->set[i] != b->set[i])
                        return false;
        }
        IntegerSet *largest = a->count > b->count ? a : b;

        for (size_t i = min; i < max; i++) {
                if (largest->set[i] != 0)
                        return false;
        }

        return true;
}

IntegerSet *set_add (IntegerSet *set, uint64_t v)
{
        if (is_universal_set (set))
                return set;

        uint64_t index = v / 64ULL;
        int bit = v % 64ULL;

        if (index >= set->count) {
                set->set = compiler_realloc (set->set, (index + 1) * sizeof (uint64_t));

                if (!set->set) {
                        perror ("realloc");
                        exit (EXIT_FAILURE);
                }

                memset (set->set + set->count, 0, (index - set->count + 1) * sizeof (uint64_t));

                set->count = index + 1;
        }

        set->set[index] |= (1ULL << bit);

        return set;
}

IntegerSet *set_delete (IntegerSet *set, uint64_t v)
{
        uint64_t index = v / 64ULL;
        int bit = v % 64ULL;

        if (index >= set->count) {
                return set;
        }
        set->set[index] &= ~(1ULL << bit);

        return set;
}

bool set_has (IntegerSet *set, uint64_t v)
{
        if (is_universal_set (set))
                return true;

        uint64_t index = v / (uint64_t)64;
        uint64_t bit = v % (uint64_t)64;

        if (index >= set->count)
                return false;

        return (set->set[index] & (1ULL << bit)) > 0ULL;
}

bool set_iter (IntegerSet *set, int64_t *bit_no)
{
        (*bit_no)++;

        if (is_universal_set (set))
                return true;

        uint64_t index = *bit_no / 64LL;
        int bit = *bit_no % 64LL;

        if (index >= set->count)
                return false;

        if (set->set[index] == 0) {
                bit = 0;

                while (index < set->count && set->set[index] == 0ULL)
                        index++;

                if (index == set->count && set->set[index] == 0ULL)
                        return false;
        }

        while (!set_has (set, *bit_no))
                (*bit_no)++;

        return true;
}

void set_union (IntegerSet *dest, IntegerSet *src)
{
        if (is_universal_set (dest))
                return;
        if (is_universal_set (src)) {
                make_universal_set (dest);
                return;
        }

        if (src->count > dest->count) {
                dest->set = compiler_realloc (dest->set, (src->count) * sizeof (uint64_t));

                if (!dest->set) {
                        perror ("realloc");
                        exit (EXIT_FAILURE);
                }

                dest->count = src->count;
        }

        for (size_t i = 0; i < src->count; i++) {
                if (i < src->count)
                        dest->set[i] |= src->set[i];
        }

        return;
}

void set_intersection (IntegerSet *dest, IntegerSet *src)
{
        if (is_universal_set (dest)) {
                set_copy (dest, src);
                return;
        }

        if (is_universal_set (src)) {
                return;
        }

        size_t new_count = dest->count > src->count ? dest->count : src->count;

        for (size_t i = 0; i < new_count; i++) {
                if (i < dest->count && i < src->count)
                        dest->set[i] &= src->set[i];
                else
                        dest->set[i] = 0ULL;
        }

        return;
}

void set_subtraction (IntegerSet *dest, IntegerSet *src)
{
        if (is_universal_set (src)) {
                set_empty (dest);
                return;
        }

        if (is_universal_set (dest)) {
                fprintf (stderr, "cannot subtract from universal set!\n");
                exit (EXIT_FAILURE);
        }

        for (size_t i = 0; i < dest->count; i++)
                if (i < src->count)
                        dest->set[i] -= dest->set[i] & src->set[i];

        return;
}
