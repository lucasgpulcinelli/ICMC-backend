#ifndef LLVM_ICMC_INSTR_INFO_H
#define LLVM_ICMC_INSTR_INFO_H

#include "ICMCRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "ICMCGenInstrInfo.inc"

namespace llvm {


namespace ICMCCC {

enum CondCodes {
  COND_EQ, //!< Equal
  COND_NE, //!< Not equal
  COND_GT,  //!< Greater than
  COND_LT, //!< Less than
  COND_GE, //!< Greater than or equal
  COND_LE, //!< Less than or equal
  COND_INVALID
};

} // end of namespace ICMCCC

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

  const MCInstrDesc &getBrCond(ICMCCC::CondCodes CC) const;
};

Register scavengeGPR(const TargetRegisterInfo* TRI,
                     MachineBasicBlock::iterator& MI);

} // end namespace llvm

#endif
