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

#define ARRAY_ITER(array, array_var, idx_var)                                                                          \
        struct Array *array_var = NULL;                                                                                \
        size_t idx_var = 0;                                                                                            \
        while ((array_var = Array_iter (array, &idx_var)) != NULL)

typedef void (*ArrayApplyFn) (void *);

void Array_init (struct Array *array);
struct Array *Array_create (void);
void Array_push (struct Array *array, void *item);
void *Array_pop (struct Array *array);
bool Array_contains (struct Array *array, void *item);
size_t Array_length (struct Array *array);
void *Array_top (struct Array *array);
void Array_free (struct Array *array);
void Array_destroy (struct Array *array);
void Array_reverse (struct Array *array);
void *Array_get_index (struct Array *array, size_t index);
void Array_set_index (struct Array *array, size_t index, void *item);
void Array_delete (struct Array *array, size_t index);
void Array_insert (struct Array *array, size_t index, void *item);
void *Array_iter (struct Array *array, size_t *iter_count);
struct Array Array_copy (struct Array *array);
void Array_empty (struct Array *array);
void Array_apply (struct Array *array, ArrayApplyFn map);
bool Array_find_and_delete (struct Array *array, void *item);