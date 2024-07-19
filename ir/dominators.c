#include "dominators.h"
#include "array.h"
#include "basicblock.h"
#include "dfa.h"
#include "function.h"
#include "global_constants.h"
#include "map.h"
#include "mem.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void DominatorTransfer (struct BitMap *in, void *basic_block)
{
        BitMapSetBit (in, ((struct BasicBlock *)basic_block)->block_no);
}

void DominatorMeet (struct BitMap *accum, struct BitMap *item)
{
        BitMapInPlaceIntersect (accum, item);
}

struct DFAConfiguration DominatorDFAConfiguration (struct Function *function)
{
        struct DFAConfiguration config = { .Meet = DominatorMeet,
                                           .Transfer = DominatorTransfer,
                                           .direction = DFA_FORWARD,
                                           .domain_value_type = DOMAIN_BASIC_BLOCK };

        assert (BASICBLOCK_IS_ENTRY (function->entry_block));

        hash_table_init (&config.in_sets);
        hash_table_init (&config.out_sets);
        BitMapInit (&config.top, MAX_BASIC_BLOCK_COUNT);
        BitMapFill (&config.top);

        for (size_t block_no = 0; block_no < MAX_BASIC_BLOCK_COUNT; block_no++) {
                struct BitMap *out_map = BitMapCreate (MAX_BASIC_BLOCK_COUNT);

                if (block_no == function->entry_block->block_no) {
                        BitMapSetBit (out_map, block_no);
                } else {
                        BitMapFill (out_map);
                }

                hash_table_insert (&config.out_sets, block_no, out_map);

                struct BitMap *in_map = BitMapCreate (MAX_BASIC_BLOCK_COUNT);
                hash_table_insert (&config.in_sets, block_no, in_map);
        }

        return config;
}

struct DFAConfiguration PostDominatorDFAConfiguration (struct Function *function)
{
        struct DFAConfiguration config = { .Meet = DominatorMeet,
                                           .Transfer = DominatorTransfer,
                                           .direction = DFA_BACKWARD,
                                           .domain_value_type = DOMAIN_BASIC_BLOCK };

        assert (BASICBLOCK_IS_ENTRY (function->entry_block));

        hash_table_init (&config.in_sets);
        hash_table_init (&config.out_sets);
        BitMapInit (&config.top, MAX_BASIC_BLOCK_COUNT);
        BitMapFill (&config.top);

        for (size_t block_no = 0; block_no < MAX_BASIC_BLOCK_COUNT; block_no++) {
                struct BitMap *in_map = BitMapCreate (MAX_BASIC_BLOCK_COUNT);

                if (block_no == function->exit_block->block_no) {
                        BitMapSetBit (in_map, block_no);
                } else {
                        BitMapFill (in_map);
                }

                hash_table_insert (&config.in_sets, block_no, in_map);

                struct BitMap *out_map = BitMapCreate (MAX_BASIC_BLOCK_COUNT);
                hash_table_insert (&config.out_sets, block_no, out_map);
        }

        return config;
}

struct HashTable ComputePostDominatorTree (struct Function *function, struct DFAConfiguration *result)
{
        struct Array traversal_order = reverse_postorder (function->entry_block);
        struct BasicBlock *curr_block;
        HashTable dom_tree_adjacency_list;
        hash_table_init (&dom_tree_adjacency_list);
        size_t iter_count = 0;

        while ((curr_block = Array_iter (&traversal_order, &iter_count)) != NULL) {
                if (BASICBLOCK_IS_ENTRY (curr_block) || BASICBLOCK_IS_EXIT (curr_block))
                        continue;

                struct BitMap *out_map = hash_table_search (&result->out_sets, curr_block->block_no);

                size_t block_count = 0;

                struct BasicBlock *dom_block = NULL, *immediate_dom = NULL;

                // Dominators have a total ordering, meaning for any two
                // dominators a and b, either a dominates b (a > b) or b
                // dominates a (a < b). The immediate dominator is the "least"
                // element in this sequence (z): a > b > c > d ... > z
                while ((dom_block = BitMap_BasicBlock_iter (function, out_map, &block_count)) != NULL) {
                        if (!immediate_dom) {
                                immediate_dom = dom_block;
                                continue;
                        }

                        struct BitMap *dom_block_map = hash_table_search (&result->out_sets, dom_block->block_no);

                        // check if current immediate dominator dominates the
                        // dom_block candidate
                        if (BitMapIsSet (dom_block_map, immediate_dom->block_no)) {
                                immediate_dom = dom_block;
                        }
                }

                // we go through each block only once, create respective
                // adjacency list for this block in the hashtable
                struct Array *array = hash_table_search (&dom_tree_adjacency_list, immediate_dom->block_no);

                if (!array) {
                        array = Array_create ();
                        hash_table_insert (&dom_tree_adjacency_list, immediate_dom->block_no, array);
                }

                Array_push (array, curr_block);
        }

        Array_free (&traversal_order);

        return dom_tree_adjacency_list;
}

static HashTable ComputeDominatorTree (struct Function *function, struct DFAConfiguration *result)
{
        struct Array traversal_order = reverse_postorder (function->entry_block);
        struct BasicBlock *curr_block;
        HashTable dom_tree_adjacency_list;
        hash_table_init (&dom_tree_adjacency_list);
        size_t iter_count = 0;

        while ((curr_block = Array_iter (&traversal_order, &iter_count)) != NULL) {
                if (BASICBLOCK_IS_ENTRY (curr_block) || BASICBLOCK_IS_EXIT (curr_block))
                        continue;

                struct BitMap *in_map = hash_table_search (&result->in_sets, curr_block->block_no);

                size_t block_count = 0;

                struct BasicBlock *dom_block = NULL, *immediate_dom = NULL;

                // Dominators have a total ordering, meaning for any two
                // dominators a and b, either a dominates b (a > b) or b
                // dominates a (a < b). The immediate dominator is the "least"
                // element in this sequence (z): a > b > c > d ... > z
                while ((dom_block = BitMap_BasicBlock_iter (function, in_map, &block_count)) != NULL) {
                        if (!immediate_dom) {
                                immediate_dom = dom_block;
                                continue;
                        }

                        struct BitMap *dom_block_map = hash_table_search (&result->in_sets, dom_block->block_no);

                        // check if current immediate dominator dominates the
                        // dom_block candidate
                        if (BitMapIsSet (dom_block_map, immediate_dom->block_no)) {
                                immediate_dom = dom_block;
                        }
                }

                // we go through each block only once, create respective
                // adjacency list for this block in the hashtable
                struct Array *array = hash_table_search (&dom_tree_adjacency_list, immediate_dom->block_no);

                if (!array) {
                        array = Array_create ();
                        hash_table_insert (&dom_tree_adjacency_list, immediate_dom->block_no, array);
                }

                Array_push (array, curr_block);
        }

        Array_free (&traversal_order);

        return dom_tree_adjacency_list;
}

HashTable ComputePostDominanceFrontier (struct Function *function)
{
        struct DFAConfiguration config = PostDominatorDFAConfiguration (function);

        run_DFA (&config, function);
}

HashTable ComputeDominanceFrontier (struct Function *function)
{
        struct Array postorder_traversal = postorder (function->entry_block);
        struct DFAConfiguration config = DominatorDFAConfiguration (function);

        run_DFA (&config, function);

        HashTable dominator_tree_adj = ComputeDominatorTree (function, &config);

        // Compute the transpose graph from the dominator tree adjacency list
        // Each node is guaranteed to have only one direct predecessor, since
        // each node can only have one immediate dominator. We will need this in
        // the DF algorithm below
        HashTable dominator_tree_transpose;
        hash_table_init (&dominator_tree_transpose);

        struct HashTableEntry *entry;
        size_t entry_iter = 0;

        while ((entry = hash_table_entry_iter (&dominator_tree_adj, &entry_iter)) != NULL) {
                struct Array *doms = entry->value;
                size_t doms_iter = 0;
                struct BasicBlock *block, *parent = hash_table_search (&function->block_number_map, entry->key);
                while ((block = Array_iter (doms, &doms_iter)) != NULL) {
                        assert (hash_table_search (&dominator_tree_transpose, block->block_no) == NULL);

                        hash_table_insert (&dominator_tree_transpose, block->block_no, parent);
                }
        }

        struct BasicBlock *block;
        size_t iter_count = 0;

        // Table to store the dominance frontier of every node
        HashTable dominance_frontier;
        hash_table_init (&dominance_frontier);
        while ((block = Array_iter (&postorder_traversal, &iter_count)) != NULL) {
                // Dominance frontier blocks can only appear at join points in
                // the graph, disregard any block that has less than two
                // predecessors.
                if (Array_length (&block->preds) < 2)
                        continue;

                struct BasicBlock *immediate_dom = hash_table_search (&dominator_tree_transpose, block->block_no);
                // The dominance frontier sets of the direct predecessors of
                // this block contain this block, unless it is the immediate
                // dominator of this block
                size_t preds_iter = 0;
                struct BasicBlock *pred;
                while ((pred = BasicBlockPredsIter (block, &preds_iter))) {
                        // The dominators of each predecessor also contain this
                        // block in their dominance frontier sets, unless such
                        // block also dominates this block. Walk up the
                        // dominator tree adding the current block to the
                        // dominance frontier sets of each node until we reach
                        // the first node that also dominates the current block.

                        while (pred != immediate_dom) {
                                struct Array *df = hash_table_search (&dominance_frontier, pred->block_no);

                                if (!df) {
                                        df = Array_create ();
                                        hash_table_insert (&dominance_frontier, pred->block_no, df);
                                }

                                if (!Array_contains (df, block))
                                        Array_push (df, block);

                                pred = hash_table_search (&dominator_tree_transpose, pred->block_no);
                        }
                }
        }
        hash_table_free (&dominator_tree_adj);
        hash_table_free (&dominator_tree_transpose);
        Array_free (&postorder_traversal);
        return dominance_frontier;
}
