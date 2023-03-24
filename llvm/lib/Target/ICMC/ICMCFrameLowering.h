#ifndef LLVM_ICMC_FRAME_LOWERING_H
#define LLVM_ICMC_FRAME_LOWERING_H

#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
class MachineFunction;
class ICMCSubtarget;
class ICMCInstrInfo;

class ICMCFrameLowering : public TargetFrameLowering {
private:
    const ICMCSubtarget &ST;

public:
  ICMCFrameLowering(const ICMCSubtarget &st)
      : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(2), 0),
        ST(st) {}

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS) const override;

  bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 ArrayRef<CalleeSavedInfo> CSI,
                                 const TargetRegisterInfo *TRI) const override;

  bool
  restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator MI,
                              MutableArrayRef<CalleeSavedInfo> CSI,
                              const TargetRegisterInfo *TRI) const override;

  void processFunctionBeforeFrameFinalized(MachineFunction &MF,
                                           RegScavenger *RS) const override;

  bool hasFP(const MachineFunction &MF) const override;

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I) const override;

  bool assignCalleeSavedSpillSlots(
      llvm::MachineFunction &, const llvm::TargetRegisterInfo *,
      std::vector<llvm::CalleeSavedInfo> &) const override;

};

} // end namespace llvm

#endif
