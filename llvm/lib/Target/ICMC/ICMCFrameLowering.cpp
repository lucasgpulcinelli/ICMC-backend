#include "ICMCFrameLowering.h"
#include "ICMCInstrInfo.h"

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
}


bool ICMCFrameLowering::hasFP(const MachineFunction &MF) const {
  return true;
}

