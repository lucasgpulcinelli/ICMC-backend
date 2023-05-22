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

  MachineInstrBuilder buildMI(Block &MBB, BlockIt MBBI, unsigned Opcode,
                              Register DestReg) {
    return BuildMI(MBB, MBBI, MBBI->getDebugLoc(), TII->get(Opcode), DestReg);
  }

  bool expandMBB(Block &MBB);
  bool expandMI(Block &MBB, BlockIt MBBI);

  bool expandINCDECFS(Block &MBB, BlockIt MBBI, MachineInstr &MI,
                      bool IsIncrement);
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

bool ICMCExpandPseudo::expandINCDECFS(Block &MBB, BlockIt MBBI,
                                      MachineInstr &MI, bool IsIncrement){
  buildMI(MBB, MBBI, ICMC::LOADN, MI.getOperand(1).getReg())
    .addImm(MI.getOperand(0).getImm());

  buildMI(MBB, MBBI, ICMC::MOVGetSP, MI.getOperand(2).getReg());

  buildMI(MBB, MBBI, (IsIncrement)? ICMC::SUB : ICMC::ADD,
      MI.getOperand(1).getReg())
    .addReg(MI.getOperand(2).getReg(), RegState::Kill)
    .addReg(MI.getOperand(1).getReg());

  buildMI(MBB, MBBI, ICMC::MOVSetSP, MI.getOperand(1).getReg());

  MI.eraseFromParent();
  return true;
}

bool ICMCExpandPseudo::expandMI(Block &MBB, BlockIt MBBI) {
  MachineInstr &MI = *MBBI;
  int Opcode = MBBI->getOpcode();

  switch(Opcode){
  case ICMC::DECFS:
  case ICMC::INCFS:
    return expandINCDECFS(MBB, MBBI, MI, Opcode == ICMC::INCFS);
  default:
    return false;
  }
}

INITIALIZE_PASS(ICMCExpandPseudo, "icmc-expand-pseudo", ICMC_EXPAND_PSEUDO_NAME,
                false, false)

namespace llvm {

FunctionPass *createICMCExpandPseudoPass() { return new ICMCExpandPseudo(); }

} // end of namespace llvm
