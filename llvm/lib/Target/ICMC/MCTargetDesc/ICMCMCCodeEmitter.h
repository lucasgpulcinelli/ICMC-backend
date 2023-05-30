#ifndef LLVM_ICMC_MC_CODE_EMITER_H
#define LLVM_ICMC_MC_CODE_EMITER_H

#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/Support/DataTypes.h"

namespace llvm {
class MCInstrInfo;
class MCContext;
class MCOperand;
class MCExpr;

/*
 * ICMCMCCodeEmitter: define how binary code is generated from MCInst, and how
 * all operands are encoded. Especially, it is the MCCodeEmitter's
 * responsability to create fixups for resolving memory labels, for example.
 */
class ICMCMCCodeEmitter : public MCCodeEmitter {
  const MCInstrInfo &MCII;
  MCContext &Ctx;

public:
  ICMCMCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx)
      : MCII(MCII), Ctx(Ctx) {}

  // encode an instruction and write it's binary representation in OS
  void encodeInstruction(const MCInst &Inst, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

  // encode a variable definition, for use with VAR in asm.
  void encodeVarDef(int16_t VarSize, raw_ostream &OS) const;

  unsigned encodeMemoryLabel(const MCInst &MI, unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  // get the binary representation for a MCInst, used in encodeInstruction and
  // defined in ICMCGenMCCodeEmitter.inc
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  // get the binary representation for a MCOperand, implemented for each kind
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  // get the binary representation for a MCExpr, used in getMachineOpValue
  unsigned getExprOpValue(const MCExpr *Expr, SmallVectorImpl<MCFixup> &Fixups,
                          const MCSubtargetInfo &STI) const;
};

MCCodeEmitter *createICMCMCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx);

} // end namespace llvm

#endif
