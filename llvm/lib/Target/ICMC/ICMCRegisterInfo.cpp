#include "ICMCMachineFunctionInfo.h"
#include "ICMCRegisterInfo.h"
#include "ICMCTargetMachine.h"

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

  return CSR_Normal_SaveList;
}

BitVector ICMCRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  Reserved.set(ICMC::SP);

  return Reserved;
}


void ICMCRegisterInfo::eliminateFrameIndex(
      MachineBasicBlock::iterator II, int SPAdj, unsigned FIOperandNum,
      RegScavenger *RS) const {

  MachineInstr &MI = *II;
  MachineBasicBlock &MBB = *MI.getParent();
  const MachineFunction &MF = *MBB.getParent();
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const ICMCMachineFunctionInfo *FI = MF.getInfo<ICMCMachineFunctionInfo>();

  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  int Offset = MFI.getObjectOffset(FrameIndex);
  if (MFI.isFixedObjectIndex(FrameIndex)) {
    llvm_unreachable("stack arguments not implemented");
  } else {
    Offset -= FI->getCalleeSavedFrameSize();
  }

  MI.getOperand(FIOperandNum).ChangeToImmediate(Offset/2+1);
}

const uint32_t *ICMCRegisterInfo::getCallPreservedMask(
      const MachineFunction &MF, CallingConv::ID CC) const {
  return CSR_Normal_RegMask;
}

Register ICMCRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
    llvm_unreachable("getFrameRegister not implemented");
}

