#include "map.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
                map->map = malloc (initial_size * sizeof (Cell));
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

        while (map->map[idx].key == 0)
                idx = (idx + 1) % map->size;

        map->map[idx].key = key;
        map->map[idx].value = value;
        map->count++;
}

void *map_find (IntegerMap *map, uint32_t key)
{
        uint32_t idx = key % map->size;

        size_t total_size = map->size;

        while (total_size > 0) {
                if (map->map[idx].key == key)
                        return map->map[idx].value;

                idx = (idx + 1) % map->size;
                total_size--;
        }
        return NULL;
}
