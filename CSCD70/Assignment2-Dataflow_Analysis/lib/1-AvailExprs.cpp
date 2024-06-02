#include "DFA.h"
#include "DFA/MeetOp.h"

using namespace llvm;

AnalysisKey AvailExprs::Key;

bool AvailExprs::transferFunc(const Instruction &Inst, const DomainVal_t &IDV,
                             DomainVal_t &ODV) {
  
  
  return false;
}
