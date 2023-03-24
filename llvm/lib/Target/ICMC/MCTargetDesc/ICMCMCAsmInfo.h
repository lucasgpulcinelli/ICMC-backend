#ifndef LLVM_ICMC_MC_ASM_INFO_H
#define LLVM_ICMC_MC_ASM_INFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {

class Triple;

class ICMCMCAsmInfo : public MCAsmInfoELF {
public:
  explicit ICMCMCAsmInfo(const Triple &TT);
};

} // end namespace llvm

#endif

