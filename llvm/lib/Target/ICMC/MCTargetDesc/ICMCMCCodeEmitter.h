#ifndef LLVM_ICMC_MC_CODE_EMITER_H
#define LLVM_ICMC_MC_CODE_EMITER_H

#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/Support/DataTypes.h"

namespace llvm {
class MCInstrInfo;
class MCContext;
class MCOperand;
class MCExpr;

class ICMCMCCodeEmitter : public MCCodeEmitter {
  const MCInstrInfo &MCII;
  MCContext &Ctx;

public:
  ICMCMCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx)
      : MCII(MCII), Ctx(Ctx) {}

  void encodeInstruction(const MCInst &Inst, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

  void encodeVarDef(int16_t VarSize, raw_ostream &OS) const;

  unsigned encodeMemoryLabel(const MCInst &MI, unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  unsigned getExprOpValue(const MCExpr *Expr,
                          SmallVectorImpl<MCFixup> &Fixups,
                          const MCSubtargetInfo &STI) const;
};

MCCodeEmitter *createICMCMCCodeEmitter(const MCInstrInfo &MCII,
                                      MCContext &Ctx);

} // end namespace llvm

#endif
