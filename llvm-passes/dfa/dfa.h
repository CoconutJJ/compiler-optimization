#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"
#include <map>
#include <vector>

using namespace llvm;

class DataFlowAnalysisFramework {

    public:
        enum MeetOperator {
            UNION,
            INTERSECTION
        };

        enum MeetOperator MeetOperator;
        std::map<BasicBlock*, int> DomainMap;
        std::map<int, BasicBlock*> RevDomainMap;

        std::vector<BasicBlock*> computePO(Function &F);
        BitVector computeMeet(std::set<BitVector> operands);
        
        void computeDomainMap(Function &F);

        BitVector computeTransferFunction(BitVector bv);







};