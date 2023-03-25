#ifndef LLVM_ICMC_ISEL_DAGTODAG_H
#define LLVM_ICMC_ISEL_DAGTODAG_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {

class FunctionPass;
class ICMCTargetMachine;

FunctionPass *createICMCISelDag(ICMCTargetMachine &TM,
                               CodeGenOpt::Level OptLevel);

} // end namespace llvm

#endif

