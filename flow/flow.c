
#include "flow.h"
#include "basicblock.h"
#include "integer_set.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>

typedef void (*MeetOperator) (IntegerSet *accum, IntegerSet *elem);
typedef void (*TransferFunction) (IntegerSet *buffer,
                                  IntegerSet *current,
                                  Block *current_block);
IntegerSet *ins = NULL;
size_t ins_count = 0;
IntegerSet *outs = NULL;
size_t outs_count = 0;

IntegerSet *get_in_set_of_block (Block *block)
{
        if (ins_count <= block->id) {
                ins = compiler_realloc (ins,
                                        (block->id + 1) * sizeof (IntegerSet));

                for (; ins_count < block->id + 1; ins_count++)
                        set_init (ins + block->id);
        }

        return ins + block->id;
}

IntegerSet *get_out_set_of_block (Block *block)
{
        if (outs_count <= block->id) {
                outs = compiler_realloc (outs,
                                         (block->id + 1) * sizeof (IntegerSet));

                for (; outs_count < block->id + 1; outs_count++)
                        set_init (outs + block->id);
        }

        return outs + block->id;
}

void forward_flow (MeetOperator Meet, TransferFunction F)
{
        bool has_changes = false;
        do {
                Block *block;

                for (size_t i = 0; (block = get_block_id (i)) != NULL; i++) {
                        IntegerSet pred_meet;
                        set_init (&pred_meet);

                        int64_t pred_block_id = -1LL;
                        while (set_iter (&(block->predecessors),
                                         &pred_block_id)) {
                                Block *pred_block =
                                        get_block_id (pred_block_id);

                                IntegerSet *out_set =
                                        get_out_set_of_block (pred_block);

                                Meet (&pred_meet, out_set);
                        }

                        IntegerSet new_out;
                        set_init (&new_out);

                        F (&new_out, &pred_meet, block);

                        IntegerSet *out_set = get_out_set_of_block (block);

                        if (!set_equal (out_set, &new_out))
                                has_changes = true;

                        set_copy (out_set, &new_out);
                }

        } while (has_changes);
}

void reaching_defintions_transfer_F (IntegerSet *buffer,
                                     IntegerSet *current,
                                     Block *current_block)
{
        IntegerSet generated;
        IntegerSet killed;
        set_init (&generated);
        set_init (&killed);

        get_generated_definitions (&generated, current_block);
        get_killed_definitions (&killed, current_block, current);

        set_subtraction (current, &killed);
        set_union (&generated, current);
        set_copy (buffer, &generated);
}

void compute_reaching_definitions ()
{
        forward_flow (set_union, reaching_defintions_transfer_F);
}