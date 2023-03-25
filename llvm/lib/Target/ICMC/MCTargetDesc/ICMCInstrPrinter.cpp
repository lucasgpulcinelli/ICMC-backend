#include "ICMCInstrPrinter.h"

using namespace llvm;

std::pair<const char *, uint64_t>
ICMCInstPrinter::getMnemonic(const MCInst *MI) {
    llvm_unreachable("getMnemonic not implemented");
}

void ICMCInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                StringRef Annot, const MCSubtargetInfo &STI,
                                raw_ostream &OS) {
    llvm_unreachable("printInst not implemented");
}

