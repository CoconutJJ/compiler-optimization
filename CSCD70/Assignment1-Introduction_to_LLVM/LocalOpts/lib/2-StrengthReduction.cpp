#include "LocalOpts.h"
#include "llvm/IR/Analysis.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <cstdint>
#include <vector>
#define IS_POW_OF_TWO(x) (((x) & (x - 1)) == 0)

using namespace llvm;

bool StrengthReductionPass::strengthReduction(llvm::Instruction &I) {

  if (!isa<BinaryOperator>(I)) {
    return false;
  }

  switch (I.getOpcode()) {

  case Instruction::Mul: {

    Value *FstOp = I.getOperand(0);
    Value *SndOp = I.getOperand(1);

    if (isa<ConstantInt>(SndOp)) {
      ConstantInt *SndConst = dyn_cast<ConstantInt>(SndOp);

      int64_t Val = SndConst->getSExtValue();

      if (IS_POW_OF_TWO(Val)) {

        int Exponent = 0;

        while (Val != 1) {
          Val >>= 1;
          Exponent++;
        }

        Value *ExponentValue =
            dyn_cast<Value>(ConstantInt::getSigned(FstOp->getType(), Exponent));

        IRBuilder<> Builder(&I);

        Value *NewInstruction = Builder.CreateShl(FstOp, ExponentValue);

        I.replaceAllUsesWith(NewInstruction);

        return true;
      }
    }

    return false;
  }
  case Instruction::SDiv: {

    Value *FstOp = I.getOperand(0);
    Value *SndOp = I.getOperand(1);
    if (isa<ConstantInt>(SndOp)) {
      ConstantInt *SndConst = dyn_cast<ConstantInt>(SndOp);

      int64_t Val = SndConst->getSExtValue();

      if (IS_POW_OF_TWO(Val)) {

        int Exponent = 0;

        while (Val != 1) {
          Val >>= 1;
          Exponent++;
        }

        Value *ExponentValue =
            dyn_cast<Value>(ConstantInt::getSigned(FstOp->getType(), Exponent));

        IRBuilder<> Builder(&I);

        Value *NewInstruction = Builder.CreateLShr(FstOp, ExponentValue);

        I.replaceAllUsesWith(NewInstruction);

        return true;
      }
    }
    return false;
  }
  default:
    return false;
  }
}

PreservedAnalyses StrengthReductionPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &) {
  bool Transformed = false;

  std::vector<Instruction *> RemovedInstructions;

  for (auto &Bb : F) {
    for (auto &I : Bb) {
      if (strengthReduction(I)) {
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
