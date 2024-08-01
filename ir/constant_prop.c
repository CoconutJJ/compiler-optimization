#include "constant_prop.h"
#include "basicblock.h"
#include "constant.h"
#include "dfa.h"
#include "function.h"
#include "global_constants.h"
#include "instruction.h"
#include "map.h"
#include "value.h"
#include <stdbool.h>
#include <stddef.h>

/**
        Sparse Simple Constant Propagation and Algebraic Identity Optimization

        We do not remove unused instructions as a result of the SSCP pass, instead
        we can just rely on the Deadcode Elimination pass to do this for us. 
*/
void SSCP (struct Function *function)
{
        struct BasicBlock *block = function->entry_block;

        struct Array preorder_traversal = postorder (block);

        struct BasicBlock *curr_block;
        size_t iter_count = 0;
        bool has_changes = false;

        do {
                has_changes = false;
                while ((curr_block = Array_iter (&preorder_traversal, &iter_count)) != NULL) {
                        struct Instruction *inst;
                        size_t inst_count = 0;

                        while ((inst = BasicBlockInstructionIter (curr_block, &inst_count))) {
                                if (Value_Use_count (AS_VALUE (inst)) == 0) {
                                        continue;
                                }

                                switch (inst->op_code) {
                                case OPCODE_ADD: {
                                        struct Value *fst = InstructionGetOperand (inst, 0),
                                                     *snd = InstructionGetOperand (inst, 1);

                                        if (VALUE_IS_CONST (fst) && VALUE_IS_CONST (snd)) {
                                                struct Constant *new_const = ConstantCreate (AS_CONST (fst)->constant +
                                                                                             AS_CONST (snd)->constant);

                                                Value_Replace_All_Uses_With (AS_VALUE (inst), AS_VALUE (new_const));

                                                has_changes = true;
                                        } else if (VALUE_IS_CONST (snd) && AS_CONST (snd)->constant == 0) {
                                                Value_Replace_All_Uses_With (AS_VALUE (inst), fst);

                                                has_changes = true;
                                        }
                                        break;
                                }
                                case OPCODE_SUB: {
                                        struct Value *fst = InstructionGetOperand (inst, 0),
                                                     *snd = InstructionGetOperand (inst, 1);

                                        if (fst == snd) {
                                                struct Constant *zero = ConstantCreate (0);
                                                Value_Replace_All_Uses_With (AS_VALUE (inst), AS_VALUE (zero));

                                                has_changes = true;

                                        } else if (VALUE_IS_CONST (fst) && VALUE_IS_CONST (snd)) {
                                                struct Constant *new_const = ConstantCreate (AS_CONST (fst)->constant -
                                                                                             AS_CONST (snd)->constant);

                                                Value_Replace_All_Uses_With (AS_VALUE (inst), AS_VALUE (new_const));

                                                has_changes = true;
                                        } else if (VALUE_IS_CONST (snd) && AS_CONST (snd)->constant == 0) {
                                                Value_Replace_All_Uses_With (AS_VALUE (inst), fst);

                                                has_changes = true;
                                        }

                                        break;
                                }
                                case OPCODE_MUL: {
                                        struct Value *fst = InstructionGetOperand (inst, 0),
                                                     *snd = InstructionGetOperand (inst, 1);

                                        if (VALUE_IS_CONST (fst) && VALUE_IS_CONST (snd)) {
                                                struct Constant *new_const = ConstantCreate (AS_CONST (fst)->constant *
                                                                                             AS_CONST (snd)->constant);

                                                Value_Replace_All_Uses_With (AS_VALUE (inst), AS_VALUE (new_const));

                                                has_changes = true;
                                        }

                                        break;
                                }
                                case OPCODE_DIV: {
                                        struct Value *fst = InstructionGetOperand (inst, 0),
                                                     *snd = InstructionGetOperand (inst, 1);

                                        if (VALUE_IS_CONST (fst) && VALUE_IS_CONST (snd)) {
                                                struct Constant *new_const = ConstantCreate (AS_CONST (fst)->constant /
                                                                                             AS_CONST (snd)->constant);

                                                Value_Replace_All_Uses_With (AS_VALUE (inst), AS_VALUE (new_const));

                                                has_changes = true;
                                        }

                                        break;
                                }
                                case OPCODE_PHI: {
                                        size_t arg_count = InstructionGetOperandCount (inst);

                                        bool is_constant = true;

                                        int constant_value;

                                        for (size_t i = 0; i < arg_count; i++) {
                                                struct Value *op = InstructionGetOperand (inst, i);

                                                if (!VALUE_IS_CONST (op)) {
                                                        is_constant = false;
                                                        break;
                                                }

                                                if (i == 0) {
                                                        constant_value = AS_CONST (op)->constant;
                                                        continue;
                                                }

                                                if (constant_value != AS_CONST (op)->constant) {
                                                        is_constant = false;
                                                        break;
                                                }
                                        }

                                        if (is_constant) {
                                                Value_Replace_All_Uses_With (
                                                        AS_VALUE (inst), AS_VALUE (ConstantCreate (constant_value)));

                                                has_changes = true;
                                        }

                                        break;
                                }
                                case OPCODE_CMP: break;
                                case OPCODE_JUMP: break;
                                case OPCODE_JUMPIF: break;
                                case OPCODE_NIL: break;
                                case OPCODE_RET: break;
                                case OPCODE_STORE: break;
                                case OPCODE_LOAD: break;
                                case OPCODE_ALLOCA: break;
                                }
                        }
                }
        } while (has_changes);
}
