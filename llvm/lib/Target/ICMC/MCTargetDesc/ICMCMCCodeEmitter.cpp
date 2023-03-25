#include "ICMCMCCodeEmitter.h"
#include "ICMCInstrInfo.h"

#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Support/EndianStream.h"

using namespace llvm;

#include "ICMCGenMCCodeEmitter.inc"


void ICMCMCCodeEmitter::encodeInstruction(const MCInst &Inst, raw_ostream &OS,
                       SmallVectorImpl<MCFixup> &Fixups,
                       const MCSubtargetInfo &STI) const {
  const MCInstrDesc &Desc = MCII.get(Inst.getOpcode());

  unsigned Size = Desc.getSize();
  assert(Size > 0 && "Instruction size cannot be zero");

  uint64_t BinaryOpCode = getBinaryCodeForInstr(Inst, Fixups, STI);
  size_t WordCount = Size / 2;

  for (int64_t i = WordCount - 1; i >= 0; --i) {
    uint16_t Word = (BinaryOpCode >> (i * 16));
    support::endian::write(OS, Word, support::endianness::little);
  }
}

unsigned ICMCMCCodeEmitter::getMachineOpValue(
      const MCInst &MI, const MCOperand &MO, SmallVectorImpl<MCFixup> &Fixups,
      const MCSubtargetInfo &STI) const {
  if (MO.isReg())
    return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());

  if (MO.isDFPImm())
    return static_cast<unsigned>(bit_cast<double>(MO.getDFPImm()));

  // MO must be an Expr.
  assert(MO.isExpr());

  return getExprOpValue(MO.getExpr(), Fixups, STI);
}

unsigned ICMCMCCodeEmitter::getExprOpValue(const MCExpr *Expr,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
    llvm_unreachable("getExprOpValue not implemented");
}

MCCodeEmitter *llvm::createICMCMCCodeEmitter(const MCInstrInfo &MCII,
                                             MCContext &Ctx) {
  return new ICMCMCCodeEmitter(MCII, Ctx);
}

