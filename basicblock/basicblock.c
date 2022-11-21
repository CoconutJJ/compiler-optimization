#include "basicblock.h"
#include "definition.h"
#include "integer_set.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

static Block *blocks = NULL;
static uint32_t block_id = 0;
static size_t block_size = 0;

Block *create_block ()
{
        if (block_id == block_size) {
                if (block_size == 0)
                        block_size = 8;
                else
                        block_size *= 2;

                blocks = compiler_realloc (blocks, block_size * sizeof (Block));
        }
        Block *blk = blocks + block_id;

        *blk = (Block){ .id = block_id++,
                        .count = 0,
                        .definitions = NULL,
                        .jump = NULL,
                        .fallthrough = NULL };

        return blk;
}

Block *get_block_id (size_t block_id)
{
        return blocks + block_id;
}

void get_killed_definitions (IntegerSet *buffer,
                             Block *block,
                             IntegerSet *current_definitions)
{
        for (size_t i = 0; i < block->count; i++) {
                Variable *dest = block->definitions[i].dest->variable;

                int64_t curr_def = -1;

                while (set_iter (current_definitions, &curr_def)) {
                        Definition *def = get_definition_no (curr_def);

                        Variable *def_var = def->dest->variable;

                        if (def_var == dest)
                                set_add (buffer, def->def_no);
                }
        }
}

void get_generated_definitions (IntegerSet *buffer, Block *block)
{
        for (size_t i = 0; i < block->count; i++)
                set_add (buffer, block->definitions[i].def_no);
}
