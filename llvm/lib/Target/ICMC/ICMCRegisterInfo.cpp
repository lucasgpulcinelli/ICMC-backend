#include "ICMCRegisterInfo.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/Debug.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define GET_REGINFO_TARGET_DESC
#include "ICMCGenRegisterInfo.inc"

ICMCRegisterInfo::ICMCRegisterInfo(const ICMCSubtarget &ST)
    : ICMCGenRegisterInfo(ICMC::R0) {}


const MCPhysReg *ICMCRegisterInfo::getCalleeSavedRegs(
      const MachineFunction *MF) const {

    llvm_unreachable("getCalleeSavedRegs not implemented");
}

BitVector ICMCRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
    llvm_unreachable("getReservedRegs not implemented");
}

bool ICMCRegisterInfo::requiresRegisterScavenging(
      const MachineFunction &MF) const {
    llvm_unreachable("requiresRegisterScavenging not implemented");
}

bool ICMCRegisterInfo::useFPForScavengingIndex(
      const MachineFunction &MF) const {
    llvm_unreachable("useFPForScavengingIndex not implemented");
}

void ICMCRegisterInfo::eliminateFrameIndex(
      MachineBasicBlock::iterator II, int SPAdj, unsigned FIOperandNum,
      RegScavenger *RS) const {
    llvm_unreachable("eliminateFrameIndex not implemented");
}

const uint32_t *ICMCRegisterInfo::getCallPreservedMask(
      const MachineFunction &MF, CallingConv::ID CC) const {
    llvm_unreachable("getCallPreservedMask not implemented");
}

Register ICMCRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
    llvm_unreachable("getFrameRegister not implemented");
}

