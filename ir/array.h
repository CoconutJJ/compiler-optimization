#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
struct Array {
        void **array;
        size_t array_size;
        size_t array_count;
};

#define DYNARR_INIT_SIZE_CNT                       10
#define AS_BYTE_BUFFER(buf)                        ((uint8_t *)(buf))
#define DYNARR_ITEM_OFFSET(item_size, index)       ((index) * (item_size))
#define DYNARR_ITEM_ADDR(buffer, item_size, index) (AS_BYTE_BUFFER (buffer) + DYNARR_ITEM_OFFSET (item_size, index))

void Array_init (struct Array *array);
struct Array *Array_create (void);
void Array_push (struct Array *array, void *item);
void *Array_pop (struct Array *array);
bool Array_contains (struct Array *array, void *item);
size_t Array_length (struct Array *array);
void *Array_top (struct Array *array);
void Array_free (struct Array *array);
void Array_reverse (struct Array *array);
void *Array_get_index (struct Array *array, size_t index);
void Array_set_index (struct Array *array, size_t index, void *item);
void Array_insert (struct Array *array, size_t index, void *item);
void *Array_iter (struct Array *array, size_t *iter_count);