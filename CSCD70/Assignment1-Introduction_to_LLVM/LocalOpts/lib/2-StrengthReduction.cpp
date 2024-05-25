#include "LocalOpts.h"
#include "llvm/IR/Analysis.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/FMF.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <cassert>
#include <cstdint>
#include <vector>
#define IS_POW_OF_TWO(x) (((x) & (x - 1)) == 0)

using namespace llvm;

int StrengthReductionPass::computePowerOfTwo(int64_t Value) {

  int Exponent = 0;

  while (Value != 1) {
    Value >>= 1;
    Exponent++;
  }

  return Exponent;
}

bool StrengthReductionPass::strengthReduction(llvm::Instruction &I) {

  if (!isa<BinaryOperator>(I)) {
    return false;
  }

  if (I.getOpcode() != Instruction::Mul && I.getOpcode() != Instruction::SDiv) {
    return false;
  }

  Value *FstOp = I.getOperand(0);
  Value *SndOp = I.getOperand(1);

  ConstantInt *Scalar;

  if (isa<ConstantInt>(FstOp) && I.getOpcode() == Instruction::Mul) {
    Scalar = dyn_cast<ConstantInt>(FstOp);
  } else if (isa<ConstantInt>(SndOp)) {
    Scalar = dyn_cast<ConstantInt>(SndOp);
  } else {
    return false;
  }

  int64_t Val = Scalar->getSExtValue();

  if (!IS_POW_OF_TWO(Val))
    return false;

  Value *ExponentValue = dyn_cast<Value>(
      ConstantInt::getSigned(FstOp->getType(), this->computePowerOfTwo(Val)));

  IRBuilder<> Builder(&I);

  Value *NewInst;

  switch (I.getOpcode()) {
  case Instruction::Mul: {
    NewInst = Builder.CreateShl(FstOp, ExponentValue);
    break;
  }
  case Instruction::SDiv: {
    NewInst = Builder.CreateLShr(FstOp, ExponentValue);
    break;
  }
  default:
    assert(0 && "unreachable");
    return false;
  }

  I.replaceAllUsesWith(NewInst);
  return true;
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
