#include "deadcode_elimination.h"
#include "array.h"
#include "basicblock.h"
#include "dfa.h"
#include "dominators.h"
#include "function.h"
#include "global_constants.h"
#include "instruction.h"
#include "lexer.h"
#include "map.h"
#include "value.h"
#include <stdatomic.h>
#include <stdbool.h>

static void
MarkCriticalInstructions (struct Function *function, struct HashTable *postdom_frontiers, bool critical_instructions[])
{
        struct Array worklist;
        Array_init (&worklist);

        struct Array preorder_traversal = postorder (function->entry_block);
        struct BasicBlock *curr_block;
        size_t iter_count = 0;

        // mark all the critical instructions, so far the only critical instruction is the ret instruction
        while ((curr_block = Array_iter (&preorder_traversal, &iter_count)) != NULL) {
                struct Instruction *inst;
                size_t inst_count = 0;

                while ((inst = BasicBlockInstructionIter (curr_block, &inst_count)) != NULL) {
                        if (INST_ISA (inst, OPCODE_RET)) {
                                critical_instructions[VALUE_NO (AS_VALUE (inst))] = true;
                                Array_push (&worklist, inst);
                        }
                }
        }

        struct Instruction *curr_inst;

        while ((curr_inst = Array_iter (&worklist, &iter_count)) != NULL) {
                size_t arg_count = InstructionGetOperandCount (curr_inst);

                // mark the operands as critical
                for (size_t i = 0; i < arg_count; i++) {
                        struct Value *operand = InstructionGetOperand (curr_inst, i);

                        // function arguments and constants don't matter
                        if (VALUE_IS_ARG (operand) || VALUE_IS_CONST (operand))
                                continue;

                        if (critical_instructions[VALUE_NO (operand)])
                                continue;

                        critical_instructions[VALUE_NO (operand)] = true;
                        Array_push (&worklist, operand);
                }

                struct BasicBlock *parent_block = curr_inst->parent;

                struct Array *frontier_nodes = hash_table_search_ptr (postdom_frontiers, parent_block->block_no);

                if (!frontier_nodes)
                        continue;

                struct BasicBlock *curr_frontier_block;
                size_t frontier_iter = 0;

                // mark jump instruction in the postdominance frontier blocks as critical as well.
                while ((curr_frontier_block = Array_iter (frontier_nodes, &frontier_iter)) != NULL) {
                        struct Instruction *last_instruction = BasicBlockLastInstruction (curr_frontier_block);

                        if (INST_ISA (last_instruction, OPCODE_JUMP) || INST_ISA (last_instruction, OPCODE_JUMPIF)) {
                                if (critical_instructions[VALUE_NO (AS_VALUE (last_instruction))])
                                        continue;

                                critical_instructions[VALUE_NO (AS_VALUE (last_instruction))] = true;
                                Array_push (&worklist, last_instruction);
                        }
                }
        }

        Array_free (&preorder_traversal);
}

static void Sweep (struct Function *function, struct HashTable *postdom_tree_transpose, bool critical_instructions[])
{
        struct Array postorder_traversal = postorder (function->entry_block);

        struct BasicBlock *curr_block;
        size_t iter_count = 0;

        struct Array useless_instructions;
        Array_init (&useless_instructions);

        while ((curr_block = Array_iter (&postorder_traversal, &iter_count)) != NULL) {
                struct Instruction *inst;
                size_t inst_iter = 0;
                while ((inst = BasicBlockInstructionIter (curr_block, &inst_iter)) != NULL) {
                        if (critical_instructions[VALUE_NO (AS_VALUE (inst))])
                                continue;

                        if (INST_ISA (inst, OPCODE_JUMP) || INST_ISA (inst, OPCODE_JUMPIF)) {
                                // rewrite jump to immediate post dominator

                        } else {
                                Array_push (&useless_instructions, inst);
                        }
                }
        }

        struct Instruction *curr_instruction;
        while ((curr_instruction = Array_iter (&useless_instructions, &iter_count)) != NULL) {
                Instruction_Remove_From_Parent (curr_instruction);
                Instruction_destroy (curr_instruction);
        }

        Array_free (&useless_instructions);
        Array_free (&postorder_traversal);
}

void RemoveDeadCode (struct Function *function)
{
        struct DFAConfiguration config = PostDominatorDFAConfiguration (function);
        RunDFA (&config, function);

        struct HashTable postdom_tree = ComputePostDominatorTree (function, &config);
        struct HashTable postdom_frontiers = ComputePostDominanceFrontier (function, &postdom_tree);

        struct HashTable postdom_tree_transpose = ComputeTranspose (function, &postdom_tree);

        bool critical_instructions[MAX_VALUE_NO] = { 0 };
        MarkCriticalInstructions (function, &postdom_frontiers, critical_instructions);
        Sweep (function, &postdom_tree_transpose, critical_instructions);
}
