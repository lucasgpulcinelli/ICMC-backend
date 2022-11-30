#include "llvm/MC/TargetRegistry.h"

#include "ICMCMCTargetDesc.h"
#include "ICMCTargetInfo.h"

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

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeICMCTargetMC() {
    Target T = getTheICMCTarget();
    RegisterMCAsmInfo<ICMCMCAsmInfo> X(T);
}

