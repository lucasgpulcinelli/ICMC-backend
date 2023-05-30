#include "ICMCInstrPrinter.h"

#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"

using namespace llvm;

#define PRINT_ALIAS_INSTR
#include "ICMCGenAsmWriter.inc"

void ICMCInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                StringRef Annot, const MCSubtargetInfo &STI,
                                raw_ostream &OS) {
  llvm_unreachable("printInst not implemented");
}

void ICMCInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) {
  llvm_unreachable("printOperand not implemented");
}

const char *ICMCInstPrinter::getPrettyRegisterName(unsigned RegNo,
                                                   MCRegisterInfo const &MRI) {
  return getRegisterName(RegNo);
}
