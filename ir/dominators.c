#include "basicblock.h"
#include "dfa.h"
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

struct DFAConfiguration DominatorDFAConfiguration ()
{
        struct DFAConfiguration config = { .meet = DominatorMeet,
                                           .transfer = DominatorTransfer,
                                           .domain_value_type = DOMAIN_BASIC_BLOCK };

        hash_table_init (&config.in_set_inits, MAX_BASIC_BLOCK_COUNT);
        hash_table_init (&config.out_set_inits, MAX_BASIC_BLOCK_COUNT);
        DFABitMap_init (&config.top, MAX_BASIC_BLOCK_COUNT);
        DFABitMap_fill (&config.top);

        for (size_t i = 0; i < MAX_BASIC_BLOCK_COUNT; i++) {
                struct DFABitMap *out_map = ir_malloc (sizeof (struct DFABitMap));
                DFABitMap_init (out_map, MAX_BASIC_BLOCK_COUNT);
                DFABitMap_fill (out_map);

                hash_table_insert (&config.out_set_inits, i, out_map);

                struct DFABitMap *in_map = ir_malloc (sizeof (struct DFABitMap));
                DFABitMap_init (in_map, MAX_BASIC_BLOCK_COUNT);
                hash_table_insert (&config.in_set_inits, i, in_map);
        }

        return config;
}