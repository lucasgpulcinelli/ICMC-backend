#ifndef LLVM_ICMC_REGISTER_INFO_H
#define LLVM_ICMC_REGISTER_INFO_H

#include "ICMCFrameLowering.h"
#include "MCTargetDesc/ICMCMCTargetDesc.h"

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "ICMCGenRegisterInfo.inc"

namespace llvm {

class ICMCSubtarget;

/*
 * ICMCRegisterInfo: defines the registers in the architecture. Uses a lot of
 * definitions from it's tablegen and also defines some helpers for the
 * compiler.
 */
class ICMCRegisterInfo : public ICMCGenRegisterInfo {
public:
  ICMCRegisterInfo(const ICMCSubtarget &);

  // get the list of saved registers between calls as defined in the calling
  // convention.
  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

  // get the list of reserved registers (aka registers that should not be used
  // without explicit declarations in the tablegen)
  BitVector getReservedRegs(const MachineFunction &MF) const override;

  // given a MachineInstr in II, remove it's high level frame index number into
  // something of lower level (in our case is just an immediate)
  void eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  // get the mask (aka a uint32_t for bit operations) for registers that should
  // be preserved, as defined in the calling convention used.
  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID CC) const override;

  // getFrameRegister gives the frame register itself.
  Register getFrameRegister(const MachineFunction &MF) const override;
};

} // end namespace llvm

#endif
