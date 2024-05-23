#include "LocalOpts.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <cstdint>

#define IS_POW_OF_TWO(x) (((x) & (x - 1)) == 0)

using namespace llvm;

bool StrengthReductionPass::strengthReduction(llvm::Instruction &I) {
  switch (I.getOpcode()) {

  case Instruction::Mul: {

    Value *FstOp = I.getOperand(0);
    Value *SndOp = I.getOperand(1);

    if (isa<ConstantInt>(SndOp)) {
      ConstantInt *SndConst = dyn_cast<ConstantInt>(SndOp);

      int64_t Val = SndConst->getSExtValue();

      if (IS_POW_OF_TWO(Val) == 0LL) {

        int Exponent = 0;

        while (Val != 0) {
          Val >>= 1;
          Exponent++;
        }

        Value *ExponentValue = dyn_cast<Value>(
            ConstantInt::getSigned(SndConst->getType(), Exponent));

        Instruction *NewInstruction =
            BinaryOperator::CreateShl(FstOp, ExponentValue);

        BasicBlock::iterator InstIter(I);
        ReplaceInstWithInst(I.getParent(), InstIter, NewInstruction);

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

      if (IS_POW_OF_TWO(Val) == 0LL) {

        int Exponent = 0;

        while (Val != 0) {
          Val >>= 1;
          Exponent++;
        }

        Value *ExponentValue = dyn_cast<Value>(
            ConstantInt::getSigned(SndConst->getType(), Exponent));

        Instruction *NewInstruction =
            BinaryOperator::CreateLShr(FstOp, ExponentValue);

        BasicBlock::iterator InstIter(I);
        ReplaceInstWithInst(I.getParent(), InstIter, NewInstruction);
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
  for (auto &Bb : F) {
    for (auto &I : Bb) {
        Transformed = Transformed || strengthReduction(I);
    }
  }
  return PreservedAnalyses::none();
}
