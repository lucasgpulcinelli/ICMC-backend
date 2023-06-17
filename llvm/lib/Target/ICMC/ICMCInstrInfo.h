#ifndef LLVM_ICMC_INSTR_INFO_H
#define LLVM_ICMC_INSTR_INFO_H

#include "ICMCRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "ICMCGenInstrInfo.inc"

namespace llvm {

namespace ICMCCC {

// enum CondCodes are the conditions that a branch instruction can have, used in
// ICMCISelLowering and ICMCInstrInfo.td. This list is not complete.
enum CondCodes {
  COND_EQ, //!< Equal
  COND_NE, //!< Not equal
  COND_GT, //!< Greater than
  COND_LT, //!< Less than
  COND_GE, //!< Greater than or equal
  COND_LE, //!< Less than or equal
  COND_INVALID
};

} // end of namespace ICMCCC

class ICMCSubtarget;

/*
 * ICMCInstrInfo: defines the instructions in the architecture. Uses a lot of
 * definitions from it's tablegen and also defines some helpers for the
 * compiler.
 */
class ICMCInstrInfo : public ICMCGenInstrInfo {
  const ICMCRegisterInfo RI;
  virtual void anchor();

public:
  ICMCInstrInfo(const ICMCSubtarget &);

  const ICMCRegisterInfo &getRegisterInfo() const { return RI; }

  // copyPhysReg emits an instruction to copy the value stored in SrcReg to
  // DestReg. This operation is special during compilation, so it's not defined
  // as usual (in tablegen).
  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
                   const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                   bool KillSrc) const override;

  // loadRegFromStackSlot emits instructions to load the value of a stack slot
  // (in the offset defined bu FrameIndex) into the DestReg register. As
  // copyPhysReg, this operation is special. Note that our architecture does not
  // have a single instruction for this, so a total of four instruction and two
  // temporary registers are used.
  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MI, Register DestReg,
                            int FrameIndex, const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI) const override;

  // storeRegToStackSlot does the same as loadRegFromStackSlot, but for storing
  // :).
  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MI, Register SrcReg,
                           bool IsKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI) const override;

  // getBrCond matches a ICMCCC to it's related instruction. Used in
  // ICMCISelLowering.
  const MCInstrDesc &getBrCond(ICMCCC::CondCodes CC) const;
};

// scavengeGPR picks a non allocated register for use in a temporary manner.
// Used in loadRegFromStackSlot and storeRegToStackSlot.
Register scavengeGPR(const TargetRegisterInfo *TRI,
                     MachineBasicBlock::iterator &MI);

} // end namespace llvm

#endif
