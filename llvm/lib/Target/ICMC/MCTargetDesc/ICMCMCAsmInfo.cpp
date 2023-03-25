#include "ICMCMCAsmInfo.h"

#include "llvm/ADT/Triple.h"


using namespace llvm;

ICMCMCAsmInfo::ICMCMCAsmInfo(const Triple &TT){
  CodePointerSize = 2;
  CalleeSaveStackSlotSize = 2;
  CommentString = ";";
  PrivateGlobalPrefix = ".L";
  PrivateLabelPrefix = ".L";
  UsesELFSectionDirectiveForBSS = true;
  SupportsDebugInformation = true;
}

