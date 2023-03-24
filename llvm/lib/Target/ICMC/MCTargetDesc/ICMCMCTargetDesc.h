#ifndef LLVM_ICMC_MC_TARGET_DESC_H
#define LLVM_ICMC_MC_TARGET_DESC_H

#include "llvm/Support/DataTypes.h"

namespace llvm {

class Target;

} // end namespace llvm

#define GET_REGINFO_ENUM
#include "ICMCGenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_MC_HELPER_DECLS
#include "ICMCGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "ICMCGenSubtargetInfo.inc"

#endif
