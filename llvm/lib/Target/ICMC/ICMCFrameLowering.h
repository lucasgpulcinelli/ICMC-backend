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

/*
 * ICMCFrameLowering: defines how the stack works. This is extremely important
 * in any program using more variables than registers, or any program compiled
 * in -O0
 */
class ICMCFrameLowering : public TargetFrameLowering {

public:
  ICMCFrameLowering(const ICMCSubtarget &St)
      : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(2), -2) {
  }

  // emitPrologue emits the code at the start of every function. Used to
  // increase the frame size
  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  // emitEpilogue emits the code at the end of every function. Used to
  // decrease the frame size
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  // determineCalleeSaves modifies SavedRegs to define which registers are saved
  // (pushed and popped) during function calls. Is the base for an otimized
  // compilation when multiple function calls happen in a big function.
  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS) const override;

  // hasFP analyzes the function and returns if it needs a frame, enabling the
  // calling of most of the function in this class.
  bool hasFP(const MachineFunction &MF) const override;

  // spillCalleeSavedRegisters pushes onto the stack the Calle Saved Registers.
  bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 ArrayRef<CalleeSavedInfo> CSI,
                                 const TargetRegisterInfo *TRI) const override;

  // restoreCalleeSavedRegisters undos (aka pops) Calle Saved Registers.
  bool
  restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator MI,
                              MutableArrayRef<CalleeSavedInfo> CSI,
                              const TargetRegisterInfo *TRI) const override;
};

FunctionPass *createICMCFrameAnalyzerPass();

} // end namespace llvm

#endif
