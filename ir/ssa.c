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
#include "mem.h"
#include "utils.h"
#include "value.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct SSAFrame *SSAFrame_create ()
{
        struct SSAFrame *frame = ir_malloc (sizeof (struct SSAFrame));

        frame->next = NULL;
        frame->prev = NULL;

        hash_table_init (&frame->variable_map);

        return frame;
}

void SSAFrame_free (struct SSAFrame *frame)
{
        hash_table_free (&frame->variable_map);

        ir_free (frame);
}

struct SSAFrame *SSAFrame_push (struct SSAFrame *curr_frame)
{
        curr_frame->next = SSAFrame_create ();

        curr_frame->next->prev = curr_frame;

        return curr_frame->next;
}

struct SSAFrame *SSAFrame_pop (struct SSAFrame *curr_frame)
{
        struct SSAFrame *prev = curr_frame->prev;

        SSAFrame_free (curr_frame);

        return prev;
}

void SSAFrame_insert (struct SSAFrame *frame, uint64_t key, void *value)
{
        hash_table_insert (&frame->variable_map, key, value);
}

void *SSAFrame_search (struct SSAFrame *frame, uint64_t key)
{
        while (frame != NULL) {
                void *value = hash_table_search (&frame->variable_map, key);

                if (value)
                        return value;

                frame = frame->prev;
        }

        return NULL;
}

static bool is_used_across_multiple_blocks (struct Instruction *alloca_instruction)
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

static struct Array Find_Allocas (struct Function *function)
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

static struct Instruction *Insert_Phi_Node (struct BasicBlock *basic_block)
{
        struct Instruction *phi_node = Instruction_create (OPCODE_PHI, Token (NIL, -1));

        BasicBlock_prepend_Instruction (basic_block, phi_node);

        return phi_node;
}

static HashTable Insert_Phi_Into_Blocks (struct Function *function, struct Array *allocas)
{
        struct Instruction *alloca_inst;
        size_t alloca_iter = 0;
        struct BitMap visited_blocks;
        BitMap_init (&visited_blocks, MAX_BASIC_BLOCK_COUNT);

        HashTable dominance_frontier = ComputeDominanceFrontier (function);

        // maps each phi node inserted to the alloca variable
        HashTable phi_node_mapping;
        hash_table_init (&phi_node_mapping);

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

                        while ((curr_frontier_node = Array_iter (&IDF, &frontier_iter)) != NULL) {
                                struct Instruction *phi_inst = Insert_Phi_Node (curr_frontier_node);
                                hash_table_insert (&phi_node_mapping, AS_VALUE (phi_inst)->value_no, alloca_inst);
                        }

                        Array_free (&IDF);
                }
        }

        hash_table_free (&dominance_frontier);

        return phi_node_mapping;
}

static void
Rename (struct BasicBlock *basic_block, HashTable *phi_node_mapping, struct SSAFrame *frame, struct BitMap *visited)
{
        if (!basic_block)
                return;

        struct Instruction *curr_inst;
        size_t iter_count = 0;
        while ((curr_inst = BasicBlock_Instruction_iter (basic_block, &iter_count)) != NULL) {
                if (!INST_ISA (curr_inst, OPCODE_PHI)) {
                        continue;
                }

                struct Value *alloca_inst = hash_table_search (phi_node_mapping, AS_VALUE (curr_inst)->value_no);

                struct Value *top_value = SSAFrame_search (frame, alloca_inst->value_no);

                Instruction_push_phi_operand_list (curr_inst, top_value);
        }

        if (BitMap_BitIsSet (visited, basic_block->block_no)) {
                return;
        } else {
                BitMap_setbit (visited, basic_block->block_no);
        }

        while ((curr_inst = BasicBlock_Instruction_iter (basic_block, &iter_count)) != NULL) {
                if (INST_ISA (curr_inst, OPCODE_LOAD)) {
                        struct Value *load_from = Instruction_Load_From_Operand (curr_inst);
                        Value_Replace_All_Uses_With (AS_VALUE(curr_inst), SSAFrame_search (frame, load_from->value_no));
                } else if (INST_ISA (curr_inst, OPCODE_STORE)) {
                        struct Value *store_to = Instruction_Store_To_Operand(curr_inst);
                        struct Value *store_from = Instruction_Store_From_Operand(curr_inst);
                        SSAFrame_insert(frame, store_to->value_no, store_from);
                }
        }

        if (basic_block->left) {
                frame = SSAFrame_push (frame);
                Rename (basic_block->left, phi_node_mapping, frame, visited);
                frame = SSAFrame_pop (frame);
        }

        if (basic_block->right) {
                frame = SSAFrame_push (frame);
                Rename (basic_block->right, phi_node_mapping, frame, visited);
                frame = SSAFrame_pop (frame);
        }
}