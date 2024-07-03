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

static struct SSAFrame *SSAFrameCreate (struct BasicBlock *block)
{
        struct SSAFrame *frame = ir_malloc (sizeof (struct SSAFrame));

        frame->block = block;
        frame->next = NULL;
        frame->prev = NULL;

        hash_table_init (&frame->variable_map);

        return frame;
}

static void SSAFrameFree (struct SSAFrame *frame)
{
        hash_table_free (&frame->variable_map);

        ir_free (frame);
}

static struct SSAFrame *SSAFramePush (struct SSAFrame *curr_frame, struct BasicBlock *block)
{
        curr_frame->next = SSAFrameCreate (block);

        curr_frame->next->prev = curr_frame;

        return curr_frame->next;
}

static struct SSAFrame *SSAFramePop (struct SSAFrame *curr_frame)
{
        struct SSAFrame *prev = curr_frame->prev;

        SSAFrameFree (curr_frame);

        return prev;
}

static void SSAFrameInsert (struct SSAFrame *frame, uint64_t key, void *value)
{
        hash_table_insert (&frame->variable_map, key, value);
}

static void *SSAFrameSearch (struct SSAFrame *frame, uint64_t key)
{
        while (frame != NULL) {
                void *value = hash_table_search (&frame->variable_map, key);

                if (value)
                        return value;

                frame = frame->prev;
        }

        return NULL;
}

static struct BasicBlock *SSAFrameGetParentBlock (struct SSAFrame *frame)
{
        if (!frame->prev)
                return NULL;

        return frame->prev->block;
}

static struct Array FindAllocas (struct Function *function)
{
        struct Array postorder_traversal = postorder (function->entry_basic_block);

        struct Array allocas;

        Array_init (&allocas);

        struct BasicBlock *block;
        size_t iter_count = 0;

        while ((block = Array_iter (&postorder_traversal, &iter_count)) != NULL) {
                size_t inst_iter = 0;
                struct Instruction *inst;

                while ((inst = BasicBlockInstructionIter (block, &inst_iter)) != NULL) {
                        if (!INST_ISA (inst, OPCODE_ALLOCA)) {
                                continue;
                        }

                        Array_push (&allocas, inst);
                }
        }

        Array_free (&postorder_traversal);

        return allocas;
}

static struct Instruction *InsertPhiNode (struct BasicBlock *basic_block)
{
        struct Instruction *phi_node = Instruction_create (OPCODE_PHI, Token (NIL, -1));

        BasicBlockPrependInstruction (basic_block, phi_node);

        return phi_node;
}

static bool BlockHasPhiNodeForAlloca (struct BasicBlock *block, struct Instruction *alloca_inst, HashTable *phi_map)
{
        struct Instruction *curr_inst;
        size_t iter_count = 0;

        while ((curr_inst = BasicBlockInstructionIter (block, &iter_count)) != NULL) {
                if (!INST_ISA (curr_inst, OPCODE_PHI))
                        continue;

                struct Instruction *alloca_candidate = hash_table_search (phi_map, AS_VALUE (curr_inst)->value_no);

                if (alloca_candidate == alloca_inst)
                        return true;
        }

        return false;
}

static HashTable InsertPhiIntoBlocks (struct Function *function, struct Array *allocas)
{
        struct Instruction *alloca_inst;
        size_t alloca_iter = 0;
        struct BitMap visited_blocks;
        BitMapInit (&visited_blocks, MAX_BASIC_BLOCK_COUNT);

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
                                // check if block already as phi node for current alloca instruction, if so, skip
                                if (BlockHasPhiNodeForAlloca (curr_frontier_node, alloca_inst, &phi_node_mapping))
                                        continue;

                                struct Instruction *phi_inst = InsertPhiNode (curr_frontier_node);
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
        while ((curr_inst = BasicBlockInstructionIter (basic_block, &iter_count)) != NULL) {
                if (!INST_ISA (curr_inst, OPCODE_PHI)) {
                        continue;
                }

                struct Value *alloca_inst = hash_table_search (phi_node_mapping, AS_VALUE (curr_inst)->value_no);

                struct Value *top_value = SSAFrameSearch (frame, alloca_inst->value_no);

                if (!top_value) {
                        error (alloca_inst->token,
                               "Attempting to populate PHI node for alloca instruction, but value is potentially undefined.");
                        exit (EXIT_FAILURE);
                }

                Instruction_push_phi_operand_list (curr_inst, top_value, SSAFrameGetParentBlock (frame));
                SSAFrameInsert (frame, alloca_inst->value_no, curr_inst);
        }

        // we may need to revisit a block to insert Phi operands, so we only perform the visited check after inserting
        // the phi operands.
        if (BitMapIsSet (visited, basic_block->block_no)) {
                return;
        } else {
                BitMapSetBit (visited, basic_block->block_no);
        }

        while ((curr_inst = BasicBlockInstructionIter (basic_block, &iter_count)) != NULL) {
                if (INST_ISA (curr_inst, OPCODE_LOAD)) {
                        struct Value *load_from = Instruction_Load_From_Operand (curr_inst);

                        if (!VALUE_IS_INST (load_from) || !INST_ISA (AS_INST (load_from), OPCODE_ALLOCA)) {
                                error (load_from->token, "Expected alloca instruction target for load src operand!");
                                exit (EXIT_FAILURE);
                        }

                        Value_Replace_All_Uses_With (AS_VALUE (curr_inst), SSAFrameSearch (frame, load_from->value_no));
                } else if (INST_ISA (curr_inst, OPCODE_STORE)) {
                        struct Value *store_to = Instruction_Store_To_Operand (curr_inst);
                        struct Value *store_from = Instruction_Store_From_Operand (curr_inst);
                        SSAFrameInsert (frame, store_to->value_no, store_from);
                }
        }

        if (basic_block->left) {
                frame = SSAFramePush (frame, basic_block->left);
                Rename (basic_block->left, phi_node_mapping, frame, visited);
                frame = SSAFramePop (frame);
        }

        if (basic_block->right) {
                frame = SSAFramePush (frame, basic_block->right);
                Rename (basic_block->right, phi_node_mapping, frame, visited);
                frame = SSAFramePop (frame);
        }
}

static void RemoveMemoryInstructions (struct Function *function)
{
        struct Array mem_inst;
        Array_init (&mem_inst);

        struct Array traversal = postorder (function->entry_basic_block);

        struct BasicBlock *block;
        size_t iter_count = 0;
        while ((block = Array_iter (&traversal, &iter_count)) != NULL) {
                struct Instruction *inst;
                size_t inst_count = 0;

                while ((inst = BasicBlockInstructionIter (block, &inst_count)) != NULL) {
                        if (!INST_ISA (inst, OPCODE_STORE) && !INST_ISA (inst, OPCODE_LOAD) &&
                            !INST_ISA (inst, OPCODE_ALLOCA))
                                continue;

                        Array_push (&mem_inst, inst);
                }
        }

        Array_free (&traversal);

        struct Instruction *curr_mem_inst;

        while ((curr_mem_inst = Array_iter (&mem_inst, &iter_count)) != NULL) {
                Instruction_Remove_From_Parent (curr_mem_inst);

                Instruction_destroy (curr_mem_inst);
        }

        Array_free (&mem_inst);
}

void SSATranslation (struct Function *function)
{
        struct Array allocas = FindAllocas (function);

        struct HashTable phi_node_mapping = InsertPhiIntoBlocks (function, &allocas);

        struct SSAFrame *frame = SSAFrameCreate (function->entry_basic_block);

        struct BitMap visited;

        BitMapInit (&visited, MAX_BASIC_BLOCK_COUNT);

        Rename (function->entry_basic_block, &phi_node_mapping, frame, &visited);

        RemoveMemoryInstructions (function);
}
