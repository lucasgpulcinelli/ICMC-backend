#include "ICMCFrameLowering.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

void ICMCFrameLowering::emitPrologue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {
    llvm_unreachable("emitPrologue not implemented");
}

void ICMCFrameLowering::emitEpilogue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {
    llvm_unreachable("emitEpilogue not implemented");
}

void ICMCFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                             BitVector &SavedRegs,
                                             RegScavenger *RS) const {
    llvm_unreachable("determineCalleeSaves not implemented");
}

bool ICMCFrameLowering::spillCalleeSavedRegisters(
                               MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MI,
                               ArrayRef<CalleeSavedInfo> CSI,
                               const TargetRegisterInfo *TRI) const {
    llvm_unreachable("spillCalleeSavedRegisters not implemented");
}

bool ICMCFrameLowering::restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 MutableArrayRef<CalleeSavedInfo> CSI,
                                 const TargetRegisterInfo *TRI) const {
    llvm_unreachable("restoreCalleeSavedRegisters not implemented");
}

void ICMCFrameLowering::processFunctionBeforeFrameFinalized(
      MachineFunction &MF, RegScavenger *RS) const {
    llvm_unreachable("processFunctionBeforeFrameFinalized not implemented");
}

bool ICMCFrameLowering::hasFP(const MachineFunction &MF) const {
    llvm_unreachable("hasFP not implemented");
}

MachineBasicBlock::iterator
ICMCFrameLowering::eliminateCallFramePseudoInstr(MachineFunction &MF,
      MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const {
    llvm_unreachable("eliminateCallFramePseudoInstr not implemented");
}

bool ICMCFrameLowering::assignCalleeSavedSpillSlots(llvm::MachineFunction &,
                                 const llvm::TargetRegisterInfo *,
                                 std::vector<llvm::CalleeSavedInfo> &) const {
    llvm_unreachable("assignCalleeSavedSpillSlots not implemented");
}


