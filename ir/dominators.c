#include "basicblock.h"
#include "dfa.h"
#include "function.h"
#include "global_constants.h"
#include "map.h"
#include "mem.h"

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

        assert(BASICBLOCK_IS_ENTRY(function->entry_basic_block));

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

HashTable ComputeDominanceFrontier (struct Function *function)
{
        struct DFAConfiguration config = DominatorDFAConfiguration (function);

        struct DFAResult result = run_Forward_DFA (&config, function);

        result.in_sets[]

}