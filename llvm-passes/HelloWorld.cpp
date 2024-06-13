

#include "llvm/IR/Analysis.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"

#include "llvm/Support/raw_ostream.h"
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

using namespace llvm;

class HelloWorldPass : public llvm::PassInfoMixin<HelloWorldPass> {
    public:
        llvm::PreservedAnalyses run (llvm::Function &F, llvm::FunctionAnalysisManager &)
        {
                for (auto &BB : F) {
                        for (auto &I : BB) {
                                if (!llvm::isa<llvm::Argument> (I))
                                        continue;
                                
                                errs() << I;
                                // I.setOperand (0, llvm::ConstantInt::getSigned (I.getOperand (0)->getType (), 0));
                        }
                }

                return llvm::PreservedAnalyses::none();
        };
};


PassPluginLibraryInfo getFunctionInfoPassPluginInfo ()
{
        const auto callback = [] (PassBuilder &PB) {
                PB.registerPipelineParsingCallback ([&] (StringRef Name, FunctionPassManager &FPM, auto) {
                        if (Name == "hello-world") {
                                FPM.addPass (HelloWorldPass ());
                                return true;
                        }
                        return false;
                });
        };

        return { LLVM_PLUGIN_API_VERSION, "FunctionInfoPass", "0.0.1", callback };
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo ()
{
        return getFunctionInfoPassPluginInfo ();
}
