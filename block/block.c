#include "block.h"
#include "definition.h"
#include "integer_set.h"
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

                blocks = realloc (blocks, block_size * sizeof (Block));
        }
        Block *blk = blocks + block_id;

        blk->id = block_id++;
        blk->count = 0;
        blk->definitions = NULL;
        blk->jump = NULL;
        blk->fallthrough = NULL;

        return blk;
}

IntegerSet *get_killed_definitions (Block *block,
                                    IntegerSet *current_definitions)
{
        IntegerSet *killed_defs = set_create ();

        for (size_t i = 0; i < block->count; i++) {
                Variable *dest = block->definitions[i].dest->variable;

                int64_t curr_def = -1;

                while (set_iter (current_definitions, &curr_def)) {
                        Definition *def = get_definition_no (curr_def);

                        Variable *def_var = def->dest->variable;

                        if (def_var == dest)
                                set_add (killed_defs, def->def_no);
                }
        }

        return killed_defs;
}

IntegerSet *get_generated_definitions (Block *block)
{
        IntegerSet *generated_defs = set_create ();

        for (size_t i = 0; i < block->count; i++)
                set_add (generated_defs, block->definitions[i].def_no);

        return generated_defs;
}
