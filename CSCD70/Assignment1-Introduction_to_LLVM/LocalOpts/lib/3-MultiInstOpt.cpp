#include "LocalOpts.h"
#include "llvm/IR/Analysis.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include <cstdlib>
#include <vector>
using namespace llvm;

bool MultiInstOptPass::hasNegOperand (llvm::Instruction *inst, llvm::Value *op)
{
        if (!isa<BinaryOperator> (*inst))
                return false;

        if (inst->getOpcode () != Instruction::Sub)
                return false;

        Value *SndOp = inst->getOperand (1);

        return SndOp == op;
}

bool MultiInstOptPass::hasPosOperand (llvm::Instruction *inst, llvm::Value *op)
{
        if (!isa<BinaryOperator> (inst))
                return false;

        if (inst->getOpcode () == Instruction::Add) {
                return inst->getOperand (0) == op || inst->getOperand (1) == op;
        } else if (inst->getOpcode () == Instruction::Sub) {
                return inst->getOperand (0) == op;
        }

        return false;
}

bool MultiInstOptPass::multiInstOpt (llvm::Instruction &I)
{
        if (!isa<BinaryOperator> (I))
                return false;

        Value *FstOp = I.getOperand (0);
        Value *SndOp = I.getOperand (1);

        if (!isa<BinaryOperator> (FstOp) && !isa<BinaryOperator> (SndOp)) {
                return false;
        }

        Instruction *FstInst = dyn_cast<Instruction> (FstOp);

        Instruction *SndInst = dyn_cast<Instruction> (SndOp);

        // Two cases for cancellation: c1 = a + b, c2 = a - b

        // c1 - a = b
        // c1 - b = a
        // a - c2 = b

        // c2 + b = a
        // b + c2 = a

        switch (I.getOpcode ()) {
        case Instruction::Add:
                // c2 + b
                if (this->hasNegOperand (FstInst, SndOp)) {
                        Value *collapsedValue = FstInst->getOperand (0);

                        I.replaceAllUsesWith (collapsedValue);

                        return true;
                        // b + c2
                } else if (this->hasNegOperand (SndInst, FstOp)) {
                        Value *collapsedValue = SndInst->getOperand (0);

                        I.replaceAllUsesWith (collapsedValue);

                        return true;
                } else {
                        return false;
                }

                break;
        case Instruction::Sub:

                if (this->hasPosOperand (FstInst, SndOp)) {
                        Value *a = FstInst->getOperand (0);
                        Value *b = FstInst->getOperand (1);

                        // c1 - a
                        if (SndOp == a) {
                                I.replaceAllUsesWith (b);
                        // c1 - b
                        } else if (SndOp == b) {
                                I.replaceAllUsesWith (a);
                        } else {
                                fprintf (stderr, "unreachable!");

                                exit (EXIT_FAILURE);
                        }

                        return true;

                } else {
                        // a - c2
                        if (!isa<BinaryOperator> (SndInst))
                                return false;

                        if (SndInst->getOpcode () != Instruction::Sub)
                                return false;

                        if (!this->hasPosOperand (SndInst, FstOp))
                                return false;

                        I.replaceAllUsesWith (SndInst->getOperand (1));

                        return true;
                }

                break;
        }

        return false;
}

PreservedAnalyses MultiInstOptPass::run ([[maybe_unused]] Function &F, FunctionAnalysisManager &)
{
        bool Transformed = false;

        std::vector<Instruction *> RemovedInstructions;

        for (auto &Bb : F) {
                for (auto &Inst : Bb) {
                        if (multiInstOpt (Inst)) {
                                Transformed = true;
                                RemovedInstructions.push_back (&Inst);
                        }
                }
        }

        for (auto *I : RemovedInstructions) {
                I->eraseFromParent ();
        }

        return Transformed ? PreservedAnalyses::none () : PreservedAnalyses::all ();
}
