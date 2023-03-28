#include "ICMCMCCodeEmitter.h"
#include "ICMCInstrInfo.h"

#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/MC/MCFixup.h"


using namespace llvm;

#include "ICMCGenMCCodeEmitter.inc"


void ICMCMCCodeEmitter::encodeInstruction(const MCInst &Inst, raw_ostream &OS,
                       SmallVectorImpl<MCFixup> &Fixups,
                       const MCSubtargetInfo &STI) const {
  const MCInstrDesc &Desc = MCII.get(Inst.getOpcode());

  unsigned Size = Desc.getSize();

  //normal instruction
  if(Size != 0){
    size_t WordCount = Size / 2;
    uint64_t BinaryOpCode = getBinaryCodeForInstr(Inst, Fixups, STI);

    for (int64_t i = WordCount - 1; i >= 0; --i) {
      uint16_t Word = (BinaryOpCode >> (i * 16));
      support::endian::write(OS, Word, support::endianness::big);
    }
    return;
  }

  //variable definition instructions
  switch(Inst.getOpcode()){
  case ICMC::VAR:
    encodeVarDef(Inst.getOperand(0).getImm(), OS);
    return;
  default:
    llvm_unreachable("invalid opcode for instruction size of zero");
  }
}

void ICMCMCCodeEmitter::encodeVarDef(int VarSize, raw_ostream &OS) const {
  if(VarSize <= 0){
    llvm_unreachable("var cannot have size less than zero");
  }

  for(int i = 0; i < (VarSize+1)/2; i++){
    support::endian::write(OS, 0, support::endianness::big);
  }
}

unsigned
ICMCMCCodeEmitter::encodeMemoryLabel(const MCInst &MI, unsigned OpNo,
                                     SmallVectorImpl<MCFixup> &Fixups,
                                     const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (MO.isExpr()) {
    Fixups.push_back(
        MCFixup::create(0, MO.getExpr(), MCFixupKind(FK_Data_2), MI.getLoc()));
    return 0;
  }

  assert(MO.isImm());

  auto target = MO.getImm();
  return target;
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

