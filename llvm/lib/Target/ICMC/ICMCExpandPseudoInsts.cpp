#include "ICMCInstrInfo.h"
#include "ICMCTargetMachine.h"
#include "MCTargetDesc/ICMCMCTargetDesc.h"

#include "llvm/CodeGen/RegisterScavenging.h"

#define ICMC_EXPAND_PSEUDO_NAME "ICMC pseudo instruction expansion pass"

using namespace llvm;

namespace {

class ICMCExpandPseudo : public MachineFunctionPass {
public:
  static char ID;

  ICMCExpandPseudo() : MachineFunctionPass(ID) {
    initializeICMCExpandPseudoPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  StringRef getPassName() const override { return ICMC_EXPAND_PSEUDO_NAME; }

private:
  typedef MachineBasicBlock Block;
  typedef Block::iterator BlockIt;
  const TargetInstrInfo *TII;
  const ICMCRegisterInfo *TRI;

  MachineInstrBuilder buildMI(Block &MBB, BlockIt MBBI, unsigned Opcode) {
    return BuildMI(MBB, MBBI, MBBI->getDebugLoc(), TII->get(Opcode));
  }

  Register scavengeGPR8(MachineInstr &MI);

  bool expandMBB(Block &MBB);
  bool expandMI(Block &MBB, BlockIt MBBI);

  bool expandLOADISP(Block &MBB, BlockIt MBBI, MachineInstr &MI);
  bool expandSTOREISP(Block &MBB, BlockIt MBBI, MachineInstr &MI);
};

char ICMCExpandPseudo::ID = 0;

} // end anonymous namespace

bool ICMCExpandPseudo::runOnMachineFunction(MachineFunction &MF) {
  bool Modified = false;

  const ICMCSubtarget &STI = MF.getSubtarget<ICMCSubtarget>();
  TRI = STI.getRegisterInfo();
  TII = STI.getInstrInfo();

  MF.getProperties().set(MachineFunctionProperties::Property::TracksLiveness);

  for (Block &MBB : MF) {
    bool ContinueExpanding = true;
    unsigned ExpandCount = 0;

    do {
      assert(ExpandCount < 10 && "pseudo expand limit reached");
      (void)ExpandCount;

      bool BlockModified = expandMBB(MBB);
      Modified |= BlockModified;
      ExpandCount++;

      ContinueExpanding = BlockModified;
    } while (ContinueExpanding);
  }

  return Modified;
}

bool ICMCExpandPseudo::expandMBB(Block &MBB) {
  bool Modified = false;

  BlockIt MBBI = MBB.begin(), E = MBB.end();
  while (MBBI != E) {
    BlockIt NMBBI = std::next(MBBI);
    Modified |= expandMI(MBB, MBBI);
    MBBI = NMBBI;
  }

  return Modified;
}

Register ICMCExpandPseudo::scavengeGPR8(MachineInstr &MI) {
  MachineBasicBlock &MBB = *MI.getParent();
  RegScavenger RS;

  RS.enterBasicBlock(MBB);
  RS.forward(MI);

  BitVector Candidates =
      TRI->getAllocatableSet(*MBB.getParent(), &ICMC::GPRRegClass);

  for (MachineOperand &MO : MI.operands()) {
    if (MO.isReg() && MO.getReg() != 0 && !MO.isDef() &&
        !Register::isVirtualRegister(MO.getReg()))
      Candidates.reset(MO.getReg());
  }

  BitVector Available = RS.getRegsAvailable(&ICMC::GPRRegClass);
  Available &= Candidates;

  signed Reg = Available.find_first();
  assert(Reg != -1 && "ran out of registers");
  return Reg;
}

bool ICMCExpandPseudo::expandLOADISP(Block &MBB, BlockIt MBBI, MachineInstr &MI){
  Register SPTmpReg, FITmpReg, AddTmpReg;

  SPTmpReg = scavengeGPR8(MI);
  FITmpReg = scavengeGPR8(MI);
  AddTmpReg = scavengeGPR8(MI);

  buildMI(MBB, MBBI, ICMC::MOVGetSP)
    .addReg(SPTmpReg, RegState::Define);
  buildMI(MBB, MBBI, ICMC::LOADN)
    .addReg(FITmpReg, RegState::Define)
    .addImm(MI.getOperand(1).getImm());
  buildMI(MBB, MBBI, ICMC::ADD)
    .addReg(AddTmpReg, RegState::Define)
    .addReg(SPTmpReg)
    .addReg(FITmpReg);
  buildMI(MBB, MBBI, ICMC::LOADI)
    .addReg(MI.getOperand(0).getReg())
    .addReg(AddTmpReg);

  MI.eraseFromParent();
  return true;
}

bool ICMCExpandPseudo::expandSTOREISP(Block &MBB, BlockIt MBBI, 
                                      MachineInstr &MI){
  Register SPTmpReg, FITmpReg, AddTmpReg;

  SPTmpReg = scavengeGPR8(MI);
  FITmpReg = scavengeGPR8(MI);
  AddTmpReg = scavengeGPR8(MI);

  buildMI(MBB, MBBI, ICMC::MOVGetSP)
    .addReg(SPTmpReg, RegState::Define);
  buildMI(MBB, MBBI, ICMC::LOADN)
    .addReg(FITmpReg, RegState::Define)
    .addImm(MI.getOperand(1).getImm());
  buildMI(MBB, MBBI, ICMC::ADD)
    .addReg(AddTmpReg, RegState::Define)
    .addReg(SPTmpReg)
    .addReg(FITmpReg);
  buildMI(MBB, MBBI, ICMC::STOREI)
    .addReg(AddTmpReg)
    .addReg(MI.getOperand(0).getReg());

  MI.eraseFromParent();
  return true;
}

bool ICMCExpandPseudo::expandMI(Block &MBB, BlockIt MBBI) {
  MachineInstr &MI = *MBBI;
  int Opcode = MBBI->getOpcode();

  switch(Opcode){
  case ICMC::LOADISP:
    return expandLOADISP(MBB, MBBI, MI);
  case ICMC::STOREISP:
    return expandSTOREISP(MBB, MBBI, MI);
  default:
    return false;
  }
}

INITIALIZE_PASS(ICMCExpandPseudo, "icmc-expand-pseudo", ICMC_EXPAND_PSEUDO_NAME,
                false, false)

namespace llvm {

FunctionPass *createICMCExpandPseudoPass() { return new ICMCExpandPseudo(); }

} // end of namespace llvm
