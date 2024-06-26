#include "ssa.h"
#include "array.h"
#include "basicblock.h"
#include "bitmap.h"
#include "dfa.h"
#include "dominators.h"
#include "function.h"
#include "global_constants.h"
#include "instruction.h"
#include "lexer.h"
#include "map.h"
#include "utils.h"
#include "value.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
/*

                alloca %3, 4
                load %4, %3
add %5, %4, 1                   add %6, %4, 1
store %3, %5                    store %3, %6

                load %7, %3

*/

static bool is_live_across_multiple_blocks (struct Instruction *alloca_instruction)
{
        if (Value_Use_count (AS_VALUE (alloca_instruction)) < 2) {
                return false;
        }

        struct Value *use;
        size_t use_iter = 0;

        struct BitMap blocks_def;
        struct BitMap blocks_use;

        BitMap_init (&blocks_def, MAX_BASIC_BLOCK_COUNT);
        BitMap_init (&blocks_use, MAX_BASIC_BLOCK_COUNT);

        while ((use = Value_Use_iter (AS_VALUE (alloca_instruction), &use_iter)) != NULL) {
                if (INST_ISA (AS_INST (use), OPCODE_LOAD)) {
                        BitMap_setbit (&blocks_use, AS_INST (use)->parent->block_no);
                } else if (INST_ISA (AS_INST (use), OPCODE_STORE)) {
                        BitMap_setbit (&blocks_def, AS_INST (use)->parent->block_no);
                } else {
                        error (use->token, "%s cannot reference an alloca target", Token_to_str (use->token));
                        error (AS_VALUE (alloca_instruction)->token, "note: alloca instruction defined here");
                        exit (EXIT_FAILURE);
                }
        }

        size_t use_count = BitMap_count (&blocks_use);

        BitMap_free (&blocks_use);
        BitMap_free (&blocks_def);

        if (use_count < 2)
                return false;

        return true;
}

static struct Array find_allocas (struct Function *function)
{
        struct Array postorder_traversal = postorder (function->entry_basic_block);

        struct Array allocas;

        Array_init (&allocas);

        struct BasicBlock *block;
        size_t iter_count = 0;

        while ((block = Array_iter (&postorder_traversal, &iter_count)) != NULL) {
                size_t inst_iter = 0;
                struct Instruction *inst;

                while ((inst = BasicBlock_Instruction_iter (block, &inst_iter)) != NULL) {
                        if (!INST_ISA (inst, OPCODE_ALLOCA)) {
                                continue;
                        }

                        Array_push (&allocas, inst);
                }
        }

        Array_free (&postorder_traversal);

        return allocas;
}

static void insert_Phi (struct Function *function, struct Array *allocas)
{
        struct Instruction *alloca_inst;
        size_t alloca_iter = 0;
        struct BitMap visited_blocks;
        BitMap_init (&visited_blocks, MAX_BASIC_BLOCK_COUNT);

        HashTable dominance_frontier = ComputeDominanceFrontier (function);

        while ((alloca_inst = Array_iter (allocas, &alloca_iter)) != NULL) {
                struct Instruction *use;
                size_t use_iter = 0;

                while ((use = AS_INST (Value_Use_iter (AS_VALUE (alloca_inst), &use_iter))) != NULL) {
                        if (!INST_ISA (AS_INST (use), OPCODE_STORE))
                                continue;

                        struct BasicBlock *parent_block = use->parent;

                        // Compute the Iterated Dominance Frontier, first find
                        // the dominance frontier of the current block
                        struct Array *parent_frontier_nodes =
                                hash_table_search (&dominance_frontier, parent_block->block_no);

                        if (!parent_frontier_nodes)
                                continue;

                        // array to store all of the Iterated Dominance Frontier
                        // nodes, initialized to current dominance frontier
                        struct Array IDF = Array_copy (parent_frontier_nodes);

                        // loop through IDF,
                        struct BasicBlock *curr_frontier_node;
                        size_t frontier_iter = 0;
                        while ((curr_frontier_node = Array_iter (&IDF, &frontier_iter)) != NULL) {
                                struct Array *child_frontier_nodes =
                                        hash_table_search (&dominance_frontier, curr_frontier_node->block_no);

                                if (!child_frontier_nodes)
                                        continue;

                                struct BasicBlock *child_frontier_node;
                                size_t child_frontier_iter = 0;
                                while ((child_frontier_node =
                                                Array_iter (child_frontier_nodes, &child_frontier_iter)) != NULL) {
                                        if (!Array_contains (&IDF, child_frontier_node))
                                                Array_push (&IDF, child_frontier_node);
                                }
                        }

                        // TODO: insert a Phi node for each block in IDF list
                }
        }

        hash_table_free (&dominance_frontier);
}
