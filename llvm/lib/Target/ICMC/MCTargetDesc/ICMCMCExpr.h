#ifndef LLVM_ICMC_MC_EXPR_H
#define LLVM_ICMC_MC_EXPR_H

#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"

namespace llvm {

/*
 * ICMCMCExpr: defines a custom expression kind for the ICMC target. Is used for
 * the singular purpose of adding the static instruction, giving the ability for
 * the AsmParser to change arbitrary memory position values at will.
 */
class ICMCMCExpr : public MCTargetExpr {

  uint16_t Offset;
  uint16_t SubstValue;
  const MCSymbol *Sym;

  explicit ICMCMCExpr(const MCSymbol *Sym, uint16_t Offset, uint16_t SubstValue)
      : MCTargetExpr() {

    this->Sym = Sym;
    this->Offset = Offset;
    this->SubstValue = SubstValue;
  }

public:
  static const ICMCMCExpr *create(MCContext &Ctx, const MCSymbol *Sym,
                                  uint16_t Offset = 0,
                                  uint16_t SubstValue = 0) {
    return new (Ctx) ICMCMCExpr(Sym, Offset, SubstValue);
  }

  uint16_t getOffset() const { return Offset; }

  uint16_t getSubstValue() const { return SubstValue; }

  const MCSymbol *getSymbol() const { return Sym; }

  void printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const override;
  bool evaluateAsRelocatableImpl(MCValue &Res, const MCAsmLayout *Layout,
                                 const MCFixup *Fixup) const override;
  void visitUsedExpr(MCStreamer &Streamer) const override;

  MCFragment *findAssociatedFragment() const override;

  void fixELFSymbolsInTLSFixups(MCAssembler &) const override;
};

} // end namespace llvm

#endif
