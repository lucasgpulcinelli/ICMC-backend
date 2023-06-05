#include "ICMCInstrInfo.h"
#include "ICMCTargetMachine.h"
#include "MCTargetDesc/ICMCMCTargetDesc.h"

#include "llvm/CodeGen/RegisterScavenging.h"

#define ICMC_EXPAND_PSEUDO_NAME "ICMC pseudo instruction expansion pass"

using namespace llvm;

namespace {

/*
 * ICMCExpandPseudo: defines how pseudo instructions (Instructions in the
 * tablegen defined with IsPseudo=1, meaning instructions that can be treated as
 * atomic, but are actually a list of correlated operations) are expanded.
 * This is the final compilation step before turning MCInsts into binary output
 * code.
 */
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
  bool expandFRIDX(Block &MBB, BlockIt MBBI, MachineInstr &MI);
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

// INC and DEC Frame Size are just LOADN of the frame size, a MOVGetSP, an
// ADD or SUB, and a MOVSetSP. This is ideal if the frame size is greater than 5
bool ICMCExpandPseudo::expandINCDECFS(Block &MBB, BlockIt MBBI,
                                      MachineInstr &MI, bool IsIncrement) {
  buildMI(MBB, MBBI, ICMC::LOADN, MI.getOperand(1).getReg())
      .addImm(MI.getOperand(0).getImm());

  buildMI(MBB, MBBI, ICMC::MOVGetSP, MI.getOperand(2).getReg());

  buildMI(MBB, MBBI, (IsIncrement) ? ICMC::SUB : ICMC::ADD,
          MI.getOperand(1).getReg())
      .addReg(MI.getOperand(2).getReg(), RegState::Kill)
      .addReg(MI.getOperand(1).getReg());

  buildMI(MBB, MBBI, ICMC::MOVSetSP, MI.getOperand(1).getReg());

  MI.eraseFromParent();
  return true;
}

// FRIDX is just a MOVGetSP, a LOADN of the frame index, and an ADD
bool ICMCExpandPseudo::expandFRIDX(Block &MBB, BlockIt MBBI, MachineInstr &MI) {
  buildMI(MBB, MBBI, ICMC::MOVGetSP, MI.getOperand(0).getReg());

  Register TmpReg = scavengeGPR(TRI, MBBI);
  buildMI(MBB, MBBI, ICMC::LOADN, TmpReg).addImm(MI.getOperand(1).getImm());

  buildMI(MBB, MBBI, ICMC::ADD, MI.getOperand(0).getReg())
      .addReg(TmpReg)
      .addReg(MI.getOperand(0).getReg());

  MI.eraseFromParent();
  return true;
}

bool ICMCExpandPseudo::expandMI(Block &MBB, BlockIt MBBI) {
  MachineInstr &MI = *MBBI;
  int Opcode = MBBI->getOpcode();

  switch (Opcode) {
  case ICMC::DECFS:
  case ICMC::INCFS:
    return expandINCDECFS(MBB, MBBI, MI, Opcode == ICMC::INCFS);
  case ICMC::FRIDX:
    return expandFRIDX(MBB, MBBI, MI);
  default:
    // if the opcode is not a pseudo, we don't do anything
    return false;
  }
}

INITIALIZE_PASS(ICMCExpandPseudo, "icmc-expand-pseudo", ICMC_EXPAND_PSEUDO_NAME,
                false, false)

namespace llvm {

FunctionPass *createICMCExpandPseudoPass() { return new ICMCExpandPseudo(); }

} // end of namespace llvm
