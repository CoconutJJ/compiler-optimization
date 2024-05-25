#pragma once // NOLINT(llvm-header-guard)

#include "llvm/IR/Instruction.h"
#include <cstdint>
#include <llvm/IR/PassManager.h>

class AlgebraicIdentityPass final
    : public llvm::PassInfoMixin<AlgebraicIdentityPass> {
public:

  bool algebraicIdentity(llvm::Instruction &I);
  llvm::PreservedAnalyses run(llvm::Function &,
                              llvm::FunctionAnalysisManager &);
}; // class AlgebraicIdentityPass

class StrengthReductionPass final
    : public llvm::PassInfoMixin<StrengthReductionPass> {
public:
  int computePowerOfTwo(int64_t value);
  bool strengthReduction(llvm::Instruction &I);
  llvm::PreservedAnalyses run(llvm::Function &,
                              llvm::FunctionAnalysisManager &);
}; // class StrengthReductionPass

class MultiInstOptPass final : public llvm::PassInfoMixin<MultiInstOptPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &,
                              llvm::FunctionAnalysisManager &);
}; // class MultiInstOptPass
