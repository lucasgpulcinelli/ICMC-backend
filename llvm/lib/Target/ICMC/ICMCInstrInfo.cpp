#include "ICMCInstrInfo.h"
#include "ICMCMachineFunctionInfo.h"

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/RegisterScavenging.h"
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

  Register Tmp1 = scavengeGPR(TRI, MI);
  Register Tmp2 = scavengeGPR(TRI, MI, Tmp1);

  BuildMI(MBB, MI, DL, get(ICMC::LOADISP), DestReg)
      .addFrameIndex(FrameIndex)
      .addReg(Tmp1, RegState::Define | RegState::EarlyClobber)
      .addReg(Tmp2, RegState::Define | RegState::EarlyClobber);
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

  Register Tmp1 = scavengeGPR(TRI, MI);
  Register Tmp2 = scavengeGPR(TRI, MI, Tmp1);

  BuildMI(MBB, MI, DL, get(ICMC::STOREISP))
      .addReg(SrcReg, getKillRegState(isKill))
      .addFrameIndex(FrameIndex)
      .addReg(Tmp1, RegState::Define | RegState::EarlyClobber)
      .addReg(Tmp2, RegState::Define | RegState::EarlyClobber);
}


Register ICMCInstrInfo::scavengeGPR(const TargetRegisterInfo* TRI,
    MachineBasicBlock::iterator &MI, Register Prev) const {
  MachineBasicBlock *MBB = MI->getParent();
  RegScavenger RS;

  RS.enterBasicBlock(*MBB);
  RS.forward(MI);

  BitVector Candidates =
      TRI->getAllocatableSet(*MBB->getParent(), &ICMC::GPRRegClass);

  // Exclude all the registers being used by the instruction.
  for (MachineOperand &MO : MI->operands()) {
    if (MO.isReg() && MO.getReg() != 0 && !MO.isDef() &&
        !Register::isVirtualRegister(MO.getReg()))
      Candidates.reset(MO.getReg());
  }

  BitVector Available = RS.getRegsAvailable(&ICMC::GPRRegClass);
  Available &= Candidates;

  signed Reg = Available.find_next(Prev);
  assert(Reg != -1 && "ran out of registers");
  return Reg;
}

