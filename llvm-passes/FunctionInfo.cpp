#include <llvm/IR/Analysis.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>
using namespace llvm;

class FunctionInfoPass final : public PassInfoMixin<FunctionInfoPass> {
    public:
        PreservedAnalyses run ([[maybe_unused]] Module &M, ModuleAnalysisManager &)
        {

                for (auto &F : M.functions ()) {
                        errs () << F.getName () << "\n";

                        printf ("Function Name: %s\n", F.getName ().str ().c_str ());
                        printf ("Number of arguments: %lu\n", F.arg_size ());

                        size_t basic_block_count = 0;
                        size_t instruction_count = 0;
                        for (Function::iterator b = F.begin (), b_end = F.end (); b != b_end; b++) {
                                BasicBlock *bb = &*b;

                                basic_block_count++;

                                for (BasicBlock::iterator inst_iter = bb->begin (), bb_end = bb->end ();
                                     inst_iter != bb_end;
                                     inst_iter++) {
                                        Instruction *inst = &*inst_iter;

                                        instruction_count++;
                                }
                        }

                        printf ("Number of Basic Blocks: %lu\n", basic_block_count);
                }

                return PreservedAnalyses::all ();
        }
};

PassPluginLibraryInfo getFunctionInfoPassPluginInfo ()
{
        const auto callback = [] (PassBuilder &PB) {
                PB.registerPipelineParsingCallback ([&] (StringRef Name, ModulePassManager &MPM, auto) {
                        if (Name == "function-info") {
                                MPM.addPass (FunctionInfoPass ());
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
