#include "array.h"
#include "basicblock.h"
#include "dfa.h"
#include "function.h"
#include "global_constants.h"
#include "map.h"
#include "mem.h"
#include <stdint.h>
#include <stdio.h>

void DominatorTransfer (struct DFABitMap *in, void *basic_block)
{
        DFABitMap_setbit (in, ((struct BasicBlock *)basic_block)->block_no);
}

void DominatorMeet (struct DFABitMap *accum, struct DFABitMap *item)
{
        DFABitMap_inplace_Intersect (accum, item);
}

struct DFAConfiguration DominatorDFAConfiguration (struct Function *function)
{
        struct DFAConfiguration config = { .meet = DominatorMeet,
                                           .transfer = DominatorTransfer,
                                           .domain_value_type = DOMAIN_BASIC_BLOCK };

        assert (BASICBLOCK_IS_ENTRY (function->entry_basic_block));

        hash_table_init (&config.in_set_inits);
        hash_table_init (&config.out_set_inits);
        DFABitMap_init (&config.top, MAX_BASIC_BLOCK_COUNT);
        DFABitMap_fill (&config.top);

        for (size_t block_no = 0; block_no < MAX_BASIC_BLOCK_COUNT; block_no++) {
                struct DFABitMap *out_map = DFABitMap_create (MAX_BASIC_BLOCK_COUNT);

                if (block_no == function->entry_basic_block->block_no) {
                        DFABitMap_setbit (out_map, block_no);
                } else {
                        DFABitMap_fill (out_map);
                }

                hash_table_insert (&config.out_set_inits, block_no, out_map);

                struct DFABitMap *in_map = DFABitMap_create (MAX_BASIC_BLOCK_COUNT);
                hash_table_insert (&config.in_set_inits, block_no, in_map);
        }

        return config;
}

void ComputeDominanceFrontier (struct Function *function)
{
        struct DFAConfiguration config = DominatorDFAConfiguration (function);

        struct DFAResult result = run_Forward_DFA (&config, function);

        struct Array traversal_order = reverse_postorder_iter (function->entry_basic_block);

        int64_t block_no;

        for (size_t i = 0; i < Array_length (&traversal_order); i++) {
                struct BasicBlock *curr_basic_block = Array_get_index (&traversal_order, i);

                struct DFABitMap *in_map = hash_table_search (&result.in_sets, curr_basic_block->block_no);
                size_t iter_count = 0;

                printf ("Block %ld is dominated by...\n", curr_basic_block->block_no);
                while ((block_no = DFABitMap_iter (in_map, &iter_count)) != -1LL) {
                        printf ("Block %lld\n", block_no);
                }
        }
}