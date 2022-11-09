#include "integer_set.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static IntegerSet UNIVERSAL_SET = { .count = -1,
                                    .set = (uint64_t *)0xDEADBEEF };

void set_init (IntegerSet *set)
{
        set->count = 0;
        set->set = NULL;
}

IntegerSet *set_create ()
{
        IntegerSet *set = malloc (sizeof (IntegerSet));

        if (!set) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        set_init (set);
        return set;
}

IntegerSet *universal_set ()
{
        return &UNIVERSAL_SET;
}

bool is_universal_set (IntegerSet *set)
{
        return set == &UNIVERSAL_SET;
}

void set_destroy (IntegerSet *set)
{
        if (is_universal_set (set))
                return;

        free (set->set);
        free (set);
}

void set_empty (IntegerSet *set)
{
        if (is_universal_set (set)) {
                fprintf (stderr, "cannot empty universal set!\n");
                exit (EXIT_FAILURE);
        }

        set_destroy (set);
        set_init (set);
}

void set_copy (IntegerSet *dest, IntegerSet *src)
{
        dest->set = malloc (src->count * sizeof (uint64_t));

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
                set->set = realloc (set->set, (index + 1) * sizeof (uint64_t));

                if (!set->set) {
                        perror ("realloc");
                        exit (EXIT_FAILURE);
                }

                memset (set->set + set->count,
                        0,
                        (index - set->count + 1) * sizeof (uint64_t));

                set->count = index + 1;
        }

        set->set[index] |= (1ULL << bit);

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

IntegerSet *set_union (IntegerSet *dest, IntegerSet *src)
{
        if (is_universal_set (dest))
                return dest;
        if (is_universal_set (src))
                return src;

        if (src->count > dest->count) {
                dest->set =
                        realloc (dest->set, (src->count) * sizeof (uint64_t));

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

        return dest;
}

IntegerSet *set_intersection (IntegerSet *dest, IntegerSet *src)
{
        if (is_universal_set (dest))
                return src;

        if (is_universal_set (src))
                return dest;

        size_t new_count = dest->count > src->count ? dest->count : src->count;

        for (size_t i = 0; i < new_count; i++) {
                if (i < dest->count && i < src->count)
                        dest->set[i] &= src->set[i];
                else
                        dest->set[i] = 0ULL;
        }

        return dest;
}

IntegerSet *set_subtraction (IntegerSet *dest, IntegerSet *src)
{
        if (is_universal_set (src)) {
                set_empty (dest);
                return dest;
        }

        if (is_universal_set (dest)) {
                fprintf (stderr, "cannot subtract from universal set!\n");
                exit (EXIT_FAILURE);
        }

        for (size_t i = 0; i < dest->count; i++)
                if (i < src->count)
                        dest->set[i] -= dest->set[i] & src->set[i];

        return dest;
}