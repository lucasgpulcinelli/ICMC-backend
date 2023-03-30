#include "ICMCTargetMachine.h"
#include "TargetInfo/ICMCTargetInfo.h"
#include "ICMCISelDAGToDAG.h"

#include "llvm/Target/TargetMachine.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/CodeGen/TargetPassConfig.h"

using namespace llvm;

static const char* ICMCDataLayout = \
      "e-P1-p:16:16-i8:16-i16:16-i32:16-i64:16-f32:16-f64:16-n8-a:16";

ICMCTargetMachine::ICMCTargetMachine(const Target &T, const Triple &TT,
                                     StringRef CPU, StringRef FS,
                                     const TargetOptions &Options,
                                     Optional<Reloc::Model> RM,
                                     Optional<CodeModel::Model> CM,
                                     CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, ICMCDataLayout, TT, CPU, FS, Options,
                          RM.value_or(Reloc::Static),
                          getEffectiveCodeModel(CM, CodeModel::Small), OL),
      Subtarget(TT, std::string(CPU), std::string(FS), *this) {
  initAsmInfo();
}


namespace {
class ICMCPassConfig : public TargetPassConfig {
public:
  ICMCPassConfig(ICMCTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  ICMCTargetMachine &getICMCTargetMachine() const {
    return getTM<ICMCTargetMachine>();
  }

  bool addInstSelector() override;
};
} // namespace

TargetPassConfig *ICMCTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new ICMCPassConfig(*this, PM);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeICMCTarget(){
  RegisterTargetMachine<ICMCTargetMachine> X(getTheICMCTarget());
}

bool ICMCPassConfig::addInstSelector() {
  addPass(createICMCISelDag(getICMCTargetMachine(), getOptLevel()));
  return false;
}

