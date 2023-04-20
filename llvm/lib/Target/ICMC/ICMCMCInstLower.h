#ifndef LLVM_ICMC_INST_LOWER_H
#define LLVM_ICMC_INST_LOWER_H

#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/Support/Compiler.h"

namespace llvm {
class MCContext;
class MCInst;
class MCOperand;
class MachineInstr;
class MachineFunction;
class Mangler;
class AsmPrinter;

class LLVM_LIBRARY_VISIBILITY ICMCMCInstLower {

public:
  ICMCMCInstLower(MCContext& Ctx, AsmPrinter &Printer)
      : Ctx(Ctx), Printer(Printer) {}
  void lower(const MachineInstr *MI, MCInst &OutMI) const;
  MCOperand lowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const;

private:
  MCContext& Ctx;
  AsmPrinter& Printer;
};

} // end namespace llvm

#endif
