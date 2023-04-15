#include "ICMCFrameLowering.h"
#include "ICMCInstrInfo.h"
#include "ICMCSubtarget.h"

#include "llvm/Support/ErrorHandling.h"
#include "llvm/CodeGen/MachineFunction.h"

using namespace llvm;

void ICMCFrameLowering::emitPrologue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {
}

void ICMCFrameLowering::emitEpilogue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {
}

void ICMCFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                             BitVector &SavedRegs,
                                             RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
}


bool ICMCFrameLowering::hasFP(const MachineFunction &MF) const {
  return true;
}


bool ICMCFrameLowering::spillCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    ArrayRef<CalleeSavedInfo> CSI, const TargetRegisterInfo *TRI) const {
  if (CSI.empty()) {
    return false;
  }

  DebugLoc DL = MBB.findDebugLoc(MI);
  MachineFunction &MF = *MBB.getParent();
  const ICMCSubtarget &STI = MF.getSubtarget<ICMCSubtarget>();
  const TargetInstrInfo &TII = *STI.getInstrInfo();

  for (const CalleeSavedInfo &I : CSI) {
    Register Reg = I.getReg();
    bool IsNotLiveIn = !MBB.isLiveIn(Reg);

    assert(TRI->getRegSizeInBits(*TRI->getMinimalPhysRegClass(Reg)) == 16 &&
           "Invalid register size");

    if (IsNotLiveIn) {
      MBB.addLiveIn(Reg);
    }

    // Do not kill the register when it is an input argument.
    BuildMI(MBB, MI, DL, TII.get(ICMC::PUSH))
        .addReg(Reg, getKillRegState(IsNotLiveIn))
        .setMIFlag(MachineInstr::FrameSetup);
  }

  return true;
}

bool ICMCFrameLowering::restoreCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    MutableArrayRef<CalleeSavedInfo> CSI,
    const TargetRegisterInfo *TRI) const {
  if (CSI.empty()) {
    return false;
  }

  DebugLoc DL = MBB.findDebugLoc(MI);
  const MachineFunction &MF = *MBB.getParent();
  const ICMCSubtarget &STI = MF.getSubtarget<ICMCSubtarget>();
  const TargetInstrInfo &TII = *STI.getInstrInfo();

  for (const CalleeSavedInfo &CCSI : llvm::reverse(CSI)) {
    Register Reg = CCSI.getReg();

    assert(TRI->getRegSizeInBits(*TRI->getMinimalPhysRegClass(Reg)) == 16 &&
           "Invalid register size");

    BuildMI(MBB, MI, DL, TII.get(ICMC::POP), Reg);
  }

  return true;
}

