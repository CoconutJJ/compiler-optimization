#include "llvm/IR/Analysis.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

using namespace llvm;

class HelloWorldPass : public llvm::PassInfoMixin<HelloWorldPass> {
    public:
        llvm::PreservedAnalyses run (llvm::Function &F, llvm::FunctionAnalysisManager &)
        {
                for (auto &BB : F) {
                        for (auto &I : BB) {
                                
                        }
                }

                return llvm::PreservedAnalyses::none ();
        }

        static bool isRequired ()
        {
                return true;
        }
};

PassPluginLibraryInfo getHelloWorldPassPluginInfo ()
{
        const auto callback = [] (PassBuilder &PB) {
                PB.registerPipelineParsingCallback ([&] (StringRef Name, FunctionPassManager &FPM, auto) {
                        if (Name == "hello-world-pass") {
                                FPM.addPass (HelloWorldPass ());
                                return true;
                        }
                        return false;
                });
        };

        return { LLVM_PLUGIN_API_VERSION, "HelloWorldPass", "0.0.1", callback };
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo ()
{
        return getHelloWorldPassPluginInfo ();
}
