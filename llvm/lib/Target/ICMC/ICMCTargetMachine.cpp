#include "ICMCTargetMachine.h"
#include "ICMCFrameLowering.h"
#include "ICMCISelDAGToDAG.h"
#include "ICMCTargetObjectFile.h"
#include "TargetInfo/ICMCTargetInfo.h"

#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

// the data layout of the architecure: shows alignment info for each type
static const char *ICMCDataLayout =
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
  this->TLOF = std::make_unique<ICMCTargetObjectFile>();
  initAsmInfo();
}

namespace {
/*
 * ICMCPassConfig: defines the steps for compilation of the target up to the
 * MCInst layer
 */
class ICMCPassConfig : public TargetPassConfig {
public:
  ICMCPassConfig(ICMCTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  ICMCTargetMachine &getICMCTargetMachine() const {
    return getTM<ICMCTargetMachine>();
  }

  bool addInstSelector() override;
  void addPreSched2() override;
};
} // end namespace

TargetPassConfig *ICMCTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new ICMCPassConfig(*this, PM);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeICMCTarget() {
  RegisterTargetMachine<ICMCTargetMachine> X(getTheICMCTarget());

  auto &PR = *PassRegistry::getPassRegistry();
  initializeICMCExpandPseudoPass(PR);
}

bool ICMCPassConfig::addInstSelector() {
  addPass(createICMCISelDag(getICMCTargetMachine(), getOptLevel()));
  addPass(createICMCFrameAnalyzerPass());
  return false;
}

void ICMCPassConfig::addPreSched2() { addPass(createICMCExpandPseudoPass()); }
