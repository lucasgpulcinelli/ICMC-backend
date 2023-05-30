#include "ICMCMCExpr.h"

#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"

using namespace llvm;

void ICMCMCExpr::printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const {
  OS << "[Mem: " << Sym->getName() << " Off: " << getOffset()
     << " Value: " << getSubstValue() << "]";
}

bool ICMCMCExpr::evaluateAsRelocatableImpl(MCValue &Res,
                                           const MCAsmLayout *Layout,
                                           const MCFixup *Fixup) const {
  return true; // always relocatable
}

void ICMCMCExpr::visitUsedExpr(MCStreamer &Streamer) const {}

MCFragment *ICMCMCExpr::findAssociatedFragment() const {
  llvm_unreachable("findAssociatedFragment not implemented");
}

void ICMCMCExpr::fixELFSymbolsInTLSFixups(MCAssembler &) const {}
