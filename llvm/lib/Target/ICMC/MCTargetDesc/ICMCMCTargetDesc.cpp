#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/TargetRegistry.h"

#include "ICMCMCTargetDesc.h"
#include "ICMCTargetInfo.h"

#define GET_REGINFO_ENUM
#define GET_REGINFO_MC_DESC
#include "ICMCGenRegisterInfo.inc"


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
    printf("%p\n", (void*) X);
    return X;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeICMCTargetMC() {
    Target T = getTheICMCTarget();
    RegisterMCAsmInfo<ICMCMCAsmInfo> X(T);
    TargetRegistry::RegisterMCRegInfo(T, createICMCMCRegisterInfo);
}

