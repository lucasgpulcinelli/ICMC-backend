#ifndef LLVM_ICMC_INSTR_INFO_H
#define LLVM_ICMC_INSTR_INFO_H

#include "ICMCRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "ICMCGenInstrInfo.inc"

namespace llvm {

class ICMCSubtarget;

class ICMCInstrInfo : public ICMCGenInstrInfo {
  const ICMCRegisterInfo RI;
  virtual void anchor();

public:
  ICMCInstrInfo(const ICMCSubtarget &);

  const ICMCRegisterInfo &getRegisterInfo() const { return RI; }
  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
                   const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                   bool KillSrc) const override;

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MI,
                            Register DestReg, int FrameIndex,
                            const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI) const override;

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MI, Register SrcReg,
                           bool isKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI) const override;

  Register scavengeGPR(const TargetRegisterInfo* TRI,
                       MachineBasicBlock::iterator& MI,
                       Register Prev = ICMC::R0) const;
};

} // end namespace llvm

#endif
