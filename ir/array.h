#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
struct Array {
        uint8_t *array;
        size_t item_size;
        size_t array_size;
        size_t array_count;
};

#define DYNARR_INIT_SIZE_CNT 10
#define AS_BYTE_BUFFER(buf)  ((uint8_t *)(buf))

#define DYNARR_INIT(buffer, count, size, item_size)                                                                    \
        dynarr_init ((void **)&(buffer), (void *)&(count), (void *)&(size), (item_size))
#define DYNARR_PUSH(buffer, count, size, item)                                                                         \
        dynarr_push ((void **)&(buffer), (void *)&(count), (void *)&(size), item, sizeof (item))
#define DYNARR_ALLOC(buffer, count, size, item_size)                                                                   \
        dynarr_push ((void **)&(buffer), (void *)&(count), (void *)&(size), NULL, item_size)
#define DYNARR_POP(buffer, count, retaddr, item_size)                                                                  \
        dynarr_pop ((void **)&(buffer), (void *)&(count), retaddr, item_size)
#define DYNARR_INSERT(buffer, count, size, item, item_size, index)                                                     \
        dynarr_insert ((void **)&(buffer), (void *)&(count), (void *)&(size), item, sizeof (item), (index));

#define DYNARR_ITEM_OFFSET(item_size, index)       ((index) * (item_size))
#define DYNARR_ITEM_ADDR(buffer, item_size, index) (AS_BYTE_BUFFER (buffer) + DYNARR_ITEM_OFFSET (item_size, index))

void dynarr_init (void **buffer, size_t *count, size_t *size, size_t item_size);
void _dynarr_auto_alloc (void **buffer, size_t *count, size_t *size, size_t item_size);
void *dynarr_insert (void **buffer, size_t *count, size_t *size, void *item, size_t item_size, size_t insert_index);
void *dynarr_push (void **buffer, size_t *count, size_t *size, void *item, size_t item_size);
void dynarr_delete (void **buffer, size_t *count, size_t *size, size_t item_size, size_t delete_index);
void dynarr_pop (void **buffer, size_t *count, size_t *size, void *ret, size_t item_size);

void Array_init (struct Array *array, size_t item_size);
void Array_push (struct Array *array, void *item);
void *Array_pop (struct Array *array, bool return_item);
size_t Array_length (struct Array *array);
void *Array_top (struct Array *array);
void Array_free (struct Array *array);
void Array_reverse (struct Array *array);