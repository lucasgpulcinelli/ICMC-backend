#include "ICMCMCTargetDesc.h"
#include "ICMCMCAsmInfo.h"
#include "ICMCTargetStreamer.h"
#include "ICMCInstrPrinter.h"
#include "TargetInfo/ICMCTargetInfo.h"
#include "ICMCAsmBackend.h"
#include "ICMCMCCodeEmitter.h"

#include "llvm/MC/TargetRegistry.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"

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

static MCTargetStreamer *createTargetAsmStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &OS,
                                                 MCInstPrinter *InstPrint,
                                                 bool isVerboseAsm) {
  return new ICMCTargetStreamer(S);
}

MCAsmBackend *createICMCAsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                  const MCRegisterInfo &MRI,
                                  const llvm::MCTargetOptions &TO) {
  return new ICMCAsmBackend(STI.getTargetTriple().getOS());
}



extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeICMCTargetMC() {
  Target &TheICMCTarget = getTheICMCTarget();
  RegisterMCAsmInfoFn X(TheICMCTarget, createICMCMCAsmInfo);

  TargetRegistry::RegisterMCInstrInfo(TheICMCTarget, createICMCMCInstrInfo);

  TargetRegistry::RegisterMCRegInfo(TheICMCTarget, createICMCMCRegisterInfo);

  TargetRegistry::RegisterMCSubtargetInfo(TheICMCTarget,
                                          createICMCMCSubtargetInfo);

  TargetRegistry::RegisterMCInstPrinter(TheICMCTarget,
                                        createICMCMCInstPrinter);

  TargetRegistry::RegisterAsmTargetStreamer(TheICMCTarget,
                                            createTargetAsmStreamer);

  TargetRegistry::RegisterMCAsmBackend(TheICMCTarget, createICMCAsmBackend);

  TargetRegistry::RegisterMCCodeEmitter(getTheICMCTarget(),
                                        createICMCMCCodeEmitter);
}

