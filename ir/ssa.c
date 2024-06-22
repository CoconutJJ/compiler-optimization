#include "array.h"
#include "basicblock.h"
#include "dfa.h"
#include "function.h"
#include "instruction.h"

/*

                alloca %3, 4
                load %4, %3
add %5, %4, 1                   add %6, %4, 1
store %3, %5                    store %3, %6

                load %7, %3

*/

struct Array find_allocas (struct Function *function)
{
        struct Array postorder_traversal = postorder (function->entry_basic_block);

        struct BasicBlock *block;
        size_t iter_count = 0;

        while ((block = Array_iter (&postorder_traversal, &iter_count)) != NULL) {
                size_t inst_iter = 0;
                struct Instruction *inst;

                while ((inst = BasicBlock_Instruction_iter (block, &inst_iter)) != NULL) {
                        if (!INST_ISA (inst, OPCODE_ALLOCA)) {
                                continue;
                        }
                }
        }
}
