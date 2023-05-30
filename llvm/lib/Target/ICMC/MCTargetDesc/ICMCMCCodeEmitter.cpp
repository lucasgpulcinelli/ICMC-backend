#include "ICMCMCCodeEmitter.h"
#include "ICMCInstrInfo.h"

#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/Support/EndianStream.h"

using namespace llvm;

#include "ICMCGenMCCodeEmitter.inc"

void ICMCMCCodeEmitter::encodeInstruction(const MCInst &Inst, raw_ostream &OS,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  const MCInstrDesc &Desc = MCII.get(Inst.getOpcode());

  unsigned Size = Desc.getSize();

  // normal instruction
  if (Size != 0) {
    // just get the binary code, and for every word write it in big endian in OS
    size_t WordCount = Size / 2;
    uint64_t BinaryOpCode = getBinaryCodeForInstr(Inst, Fixups, STI);

    for (int64_t I = WordCount - 1; I >= 0; I--) {
      uint16_t Word = (BinaryOpCode >> (I * 16));
      support::endian::write(OS, Word, support::endianness::big);
    }
    return;
  }

  // variable definition instructions
  switch (Inst.getOpcode()) {
  case ICMC::VAR:
    encodeVarDef(Inst.getOperand(0).getImm(), OS);
    return;
  case ICMC::STATIC:
    getBinaryCodeForInstr(Inst, Fixups, STI);
    return;
  default:
    llvm_unreachable("invalid opcode for instruction size of zero");
  }
}

void ICMCMCCodeEmitter::encodeVarDef(int16_t VarSize, raw_ostream &OS) const {
  for (int I = 0; I < VarSize; I++) {
    support::endian::write(OS, (uint16_t)0, support::endianness::big);
  }
}

unsigned
ICMCMCCodeEmitter::encodeMemoryLabel(const MCInst &MI, unsigned OpNo,
                                     SmallVectorImpl<MCFixup> &Fixups,
                                     const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  MCFixupKind Kind;
  if (MI.getOpcode() == ICMC::STATIC) {
    Kind = FK_SecRel_2;
  } else {
    Kind = FK_Data_2;
  }

  if (MO.isExpr()) {
    Fixups.push_back(
        MCFixup::create(2, MO.getExpr(), MCFixupKind(Kind), MI.getLoc()));
    return 0;
  }

  assert(MO.isImm());
  return MO.getImm();
}

unsigned
ICMCMCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                                     SmallVectorImpl<MCFixup> &Fixups,
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
  assert(Expr->getKind() == MCExpr::SymbolRef && "invalid expression kind");

  Fixups.push_back(MCFixup::create(2, Expr, FK_Data_2));
  return 0;
}

MCCodeEmitter *llvm::createICMCMCCodeEmitter(const MCInstrInfo &MCII,
                                             MCContext &Ctx) {
  return new ICMCMCCodeEmitter(MCII, Ctx);
}
