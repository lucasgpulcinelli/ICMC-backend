#ifndef LLVM_ICMC_REGISTER_INFO_H
#define LLVM_ICMC_REGISTER_INFO_H

#include "MCTargetDesc/ICMCMCTargetDesc.h"
#include "ICMCFrameLowering.h"

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "ICMCGenRegisterInfo.inc"

namespace llvm {

class ICMCSubtarget;

class ICMCRegisterInfo : public ICMCGenRegisterInfo {
public:
  ICMCRegisterInfo(const ICMCSubtarget &);

  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  bool useFPForScavengingIndex(const MachineFunction &MF) const override;

  void eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID CC) const override;

  Register getFrameRegister(const MachineFunction &MF) const override;
};

} //end namespace llvm

#endif

