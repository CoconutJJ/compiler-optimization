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

IntegerSet *set_union (IntegerSet *setA, IntegerSet *setB)
{
        if (is_universal_set (setA))
                return setA;
        if (is_universal_set (setB))
                return setB;

        size_t new_count =
                setA->count > setB->count ? setA->count : setB->count;

        IntegerSet *new_set = set_create ();

        new_set->set = malloc (new_count * sizeof (uint64_t));

        if (!new_set->set) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        new_set->count = new_count;

        for (size_t i = 0; i < new_count; i++) {
                uint64_t un = 0ULL;

                if (i < setA->count)
                        un |= setA->set[i];

                if (i < setB->count)
                        un |= setB->set[i];

                new_set->set[i] = un;
        }
        return new_set;
}

IntegerSet *set_intersection (IntegerSet *setA, IntegerSet *setB)
{
        if (is_universal_set (setA))
                return setB;

        if (is_universal_set (setB))
                return setA;

        size_t new_count =
                setA->count > setB->count ? setA->count : setB->count;

        IntegerSet *new_set = set_create ();

        new_set->set = malloc (new_count * sizeof (uint64_t));

        if (!new_set->set) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        new_set->count = new_count;

        for (size_t i = 0; i < new_count; i++) {
                if (i < setA->count && i < setB->count)
                        new_set->set[i] = setA->set[i] & setB->set[i];
                else
                        new_set->set[i] = 0ULL;
        }

        return new_set;
}

IntegerSet *set_subtraction (IntegerSet *setA, IntegerSet *setB)
{
        IntegerSet *new_set = set_create ();

        if (is_universal_set (setB))
                return new_set;

        if (is_universal_set (setA)) {
                fprintf (stderr, "cannot subtract from universal set!\n");
                exit (EXIT_FAILURE);
        }

        new_set->set = malloc (setA->count * sizeof (uint64_t));

        if (!new_set->set) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        memset (new_set->set, 0, setA->count * sizeof (uint64_t));

        new_set->count = setA->count;

        for (size_t i = 0; i < setA->count; i++) {
                uint64_t similar = 0ULL;

                if (i < setB->count)
                        similar = setA->set[i] & setB->set[i];

                new_set->set[i] = setA->set[i] - similar;
        }

        return new_set;
}