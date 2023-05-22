#ifndef LLVM_ICMC_FRAME_LOWERING_H
#define LLVM_ICMC_FRAME_LOWERING_H

#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Pass.h"

namespace llvm {
class MachineFunction;
class ICMCSubtarget;
class ICMCInstrInfo;

class ICMCFrameLowering : public TargetFrameLowering {

public:
  ICMCFrameLowering(const ICMCSubtarget &St) : TargetFrameLowering(
      TargetFrameLowering::StackGrowsDown, Align(2), -2) {}

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS) const override;

  bool hasFP(const MachineFunction &MF) const override;

  bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 ArrayRef<CalleeSavedInfo> CSI,
                                 const TargetRegisterInfo *TRI) const override;

  bool
  restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator MI,
                              MutableArrayRef<CalleeSavedInfo> CSI,
                              const TargetRegisterInfo *TRI) const override;
};

FunctionPass *createICMCFrameAnalyzerPass();

} // end namespace llvm

#endif
