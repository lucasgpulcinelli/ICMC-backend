
// operand definitions

def uimm16 : Operand<i16>, ImmLeaf<i16, [{return isInt<16>(Imm);}]>;

def memuimm16 : Operand<iPTR>, ImmLeaf<iPTR, [{return isInt<16>(Imm);}]> {
  let EncoderMethod = "encodeMemoryLabel";
}

def jbb_16 : Operand<OtherVT>;

def spmem : ComplexPattern<i16, 1, "SelectSPmem", [frameindex]>;

// return statement flag 
def ICMCretflag : SDNode<"ICMCISD::RET_FLAG", SDTNone,
                         [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def SDT_ICMCCall : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;
def ICMCcall : SDNode<"ICMCISD::CALL", SDT_ICMCCall, [SDNPHasChain,
                      SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;

def SDT_ICMCCallSeqEnd : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;
def ICMCCallseqEnd : SDNode<"ISD::CALLSEQ_END", SDT_ICMCCallSeqEnd,
                            [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def SDT_ICMCBrcond
    : SDTypeProfile<0, 2, [SDTCisVT<0, OtherVT>, SDTCisVT<1, i16>]>;
def ICMCbrcond
    : SDNode<"ICMCISD::BRCOND", SDT_ICMCBrcond, [SDNPHasChain, SDNPInGlue]>;

def SDT_ICMCCmp : SDTypeProfile<0, 2, [SDTCisSameAs<0, 1>]>;
def ICMCCmp : SDNode<"ICMCISD::CMP", SDT_ICMCCmp, [SDNPOutGlue]>;

def SDT_ICMCSelectCC
    : SDTypeProfile<1, 3,
                    [SDTCisSameAs<0, 1>, SDTCisSameAs<1, 2>, SDTCisSameAs<2, 3>]>;
def ICMCselectcc : SDNode<"ICMCISD::SELECT_CC", SDT_ICMCSelectCC, [SDNPInGlue]>;

// leaves to identify types of conditions, used in logical compares
def ICMC_COND_EQ : PatLeaf<(i16 0)>;
def ICMC_COND_NE : PatLeaf<(i16 1)>;
def ICMC_COND_GR : PatLeaf<(i16 2)>;
def ICMC_COND_LT : PatLeaf<(i16 3)>;
def ICMC_COND_GE : PatLeaf<(i16 4)>;
def ICMC_COND_LE : PatLeaf<(i16 5)>;

def SDT_ICMCWrapper: SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>, SDTCisPtrTy<0>]>;
def ICMCWrapper : SDNode<"ICMCISD::WRAPPER", SDT_ICMCWrapper>;

// generic instruction
class ICMCInst<string asmStr, dag outs, dag ins, 
               list<dag> pattern = []> : Instruction {
  let Namespace = "ICMC";
  let AsmString = asmStr;
  dag InOperandList = ins;
  dag OutOperandList = outs;
  list<dag> Pattern = pattern;
}

class ICMC16Inst<bits<6> opcode, string asmStr, dag outs, dag ins, 
                 list<dag> pattern = []> 
    : ICMCInst<asmStr, outs, ins, pattern> {
  bits<16> Inst;

  let Size = 2;
  let Inst{15-10} = opcode;
}

class ICMC32Inst<bits<6> opcode, string asmStr, dag outs, dag ins, 
                 list<dag> pattern = []>
    : ICMCInst<asmStr, outs, ins, pattern> {
  bits<32> Inst;
  bits<16> imm;
  
  let Size = 4;
  let Inst{31-26} = opcode;
  let Inst{15-0} = imm;
}

class ICMCLoadStoreInstr<bits<6> opcode, string asmStr, dag outs, dag ins,
                         list<dag> pattern = []>
    : ICMC32Inst<opcode, asmStr, outs, ins, pattern> {
  bits<3> reg;

  let Inst{31-26} = opcode;
  let Inst{25-23} = reg;
  let Inst{22-16} = 0;
}

class ICMCShiftInstr<bits<3> subOpcode, string Mnemonic, 
                     list<dag> pattern = []>
    : ICMC16Inst<0b010000, Mnemonic # " $rd, $imm", (outs GPR:$rd),
                 (ins GPR:$rs, uimm16:$imm), pattern> {
  bits<3> rs;
  bits<3> rd;
  bits<4> imm;

  let Inst{9-7} = rd;
  let Inst{6-4} = subOpcode;
  let Inst{3-0} = imm;
}

class ICMCNoParams16Instr<bits<6> opcode, string Mnemonic, 
                          list<dag> pattern = []>
    : ICMC16Inst<opcode, Mnemonic, (outs), (ins), pattern> {
  let Inst{9-0} = 0;
}

class ICMC1Reg16Instr<bits<6> opcode, string Mnemonic, 
                      list<dag> pattern = []>
    : ICMC16Inst<opcode, Mnemonic # " $reg", (outs GPR:$reg), (ins), 
                 pattern> {
  bits<3> reg;

  let Inst{9-7} = reg;
  let Inst{6-0} = 0;
}

class ICMC2Regs16Instr<bits<6> opcode, string Mnemonic, list<dag> pattern = []>
    : ICMC16Inst<opcode, Mnemonic # " $rd, $rs", (outs GPR:$rd), (ins GPR:$rs),
                 pattern> {
  bits<3> rd;
  bits<3> rs;

  let Inst{9-7} = rd;
  let Inst{6-4} = rs;
  let Inst{3-0} = 0;
}

class ICMCALUInstr<bits<6> opcode, string Mnemonic, 
                   list<dag> pattern = []>
    : ICMC16Inst<opcode, Mnemonic # " $rd, $rs1, $rs2", (outs GPR:$rd), 
                 (ins GPR:$rs1, GPR:$rs2), pattern>{
  bits<3> rd;
  bits<3> rs1;
  bits<3> rs2;

  let Inst{9-7} = rd;
  let Inst{6-4} = rs1;
  let Inst{3-1} = rs2;
  let Inst{0} = 0;
}

class ICMCBranchInstr<bits<6> opcode, bits<4> subOpcode, string Mnemonic,
                      dag outs, dag ins, list<dag> pattern = []>
    : ICMC32Inst<opcode, Mnemonic # " $imm", outs, ins,
                 pattern> {
  let Inst{31-26} = opcode;
  let Inst{25-22} = subOpcode;
  let Inst{21-16} = 0;
}

class ICMCJumpInstr<bits<4> subOpcode, string Mnemonic, list<dag> pattern = []>
    : ICMCBranchInstr<0b000010, subOpcode, Mnemonic, (outs),
                      (ins jbb_16:$imm), pattern> {
  let isBarrier = 1;
  let isBranch = 1;
  let isTerminator = 1;
}

class ICMCCallInstr<bits<4> subOpcode, string Mnemonic, list<dag> pattern = []>
    : ICMCBranchInstr<0b000011, subOpcode, Mnemonic, (outs),
                      (ins memuimm16:$imm), pattern> {
  let isCall = 1;
}

