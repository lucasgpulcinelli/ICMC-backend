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
  ICMCMCInstLower(MCContext *C, AsmPrinter &asmprinter);
  void Lower(const MachineInstr *MI, MCInst &OutMI) const;
};

} // end namespace llvm

#endif
