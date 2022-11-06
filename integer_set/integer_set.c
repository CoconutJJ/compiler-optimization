#include "integer_set.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
}

void set_add (IntegerSet *set, uint64_t v)
{
        uint64_t index = v / 64;
        int bit = v % 64;

        if (index >= set->count) {
                set->set = realloc (set->set, (index + 1) * sizeof (uint64_t));

                if (!set->set) {
                        perror ("realloc");
                        exit (EXIT_FAILURE);
                }

                for (size_t i = set->count; i <= index; i++)
                        set->set[i] = 0;

                set->count = (index + 1);
        }

        set->set[index] |= (1 << bit);
}

IntegerSet *set_union (IntegerSet *setA, IntegerSet *setB)
{
        size_t new_count =
                setA->count > setB->count ? setA->count : setB->count;

        IntegerSet *new_set = set_create ();

        new_set->set = malloc (new_count * sizeof (uint64_t));
        new_set->count = new_count;

        for (size_t i = 0; i < new_count; i++) {
                uint64_t un = 0;

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
        size_t new_count =
                setA->count > setB->count ? setA->count : setB->count;

        IntegerSet *new_set = set_create ();

        new_set->set = malloc (new_count * sizeof (uint64_t));
        new_set->count = new_count;

        for (size_t i = 0; i < new_count; i++) {
                if (i < setA->count && i < setB->count)
                        new_set->set[i] = setA->set[i] & setB->set[i];
                else
                        new_set->set[i] = 0;
        }

        return new_set;
}

IntegerSet *set_subtraction (IntegerSet *setA, IntegerSet *setB)
{
        IntegerSet *new_set = set_create ();

        new_set->set = malloc (setA->count * sizeof (uint64_t));
        new_set->count = setA->count;

        for (size_t i = 0; i < setA->count; i++) {
                uint64_t similar = 0;

                if (i < setB->count) {
                        similar = setA->set[i] & setB->set[i];
                }

                new_set->set[i] = setA->set[i] - similar;
        }

        return new_set;
}