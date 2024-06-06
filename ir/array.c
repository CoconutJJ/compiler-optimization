#include "array.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dynarr_init (void **buffer, size_t *count, size_t *size, size_t item_size)
{
        *buffer = malloc (item_size * DYNARR_INIT_SIZE_CNT);

        if (!*buffer) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        *count = 0;
        *size = item_size * DYNARR_INIT_SIZE_CNT;
}

void _dynarr_auto_alloc (void **buffer, size_t *count, size_t *size, size_t item_size)
{
        if (*size - *count <= item_size) {
                while (*size - *count <= item_size) {
                        *size *= 2;
                }
        } else if (*count < *size / 4) {
                
                // never shrink beyond initialized size
                if (*size == item_size * DYNARR_INIT_SIZE_CNT)
                        return;

                *size /= 2;

        } else {
                return;
        }

        *buffer = realloc (*buffer, *size);
        if (!*buffer) {
                perror ("realloc");
                exit (EXIT_FAILURE);
        }
}

void *dynarr_insert (void **buffer, size_t *count, size_t *size, void *item, size_t item_size, size_t insert_index)
{
        // ensure the insert address is within buffer bounds
        assert (DYNARR_ITEM_OFFSET (item_size, insert_index) <= *count);

        // check if there is enough available space in buffer, otherwise allocate more.
        _dynarr_auto_alloc (buffer, count, size, item_size);

        // starting address in buffer to insert item
        void *insert_addr = DYNARR_ITEM_ADDR (*buffer, item_size, insert_index);

        // number of bytes we have to shift in order to insert item
        size_t shift_size = AS_BYTE_BUFFER (*buffer) + *count - AS_BYTE_BUFFER (insert_addr);

        // shift
        if (shift_size > 0)
                memmove (AS_BYTE_BUFFER (insert_addr) + item_size, insert_addr, shift_size);

        *count += item_size;

        // copy in item if specified
        if (item)
                memcpy (insert_addr, item, item_size);

        return insert_addr;
}

void *dynarr_push (void **buffer, size_t *count, size_t *size, void *item, size_t item_size)
{
        return dynarr_insert (buffer, count, size, item, item_size, *count / item_size);
}

void dynarr_delete (void **buffer, size_t *count, size_t *size, size_t item_size, size_t delete_index)
{
        size_t last_item_idx = *count / item_size - 1;
        assert (last_item_idx >= delete_index);

        size_t shift_size = AS_BYTE_BUFFER (*buffer) + *count - DYNARR_ITEM_ADDR (*buffer, item_size, delete_index + 1);

        void *shift_start_addr = DYNARR_ITEM_ADDR (*buffer, item_size, delete_index + 1);

        if (shift_size > 0)
                memmove (DYNARR_ITEM_ADDR (*buffer, item_size, delete_index), shift_start_addr, shift_size);

        *count -= item_size;

        _dynarr_auto_alloc (buffer, count, size, item_size);
}

void dynarr_pop (void **buffer, size_t *count, size_t *size, void *ret, size_t item_size)
{
        size_t last_item_idx = *count / item_size - 1;

        if (ret)
                memcpy (ret, DYNARR_ITEM_ADDR (buffer, item_size, last_item_idx), item_size);

        dynarr_delete (buffer, count, size, item_size, last_item_idx);
}

void Array_init (struct Array *array, size_t item_size)
{
        array->item_size = item_size;

        DYNARR_INIT (array->array, array->array_count, array->array_size, item_size);
}

void Array_push (struct Array *array, void *item)
{
        dynarr_push ((void **)&(array->array), &(array->array_count), &(array->array_size), item, array->item_size);
}

void *Array_pop (struct Array *array, bool return_item)
{
        void *item = return_item ? malloc (array->item_size) : NULL;

        if (!item && return_item) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        dynarr_pop ((void **)&(array->array), &(array->array_count), &(array->array_size), item, array->item_size);

        return item;
}

void *Array_get_index (struct Array *array, size_t index)
{
        return array->array + index * array->item_size;
}

size_t Array_length (struct Array *array)
{
        return array->array_count / array->item_size;
}

void *Array_top (struct Array *array)
{
        return Array_get_index (array, Array_length (array) - 1);
}

void Array_swap_items (struct Array *array, size_t i, size_t j)
{
        struct Array *a = Array_get_index (array, i), *b = Array_get_index (array, j);

        void *item = malloc (array->item_size);

        if (!item) {
                perror ("malloc");
                exit (EXIT_FAILURE);
        }

        memcpy (item, a, array->item_size);
        memcpy (a, b, array->item_size);
        memcpy (b, item, array->item_size);
        free (item);
}

void Array_reverse (struct Array *array)
{
        size_t i = 0, j = Array_length (array) - 1;

        while (i < j)
                Array_swap_items (array, i++, j--);
}

void Array_free (struct Array *array)
{
        free (array->array);
}