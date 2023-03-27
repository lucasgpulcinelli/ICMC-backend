#ifndef LLVM_ICMC_ISEL_LOWERING_H
#define LLVM_ICMC_ISEL_LOWERING_H

#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class ICMCSubtarget;
class ICMCTargetMachine;

class ICMCTargetLowering : public TargetLowering {
public:
  explicit ICMCTargetLowering(const TargetMachine &TM,
                             const ICMCSubtarget &Subtarget);
};

} // end namespace llvm

#endif

