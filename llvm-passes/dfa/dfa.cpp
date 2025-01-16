#include "dfa.h"
#include "llvm/IR/BasicBlock.h"
#include <llvm/ADT/BitVector.h>
#include <llvm/ADT/SmallSet.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <stack>
#include <vector>
using namespace llvm;

std::vector<BasicBlock *> DataFlowAnalysisFramework::computePO (Function &F)
{
        SmallSet<BasicBlock *, 20> visited;

        std::stack<BasicBlock *> stack;
        std::vector<BasicBlock *> po;
        stack.push (&*(F.begin ()));

        while (stack.size () > 0) {
                auto *BB = stack.top ();

                bool visited_all_successors = true;

                for (auto *succBB : successors (BB)) {
                        if (visited.contains (succBB))
                                continue;

                        stack.push (succBB);
                        visited.insert (succBB);
                        visited_all_successors = false;
                        break;
                }

                if (visited_all_successors) {
                        po.push_back (BB);
                        stack.pop ();
                }
        }

        return po;
}

BitVector DataFlowAnalysisFramework::computeMeet (std::set<BitVector> operands)
{
        BitVector vec = *operands.begin ();

        for (auto bv_iter = std::next (operands.begin (), 1); bv_iter != operands.end (); bv_iter++) {
                BitVector bv = *bv_iter;

                switch (MeetOperator) {
                case INTERSECTION: vec &= bv; break;
                case UNION: vec |= bv; break;
                default: fprintf (stderr, "Bad MeetOperator Type!"); break;
                }
        }

        return vec;
}

void DataFlowAnalysisFramework::computeDomainMap (Function &F)
{
        int bb_no = 0;
        for (auto &BB : F) {
                DomainMap[&BB] = bb_no;
                RevDomainMap[bb_no] = &BB;
        }
}


