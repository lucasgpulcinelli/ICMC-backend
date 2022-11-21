#ifndef LLVM_ICMC_TARGET_MACHINE_H
#define LLVM_ICMC_TARGET_MACHINE_H

#include "llvm/Target/TargetMachine.h"

using namespace llvm;

class ICMCTargetMachine : public LLVMTargetMachine {
public:
    ICMCTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                      StringRef FS, const TargetOptions &Options,
                      Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                      CodeGenOpt::Level OL, bool JIT);
};

#endif

