
// operand definitions

def uimm16 : Operand<i16>, ImmLeaf<i16, [{return isInt<16>(Imm);}]>;

def memuimm16 : Operand<iPTR>, ImmLeaf<iPTR, [{return isInt<16>(Imm);}]> {
  let EncoderMethod = "encodeMemoryLabel";
}

def uimm4 : Operand<i4>;

// return statement flag 
def ICMCretflag : SDNode<"ICMCISD::RET_FLAG", SDTNone,
                         [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

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
                 (ins uimm4:$imm), pattern> {
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
                      list<dag> pattern = []>
    : ICMC32Inst<opcode, Mnemonic # " $imm", (outs), (ins memuimm16:$imm),
                 pattern> {
  let Inst{31-26} = opcode;
  let Inst{25-22} = subOpcode;
  let Inst{21-16} = 0;
}

class ICMCJumpInstr<bits<4> subOpcode, string Mnemonic, list<dag> pattern = []>
    : ICMCBranchInstr<0b000010, subOpcode, Mnemonic, pattern>;

class ICMCCallInstr<bits<4> subOpcode, string Mnemonic, list<dag> pattern = []>
    : ICMCBranchInstr<0b000011, subOpcode, Mnemonic, pattern>;

