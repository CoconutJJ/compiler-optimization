#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct Cell {
        int deleted;
        int used;
        uint32_t key;
        void *value;

} Cell;

typedef struct Map {
        Cell *map;
        size_t count;
        size_t size;

} IntegerMap;

uint32_t map_hash2 (uint32_t a, uint32_t b);
uint32_t map_hash3 (uint32_t a, uint32_t b, uint32_t c);
void init_map (IntegerMap *map, size_t initial_size);
void map_free (IntegerMap *map);
void map_resize (IntegerMap *map);
void map_insert (IntegerMap *map, uint32_t key, void *value);
void *map_find (IntegerMap *map, uint32_t key);
bool map_delete (IntegerMap *map, uint32_t key);
