
#include "flow.h"
#include "block.h"
#include "integer_set.h"
#include <stdio.h>
#include <stdlib.h>

IntegerSet *ins = NULL;
size_t ins_count = 0;
IntegerSet *outs = NULL;
size_t outs_count = 0;

IntegerSet *get_in_set_of_block (Block *block)
{
        if (ins_count <= block->id) {
                ins = realloc (ins, (block->id + 1) * sizeof (IntegerSet));

                for (; ins_count < block->id + 1; ins_count++)
                        set_init (ins + block->id);
        }

        return ins + block->id;
}

IntegerSet *get_out_set_of_block (Block *block)
{
        if (outs_count <= block->id) {
                outs = realloc (outs, (block->id + 1) * sizeof (IntegerSet));

                for (; outs_count < block->id + 1; outs_count++)
                        set_init (outs + block->id);
        }

        return outs + block->id;
}

IntegerSet *get_union_of_out_set_predecessors (Block *block)
{
        int64_t pred_block_id = -1LL;
        IntegerSet *un = set_create ();
        while (set_iter (&(block->predecessors), &pred_block_id)) {
                set_union (un,
                           get_out_set_of_block (get_block_id (pred_block_id)));
        }

        return un;
}

void compute_reaching_definitions ()
{
        bool has_changes;

        do {
                has_changes = false;
                for (size_t i = 0;; i++) {
                        Block *block = get_block_id (i);

                        if (!block)
                                break;

                        IntegerSet *in_set = get_in_set_of_block (block);
                        IntegerSet *out_set = get_out_set_of_block (block);

                        IntegerSet *new_in =
                                get_union_of_out_set_predecessors (block);

                        IntegerSet *generated =
                                get_generated_definitions (block);
                        IntegerSet *killed =
                                get_killed_definitions (block, in_set);

                        IntegerSet *new_out = set_union (
                                generated, set_subtraction (in_set, killed));

                        if (!has_changes)
                                has_changes = !set_equal (new_out, out_set);

                        set_copy (in_set, new_in);
                        set_copy (out_set, new_out);

                        set_destroy (new_in);
                        set_destroy (generated);
                        set_destroy (killed);
                        set_destroy (new_out);
                }
        } while (has_changes);
}