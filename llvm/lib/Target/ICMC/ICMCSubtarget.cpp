#include "ICMCSubtarget.h"

#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "icmc-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "ICMCGenSubtargetInfo.inc"

void ICMCSubtarget::anchor() {}

ICMCSubtarget::ICMCSubtarget(const Triple &TT, const std::string &CPU,
                           const std::string &FS, const TargetMachine &TM)
    : ICMCGenSubtargetInfo(TT, CPU, CPU, FS), InstrInfo(*this),
      FrameLowering(*this), TLInfo(TM, *this) {}


