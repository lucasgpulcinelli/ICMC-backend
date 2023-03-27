#include "ICMCMCInstLower.h"

#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"

using namespace llvm;

ICMCMCInstLower::ICMCMCInstLower(MCContext *C, AsmPrinter &AsmPrinter) {}

void ICMCMCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const {
  llvm_unreachable("Lower not implemented");

  OutMI.setOpcode(MI->getOpcode());
}

