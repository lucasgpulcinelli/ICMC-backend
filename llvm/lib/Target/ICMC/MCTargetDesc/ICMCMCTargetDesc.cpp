#include "ICMCMCTargetDesc.h"
#include "ICMCAsmBackend.h"
#include "ICMCInstrPrinter.h"
#include "ICMCMCAsmInfo.h"
#include "ICMCMCCodeEmitter.h"
#include "TargetInfo/ICMCTargetInfo.h"

#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "ICMCGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "ICMCGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "ICMCGenRegisterInfo.inc"

static MCAsmInfo *createICMCMCAsmInfo(const MCRegisterInfo &MRI,
                                      const Triple &TT,
                                      const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new ICMCMCAsmInfo(TT);

  return MAI;
}

static MCInstrInfo *createICMCMCInstrInfo() {
  auto *X = new MCInstrInfo();
  InitICMCMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createICMCMCRegisterInfo(const Triple &TT) {
  auto *X = new MCRegisterInfo();
  InitICMCMCRegisterInfo(X, ICMC::R0);
  return X;
}

static MCSubtargetInfo *createICMCMCSubtargetInfo(const Triple &TT,
                                                  StringRef CPU, StringRef FS) {
  return createICMCMCSubtargetInfoImpl(TT, CPU, CPU, FS);
}

static MCInstPrinter *createICMCMCInstPrinter(const Triple &T,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI) {
  return new ICMCInstPrinter(MAI, MII, MRI);
}

static MCAsmBackend *createICMCAsmBackend(const Target &T,
                                          const MCSubtargetInfo &STI,
                                          const MCRegisterInfo &MRI,
                                          const llvm::MCTargetOptions &TO) {
  return new ICMCAsmBackend(STI.getTargetTriple().getOS());
}

// one of the initialization functions, gives information on how the
// architecture works in a low level: how instructions and registers are layed
// out, how many subtargets there are, how instructions are displayed, how the
// final object file is and how to generate the final binary code.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeICMCTargetMC() {
  Target &TheICMCTarget = getTheICMCTarget();
  RegisterMCAsmInfoFn X(TheICMCTarget, createICMCMCAsmInfo);

  TargetRegistry::RegisterMCInstrInfo(TheICMCTarget, createICMCMCInstrInfo);

  TargetRegistry::RegisterMCRegInfo(TheICMCTarget, createICMCMCRegisterInfo);

  TargetRegistry::RegisterMCSubtargetInfo(TheICMCTarget,
                                          createICMCMCSubtargetInfo);

  TargetRegistry::RegisterMCInstPrinter(TheICMCTarget, createICMCMCInstPrinter);

  TargetRegistry::RegisterMCAsmBackend(TheICMCTarget, createICMCAsmBackend);

  TargetRegistry::RegisterMCCodeEmitter(getTheICMCTarget(),
                                        createICMCMCCodeEmitter);
}
