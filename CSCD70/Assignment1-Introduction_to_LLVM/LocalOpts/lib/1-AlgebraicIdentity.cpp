#include <vector>
#include "LocalOpts.h"
#include "llvm/IR/Analysis.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"

using namespace llvm;

bool AlgebraicIdentityPass::algebraicIdentity(llvm::Instruction &I) {
  
  if (I.getNumOperands() < 2) {
    return false;
  }
  
  Value *Op1 = I.getOperand(0);
  Value *Op2 = I.getOperand(1);

  int64_t Op1Const = -1, Op2Const = -1;
  if (ConstantInt *C1 = dyn_cast<ConstantInt>(Op1)) {
    Op1Const = C1->getSExtValue();
  }

  if (ConstantInt *C2 = dyn_cast<ConstantInt>(Op2)) {
    Op2Const = C2->getSExtValue();
  }
  switch (I.getOpcode()) {
  case Instruction::Add:
    // if both are constants, just compute the constant value
    if (isa<ConstantInt>(Op1) && isa<ConstantInt>(Op2)) {
      I.replaceAllUsesWith(
          ConstantInt::getSigned(I.getType(), Op1Const + Op2Const));

    } else if (isa<ConstantInt>(Op1) && Op1Const == 0) {
      I.replaceAllUsesWith(Op2);

    } else if (isa<ConstantInt>(Op2) && Op2Const == 0) {
      I.replaceAllUsesWith(Op1);

    } else {
      return false;
    }
    break;
  case Instruction::Mul:

    if (isa<ConstantInt>(Op1) && isa<ConstantInt>(Op2)) {
      I.replaceAllUsesWith(
          ConstantInt::getSigned(I.getType(), Op1Const * Op2Const));

    } else if (isa<ConstantInt>(Op1)) {

      if (Op1Const == 0) {
        I.replaceAllUsesWith(ConstantInt::getSigned(I.getType(), 0));

      } else if (Op1Const == 1) {
        I.replaceAllUsesWith(ConstantInt::getSigned(I.getType(), Op2Const));

      } else {
        return false;
      }
    } else if (isa<ConstantInt>(Op2) && Op2Const == 1) {
      if (Op2Const == 0) {
        I.replaceAllUsesWith(ConstantInt::getSigned(I.getType(), 0));

      } else if (Op2Const == 1) {
        I.replaceAllUsesWith(ConstantInt::getSigned(I.getType(), Op1Const));

      } else {
        return false;
      }
    } else {
      return false;
    }

    break;
  case Instruction::Sub:
    if (isa<ConstantInt>(Op1) && isa<ConstantInt>(Op2)) {
      I.replaceAllUsesWith(
          ConstantInt::getSigned(I.getType(), Op1Const - Op2Const));

    } else if (isa<ConstantInt>(Op2) && Op2Const == 0) {
      I.replaceAllUsesWith(Op1);

    } else {
      return false;
    }

    break;
  case Instruction::SDiv:

    if (isa<ConstantInt>(Op2) && Op2Const == 1) {
      I.replaceAllUsesWith(Op1);

    } else {
      return false;
    }

    break;
  default:
    return false;
  }
  return true;
}

PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &) {
  bool Transformed = false;
  
  std::vector<Instruction *> RemovedInstructions;
  
  for (auto &BB : F) {
    for (auto &I : BB) {
      if (algebraicIdentity(I)) {
        Transformed = true;
        RemovedInstructions.push_back(&I);
      }
    }
  }

  for (auto *I : RemovedInstructions) {
    I->eraseFromParent();
  }

  return Transformed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}
