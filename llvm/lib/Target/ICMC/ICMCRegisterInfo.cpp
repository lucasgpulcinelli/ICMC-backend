#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/IR/Function.h"

#include "ICMCFrameLowering.h"

#define GET_REGINFO_ENUM
#define GET_REGINFO_HEADER
#define GET_REGINFO_TARGET_DESC
#include "ICMCGenRegisterInfo.inc"

namespace llvm {

class ICMCRegisterInfo : public ICMCGenRegisterInfo {
    const uint16_t *getCalleeSavedRegs(const MachineFunction *MF) const {

    };
};

}// end namespace llvm
