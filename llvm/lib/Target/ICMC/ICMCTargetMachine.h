#ifndef LLVM_ICMC_TARGET_MACHINE_H
#define LLVM_ICMC_TARGET_MACHINE_H

#include "ICMCSubtarget.h"

#include "llvm/Target/TargetMachine.h"

namespace llvm {

class TargetPassConfig;

class ICMCTargetMachine : public LLVMTargetMachine {
private:
  ICMCSubtarget Subtarget;
  std::unique_ptr<TargetLoweringObjectFile> TLOF;

public:
  ICMCTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                    StringRef FS, const TargetOptions &Options,
                    Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                    CodeGenOpt::Level OL, bool JIT);

  const ICMCSubtarget *getSubtargetImpl() const { return &Subtarget; }
  const ICMCSubtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return this->TLOF.get();
  }

};

} // end namespace llvm

#endif

