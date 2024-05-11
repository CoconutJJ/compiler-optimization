#include "LocalOpts.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include <sys/_types/_int64_t.h>

using namespace llvm;

PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &) {

  /// @todo(CSCD70) Please complete this method.

  for (auto &BB : F) {
    for (auto &Inst : BB) {
      Value *Op1 = Inst.getOperand(0);
      Value *Op2 = Inst.getOperand(1);

      int64_t Op1Const = -1, Op2Const = -1;
      if (ConstantInt *C1 = dyn_cast<ConstantInt>(Op1)) {
        Op1Const = C1->getSExtValue();
      }

      if (ConstantInt *C2 = dyn_cast<ConstantInt>(Op2)) {
        Op2Const = C2->getSExtValue();
      }
      switch (Inst.getOpcode()) {
      case Instruction::Add:
        
        if (Op1Const == 0) {
          Inst.replaceAllUsesWith(Op2);
        } else if (Op2Const == 0) {
          Inst.replaceAllUsesWith(Op1);
        } else {
            
        }

        break;
      case Instruction::Mul:
        break;
      case Instruction::Sub:
        break;
      case Instruction::SDiv:
        break;
      }
    }
  }

  return PreservedAnalyses::none();
}
