#ifndef LLVM_ICMC_MC_TARGET_DESC_H
#define LLVM_ICMC_MC_TARGET_DESC_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {

class Triple;

class ICMCMCAsmInfo : public MCAsmInfo {
public:
    explicit ICMCMCAsmInfo(const Triple &TT, const MCTargetOptions &Options);
};

}

#endif

