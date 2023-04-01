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

public:
  ICMCFrameLowering(const ICMCSubtarget &st)
      : TargetFrameLowering(TargetFrameLowering::StackGrowsUp, Align(2), 0) {

  }

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS) const override;

  bool hasFP(const MachineFunction &MF) const override;
};

} // end namespace llvm

#endif
