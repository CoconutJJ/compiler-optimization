#include "value.h"
#include "memory.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t allocate_value_no (ValueMap *map)
{
        return map->value_no++;
}

bool value_record_compare (ValueRecord a, ValueRecord b)
{
        return (a.left == b.left) && (a.op == b.op) && (a.right == b.right);
}

uint32_t allocate_value (ValueMap *map, ValueRecord record)
{
        for (size_t i = 0; i < map->count; i++) {
                if (value_record_compare (map->map[i], record))
                        return i;
        }

        uint32_t value = allocate_value_no (map);

        if (map->count <= value) {
                map->map =
                        compiler_realloc (map->map, (value + 1) * sizeof (ValueRecord));

                if (!map->map) {
                        perror ("realloc");
                        exit (EXIT_FAILURE);
                }
                map->count = value + 1;
        }

        map->map[value] = record;

        return value;
}

void process_value_update (ValueMap *map, Definition *def)
{
        ValueRecord rec = { .left = def->fst->value_no,
                            .op = def->op,
                            .right = def->snd->value_no };

        def->dest->value_no = allocate_value (map, rec);
}
