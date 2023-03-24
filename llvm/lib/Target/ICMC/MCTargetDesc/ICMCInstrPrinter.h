#ifndef LLVM_ICMC_INSTR_PRINTER_H
#define LLVM_ICMC_INSTR_PRINTER_H

#include "llvm/MC/MCInstPrinter.h"

namespace llvm {

class ICMCInstPrinter : public MCInstPrinter {
public:
  ICMCInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
                 const MCRegisterInfo &MRI)
      : MCInstPrinter(MAI, MII, MRI) {}

  std::pair<const char *, uint64_t> getMnemonic(const MCInst *MI) override;
  void printInst(const MCInst *MI, uint64_t Address, StringRef Annot,
                         const MCSubtargetInfo &STI, raw_ostream &OS) override;
};

} // end namespace llvm

#endif
