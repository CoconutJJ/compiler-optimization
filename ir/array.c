#include "array.h"
#include "map.h"
#include "mem.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dynarr_init (void **buffer, size_t *count, size_t *size, size_t item_size)
{
        *buffer = ir_malloc (item_size * DYNARR_INIT_SIZE_CNT);

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

void dynarr_set (void **buffer, size_t *count, size_t *size, size_t index, void *item, size_t item_size)
{
        assert (index * item_size < *count);

        void *set_addr = DYNARR_ITEM_ADDR (*buffer, item_size, index);

        memcpy (set_addr, item, item_size);
}

void Array_init (struct Array *array)
{
        array->array = ir_malloc (DYNARR_INIT_SIZE_CNT * sizeof (void *));
        array->array_size = DYNARR_INIT_SIZE_CNT;
        array->array_count = 0;
}

void Array_resize_if_required (struct Array *array)
{
        if (array->array_size == array->array_count) {
                array->array_size *= 2;
                array->array = ir_realloc (array->array, array->array_size * sizeof (void *));
        } else if (array->array_count < array->array_size / 4) {
                array->array_size /= 2;
                array->array = ir_realloc (array->array, array->array_size * sizeof (void *));
        }
}

void Array_push (struct Array *array, void *item)
{
        Array_resize_if_required (array);

        array->array[array->array_count++] = item;
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

void *Array_iter(struct Array *array, size_t *iter_count) {

        if (*iter_count == Array_length(array))
                return NULL;

        return Array_get_index(array, *iter_count++);

}

void Array_free (struct Array *array)
{
        ir_free (array->array);
}