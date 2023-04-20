#include "ICMCInstrInfo.h"
#include "ICMCMachineFunctionInfo.h"

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "ICMCGenInstrInfo.inc"

void ICMCInstrInfo::anchor() {}

ICMCInstrInfo::ICMCInstrInfo(const ICMCSubtarget &ST)
    : ICMCGenInstrInfo(), RI(ST) {}

void ICMCInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
    MachineBasicBlock::iterator MI, const DebugLoc &DL, MCRegister DestReg,
    MCRegister SrcReg, bool KillSrc) const {

  if(ICMC::GPRRegClass.contains(DestReg, SrcReg)) {
    BuildMI(MBB, MI, DL, get(ICMC::MOV), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  }
  if (SrcReg == ICMC::SP && ICMC::GPRRegClass.contains(DestReg)) {
    BuildMI(MBB, MI, DL, get(ICMC::MOVGetSP), DestReg);
    return;
  }
  if (DestReg == ICMC::SP && ICMC::GPRRegClass.contains(SrcReg)) {
    BuildMI(MBB, MI, DL, get(ICMC::MOVSetSP))
      .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  }

  llvm_unreachable("invalid copy from registers");
}

void ICMCInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                         MachineBasicBlock::iterator MI,
                                         Register DestReg, int FrameIndex,
                                         const TargetRegisterClass *RC,
                                         const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (MI != MBB.end()) {
    DL = MI->getDebugLoc();
  }

  BuildMI(MBB, MI, DL, get(ICMC::LOADISP), DestReg)
      .addFrameIndex(FrameIndex);
}


void ICMCInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator MI,
                                        Register SrcReg, bool isKill,
                                        int FrameIndex,
                                        const TargetRegisterClass *RC,
                                        const TargetRegisterInfo *TRI) const {
  MachineFunction &MF = *MBB.getParent();
  ICMCMachineFunctionInfo *FI = MF.getInfo<ICMCMachineFunctionInfo>();

  FI->setHasFrameIndex();

  DebugLoc DL;
  if (MI != MBB.end()) {
    DL = MI->getDebugLoc();
  }

  BuildMI(MBB, MI, DL, get(ICMC::STOREISP))
      .addReg(SrcReg, getKillRegState(isKill))
      .addFrameIndex(FrameIndex);
}

