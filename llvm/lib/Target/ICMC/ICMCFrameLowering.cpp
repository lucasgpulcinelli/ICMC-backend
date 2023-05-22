#include "ICMCFrameLowering.h"
#include "ICMCInstrInfo.h"
#include "ICMCMachineFunctionInfo.h"
#include "ICMCSubtarget.h"

#include "llvm/Support/ErrorHandling.h"
#include "llvm/CodeGen/MachineFunction.h"

using namespace llvm;

void ICMCFrameLowering::emitPrologue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator MBBI = MBB.begin();
  const ICMCSubtarget &STI = MF.getSubtarget<ICMCSubtarget>();
  const ICMCInstrInfo &TII = *STI.getInstrInfo();
  DebugLoc DL = (MBBI != MBB.end()) ? MBBI->getDebugLoc() : DebugLoc();
  ICMCMachineFunctionInfo *FI = MF.getInfo<ICMCMachineFunctionInfo>();
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  unsigned FrameSize = MFI.getStackSize() - FI->getCalleeSavedFrameSize();

  if(!hasFP(MF)){
    return;
  }

  while (
      (MBBI != MBB.end()) && MBBI->getFlag(MachineInstr::FrameSetup) &&
      (MBBI->getOpcode() == ICMC::PUSH)) {
    ++MBBI;
  }

  BuildMI(MBB, MBBI, DL, TII.get(ICMC::INCFS))
      .addImm(FrameSize/2)
      .addReg(ICMC::R4)
      .addReg(ICMC::R5)
      .setMIFlag(MachineInstr::FrameSetup);
}

void ICMCFrameLowering::emitEpilogue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  const ICMCSubtarget &STI = MF.getSubtarget<ICMCSubtarget>();
  const ICMCInstrInfo &TII = *STI.getInstrInfo();
  DebugLoc DL = (MBBI != MBB.end()) ? MBBI->getDebugLoc() : DebugLoc();
  const ICMCMachineFunctionInfo *FI = MF.getInfo<ICMCMachineFunctionInfo>();
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  unsigned FrameSize = MFI.getStackSize() - FI->getCalleeSavedFrameSize();

  if(!hasFP(MF)){
    return;
  }

  while (MBBI != MBB.begin()) {
    MachineBasicBlock::iterator PI = std::prev(MBBI);
    int Opc = PI->getOpcode();

    if (Opc != ICMC::POP && !PI->isTerminator()) {
      break;
    }

    --MBBI;
  }


  BuildMI(MBB, MBBI, DL, TII.get(ICMC::DECFS))
    .addImm(FrameSize/2)
    .addReg(ICMC::R4)
    .addReg(ICMC::R5);
}

void ICMCFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                             BitVector &SavedRegs,
                                             RegScavenger *RS) const {

  ICMCMachineFunctionInfo *FI = MF.getInfo<ICMCMachineFunctionInfo>();

  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);

  if(FI->hasFrameIndex()){
    SavedRegs.set(ICMC::R4);
    SavedRegs.set(ICMC::R5);
  }
}


bool ICMCFrameLowering::hasFP(const MachineFunction &MF) const {
  const ICMCMachineFunctionInfo
  *FuncInfo = MF.getInfo<ICMCMachineFunctionInfo>();

  return FuncInfo->hasFrameIndex();
}

namespace llvm{
bool operator==(const CalleeSavedInfo A, const CalleeSavedInfo B){
  return A.getReg() == B.getReg();
}

} // end namespace llvm

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
  ICMCMachineFunctionInfo *FI = MF.getInfo<ICMCMachineFunctionInfo>();

  unsigned CalleeFrameSize = 0;

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
    CalleeFrameSize += 2;
  }

  FI->setCalleeSavedFrameSize(CalleeFrameSize);

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

namespace llvm {

struct ICMCFrameAnalyzer : public MachineFunctionPass {
  static char ID;
  ICMCFrameAnalyzer() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override {
    const MachineFrameInfo &MFI = MF.getFrameInfo();
    ICMCMachineFunctionInfo *FI = MF.getInfo<ICMCMachineFunctionInfo>();

    if (MFI.getNumObjects() != MFI.getNumFixedObjects()) {
      for (int I = 0; I != MFI.getObjectIndexEnd(); ++I) {
        if (MFI.getObjectSize(I)) {
          FI->setHasFrameIndex();
          return false;
        }
      }
    }

    return false;
  }

  StringRef getPassName() const override { return "ICMC Frame Analyzer"; }
};

char ICMCFrameAnalyzer::ID = 0;

FunctionPass *createICMCFrameAnalyzerPass() { return new ICMCFrameAnalyzer(); }

} // end namespace llvm

