#include "map.h"
#include "mem.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void map_insert (IntegerMap *map, uint32_t key, void *value);

uint32_t cantor_pair (uint32_t a, uint32_t b)
{
        return (a + b + 1) * (a + b) / 2 + b;
}

uint32_t map_hash2 (uint32_t a, uint32_t b)
{
        return cantor_pair (a, b);
}

uint32_t map_hash3 (uint32_t a, uint32_t b, uint32_t c)
{
        return map_hash2 (map_hash2 (a, b), c);
}

void init_map (IntegerMap *map, size_t initial_size)
{
        if (initial_size != 0)
                map->map = compiler_calloc (initial_size, sizeof (Cell));
        else
                map->map = NULL;

        map->count = 0;
        map->size = initial_size;
}

void map_free (IntegerMap *map)
{
        free (map->map);
}

void map_resize (IntegerMap *map)
{
        if (map->count < map->size)
                return;

        IntegerMap new_map;
        init_map (&new_map, map->size * 2);

        for (size_t i = 0; i < map->size; i++) {
                if (map->map[i].key == 0)
                        continue;

                map_insert (&new_map, map->map[i].key, map->map[i].value);
        }

        map_free (map);
        *map = new_map;
}

void map_insert (IntegerMap *map, uint32_t key, void *value)
{
        map_resize (map);
        uint32_t idx = key % map->size;

        while (1) {
                Cell *curr = map->map + idx;

                if (curr->used && !curr->deleted) {
                        idx = (idx + 1) % map->size;
                        continue;
                }

                curr->used = 1;
                curr->deleted = 0;
                curr->key = key;
                curr->value = value;

                break;
        }

        map->count++;
}

Cell *map_find_cell (IntegerMap *map, uint32_t key)
{
        uint32_t idx = key % map->size;

        size_t total_size = map->size;

        while (total_size > 0) {
                Cell *curr = map->map + idx;

                if (curr->deleted) {
                        continue;
                }

                if (curr->key == key)
                        return curr;

                if (!curr->used) {
                        return NULL;
                }

                idx = (idx + 1) % map->size;
                total_size--;
        }
        return NULL;
}

bool map_delete (IntegerMap *map, uint32_t key)
{
        Cell *cell = map_find_cell (map, key);

        if (!cell)
                return false;

        cell->deleted = 1;

        return true;
}

void *map_find (IntegerMap *map, uint32_t key)
{
        Cell *cell = map_find_cell (map, key);

        if (!cell)
                return NULL;

        return cell->value;
}
