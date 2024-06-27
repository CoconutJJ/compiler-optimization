#include "array.h"
#include "global_constants.h"
#include "map.h"
#include "mem.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Array_init (struct Array *array)
{
        array->array = ir_malloc (DYNARR_INIT_SIZE_CNT * sizeof (void *));
        array->array_size = DYNARR_INIT_SIZE_CNT;
        array->array_count = 0;
}

struct Array *Array_create ()
{
        struct Array *array = ir_malloc (sizeof (struct Array));
        Array_init (array);

        return array;
}

static void Array_resize_if_required (struct Array *array)
{
        if (array->array_size == array->array_count) {
                array->array_size *= 2;
                array->array = ir_realloc (array->array, array->array_size * sizeof (void *));
        } else if (array->array_count < array->array_size / 4) {
                if (array->array_count == DYNARR_INIT_SIZE_CNT)
                        return;

                array->array_size /= 2;
                array->array = ir_realloc (array->array, array->array_size * sizeof (void *));
        }
}

void Array_push (struct Array *array, void *item)
{
        Array_resize_if_required (array);

        array->array[array->array_count++] = item;
}

bool Array_contains (struct Array *array, void *item)
{
        for (size_t i = 0; i < array->array_count; i++) {
                if (array->array[i] == item)
                        return true;
        }

        return false;
}

void *Array_pop (struct Array *array)
{
        if (array->array_count == 0)
                return NULL;

        void *last_item = array->array[array->array_count - 1];
        array->array_count--;

        return last_item;
}

void Array_insert (struct Array *array, size_t index, void *item)
{
        Array_resize_if_required (array);

        memmove (&array->array[index + 1], &array->array[index], (array->array_count - (index)) * sizeof (void *));

        array->array[index] = item;
}

void Array_set_index (struct Array *array, size_t index, void *item)
{
        array->array[index] = item;
}

void *Array_get_index (struct Array *array, size_t index)
{
        return array->array[index];
}

size_t Array_length (struct Array *array)
{
        return array->array_count;
}

void *Array_top (struct Array *array)
{
        return array->array[array->array_count - 1];
}

void Array_reverse (struct Array *array)
{
        size_t i = 0, j = Array_length (array) - 1;

        while (i < j) {
                void *tmp = array->array[i];
                array->array[i] = array->array[j];
                array->array[j] = tmp;

                i++;
                j--;
        }
}

struct Array Array_copy (struct Array *array)
{
        struct Array copy = { .array = ir_malloc (array->array_size * sizeof (void *)),
                              .array_size = array->array_size,
                              .array_count = array->array_count };

        memcpy (copy.array, array->array, array->array_count * sizeof (void *));

        return copy;
}

void *Array_iter (struct Array *array, size_t *iter_count)
{
        if (*iter_count == Array_length (array)) {
                *iter_count = 0;
                return NULL;
        }

        return Array_get_index (array, (*iter_count)++);
}

void Array_empty (struct Array *array)
{
        array->array_count = 0;

        if (array->array_size > DYNARR_INIT_SIZE_CNT) {
                array->array_size = DYNARR_INIT_SIZE_CNT;
                array->array = ir_realloc (array->array, array->array_size * sizeof (void *));
        }
}

void Array_apply (struct Array *array, ArrayApplyFn map)
{
        for (size_t i = 0; i < array->array_count; i++) {
                map (array->array[i]);
        }
}

void Array_free (struct Array *array)
{
        ir_free (array->array);
}