#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/TargetRegistry.h"

#include "ICMCMCTargetDesc.h"
#include "ICMCTargetInfo.h"

#define GET_REGINFO_ENUM
#define GET_REGINFO_MC_DESC
#include "ICMCGenRegisterInfo.inc"

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "ICMCGenInstrInfo.inc"


using namespace llvm;

ICMCMCAsmInfo::ICMCMCAsmInfo(const Triple &TT, const MCTargetOptions &Options){
    CodePointerSize = 2;
    CalleeSaveStackSlotSize = 2;
    CommentString = ";";
    PrivateGlobalPrefix = ".L";
    PrivateLabelPrefix = ".L";
    UsesELFSectionDirectiveForBSS = true;
    SupportsDebugInformation = true;
}

static MCRegisterInfo *createICMCMCRegisterInfo(const Triple &TT) {
    MCRegisterInfo *X = new MCRegisterInfo();
    InitICMCMCRegisterInfo(X, ICMC::SP, 0, 0, ICMC::PC);
    return X;
}

static MCInstrInfo *createICMCMCInstrInfo(void) {
    MCInstrInfo *X = new MCInstrInfo();
    InitICMCMCInstrInfo(X);
    return X;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeICMCTargetMC() {
    Target T = getTheICMCTarget();
    RegisterMCAsmInfo<ICMCMCAsmInfo> X(T);
    TargetRegistry::RegisterMCInstrInfo(T, createICMCMCInstrInfo);
    TargetRegistry::RegisterMCRegInfo(T, createICMCMCRegisterInfo);
}

