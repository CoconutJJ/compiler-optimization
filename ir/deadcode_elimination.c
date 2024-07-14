

#include "deadcode_elimination.h"
#include "array.h"
#include "basicblock.h"
#include "dfa.h"
#include "function.h"
#include "global_constants.h"
#include "instruction.h"
#include "value.h"
#include <stdbool.h>

void RemoveDeadCode (struct Function *function)
{
        struct Array preorder_traversal = postorder (function->entry_basic_block);
        bool has_changes;
        struct Array unused_instructions;
        Array_init (&unused_instructions);
        do {
                struct BasicBlock *curr_block;
                size_t iter_count = 0;
                has_changes = false;
                while ((curr_block = Array_iter (&preorder_traversal, &iter_count)) != NULL) {
                        struct Instruction *inst;
                        size_t inst_count = 0;

                        while ((inst = BasicBlockInstructionIter (curr_block, &inst_count)) != NULL) {
                                if (INST_ISA (inst, OPCODE_JUMP) || INST_ISA (inst, OPCODE_JUMPIF) ||
                                    INST_ISA (inst, OPCODE_RET))
                                        continue;

                                size_t use_count = Value_Use_count (AS_VALUE (inst));

                                if (use_count > 0)
                                        continue;

                                Array_push (&unused_instructions, inst);
                        }
                }

                if (Array_length (&unused_instructions) > 0)
                        has_changes = true;

                size_t unused_count = 0;
                struct Instruction *curr_unused;
                while ((curr_unused = Array_iter (&unused_instructions, &unused_count)) != NULL) {
                        Instruction_Remove_From_Parent (curr_unused);
                        Instruction_destroy (curr_unused);
                }

                Array_empty (&unused_instructions);
        } while (has_changes);
}
