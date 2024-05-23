#include "LocalOpts.h"

using namespace llvm;

PreservedAnalyses MultiInstOptPass::run([[maybe_unused]] Function &F,
                                        FunctionAnalysisManager &) {

  for (auto &Bb : F) {
    for (auto &Inst : Bb) {

      

    }
  }
  return PreservedAnalyses::none();
}
