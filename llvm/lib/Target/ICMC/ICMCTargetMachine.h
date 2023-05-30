#ifndef LLVM_ICMC_TARGET_MACHINE_H
#define LLVM_ICMC_TARGET_MACHINE_H

#include "ICMCSubtarget.h"

#include "llvm/Target/TargetMachine.h"

namespace llvm {

void initializeICMCExpandPseudoPass(PassRegistry &);
FunctionPass *createICMCExpandPseudoPass();

class TargetPassConfig;

/*
 * ICMCTargetMachine: defines how IR is converted to object file. The driver of
 * the compilation process.
 */
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

  // defines all passes necessary for compilation
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return this->TLOF.get();
  }
};

} // end namespace llvm

#endif
