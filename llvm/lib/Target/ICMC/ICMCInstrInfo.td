include "ICMCInstrFormats.td"

def VAR : ICMCInst<"var $imm", (outs), (ins uimm16:$imm)> {
  bits<16> imm;
  bits<0> Inst;

  let Size = 0;
}

def STATIC : ICMCInst<"static $mem, $pos, $val", (outs), 
                      (ins memuimm16:$mem, uimm16:$pos, uimm16:$val)> {
  bits<16> mem;
  bits<16> pos;
  bits<16> val;
  bits<16> Inst;

  let Inst = mem;
  let Size = 0;
}

def ADD  : ICMCALUInstr<0b100000, "add", [(set GPR:$rd, (add GPR:$rs1, GPR:$rs2))]>;
def ADDC : ICMCALUInstr<0b100000, "addc", [(set GPR:$rd, (adde GPR:$rs1, GPR:$rs2))]> {
  let Inst{0} = 1; // carry bit
}
def SUB  : ICMCALUInstr<0b100001, "sub", [(set GPR:$rd, (sub GPR:$rs1, GPR:$rs2))]>;
def SUBC : ICMCALUInstr<0b100001, "subc", [(set GPR:$rd, (sube GPR:$rs1, GPR:$rs2))]> {
 let Inst{0} = 1; //carry bit
}
def MUL  : ICMCALUInstr<0b100010, "mul", [(set GPR:$rd, (mul GPR:$rs1, GPR:$rs2))]>;
def DIV  : ICMCALUInstr<0b100011, "div", [(set GPR:$rd, (udiv GPR:$rs1, GPR:$rs2))]>;
def MOD  : ICMCALUInstr<0b100101, "mod", [(set GPR:$rd, (urem GPR:$rs1, GPR:$rs2))]>;
def AND  : ICMCALUInstr<0b010010, "and", [(set GPR:$rd, (and GPR:$rs1, GPR:$rs2))]>;
def OR   : ICMCALUInstr<0b010011, "or", [(set GPR:$rd, (or GPR:$rs1, GPR:$rs2))]>;
def XOR  : ICMCALUInstr<0b010100, "xor", [(set GPR:$rd, (xor GPR:$rs1, GPR:$rs2))]>;

def NOT : ICMC2Regs16Instr<0b010101, "not", [(set GPR:$rd, (not GPR:$rs))]>;
def CMP : ICMC2Regs16Instr<0b010110, "cmp">;

def INC : ICMC1Reg16Instr<0b100100, "inc">;
def DEC : ICMC1Reg16Instr<0b100100, "dec"> {
  let Inst{6} = 1;
}

def CLEARC : ICMCNoParams16Instr<0b001000, "clearc">;
def SETC   : ICMCNoParams16Instr<0b001000, "setc">{
  let Inst{9} = 1;
}

def HALT   : ICMCNoParams16Instr<0b001111, "halt">;
def NOP    : ICMCNoParams16Instr<0b000000, "nop">;
def BREAKP : ICMCNoParams16Instr<0b001110, "breakp">;

def RTS    : ICMCNoParams16Instr<0b000100, "rts", [(ICMCretflag)]> {
  let hasSideEffects = 1;
  let isTerminator = 1;
  let isReturn = 1;
  let Uses = [SP];
}

def PUSH : ICMC1Reg16Instr<0b000101, "push"> {
  dag InOperandList = (ins GPR:$reg);
  dag OutOperandList = (outs); 

  let Uses = [SP];
  let mayStore = 1;
}

def POP  : ICMC1Reg16Instr<0b000110, "pop"> {
  let mayLoad = 1;
  let Uses = [SP];
}

def LOADN  : ICMCLoadStoreInstr<0b111000, "loadn $reg, $imm", (outs GPR:$reg),
    (ins uimm16:$imm), [(set GPR:$reg, (load uimm16:$imm))]> {
  let mayLoad = 1;
}

// Patterns for loading constants in registers
def : Pat<(uimm16:$imm), (LOADN uimm16:$imm)>;

def LOAD  : ICMCLoadStoreInstr<0b110000, "load $reg, $imm", (outs GPR:$reg), 
    (ins memuimm16:$imm), [(set GPR:$reg, (load memuimm16:$imm))]> {
  let mayLoad = 1;
}

def STORE  : ICMCLoadStoreInstr<0b110001, "store $imm, $reg", (outs), 
                                (ins GPR:$reg, memuimm16:$imm)> {
  let mayStore = 1;
}

def LOADI  : ICMC2Regs16Instr<0b111100, "loadi"> {
  let mayLoad = 1;
}

def STOREI : ICMC2Regs16Instr<0b111101, "storei"> {
  let mayStore = 1;
}

def MOV : ICMC2Regs16Instr<0b110011, "mov">;
def MOVGetSP : ICMC16Inst<0b110011, "mov $rd, sp", (outs GPR:$rd), (ins)> {
  bits<3> rd;

  let Uses = [SP];

  let Inst{9-7} = rd;
  let Inst{6-4} = 0;
  let Inst{3-0} = 0b0001;
}

def MOVSetSP : ICMC16Inst<0b110011, "mov sp, $rs", (outs), (ins GPR:$rs)> {
  bits<3> rs;

  let Uses = [SP];

  let Inst{9-7} = rs;
  let Inst{6-4} = 0;
  let Inst{3-0} = 0b0011;
}

def LOADISP : ICMCInst<"loadisp $rd, $imm", (outs GPR:$rd),
                       (ins memuimm16:$imm, variable_ops),
                       [(set GPR:$rd, (load spmem:$imm))]> {
  bits<3> rd;
  bits<32> imm;

  let hasPostISelHook = 1;
  let Uses = [SP];

  let isPseudo = 1;
  let isCodeGenOnly = 1;
}

def STOREISP : ICMCInst<"storeisp $rs, $imm", (outs),
                       (ins GPR:$rs, memuimm16:$imm, variable_ops),
                       [(store GPR:$rs, spmem:$imm)]> {
  bits<3> rd;
  bits<32> imm;

  let hasPostISelHook = 1;
  let Uses = [SP];

  let isPseudo = 1;
  let isCodeGenOnly = 1;
}


def ROTL    : ICMCShiftInstr<0b100, "rotl">;
def ROTR    : ICMCShiftInstr<0b110, "rotr">;

def SHIFTL0 : ICMCShiftInstr<0b000, "shiftl0">;
def SHIFTL1 : ICMCShiftInstr<0b001, "shiftl1">;
def SHIFTR0 : ICMCShiftInstr<0b010, "shiftr0">;
def SHIFTR1 : ICMCShiftInstr<0b011, "shiftr1">;

def INCHAR  : ICMC1Reg16Instr <0b110101, "inchar">;
def OUTCHAR : ICMC2Regs16Instr<0b110010, "outchar">;

def JMP : ICMCJumpInstr<0b0000, "jmp">;
def JEQ : ICMCJumpInstr<0b0001, "jeq">;
def JNE : ICMCJumpInstr<0b0010, "jne">;
def JZ  : ICMCJumpInstr<0b0011, "jz">;
def JNZ : ICMCJumpInstr<0b0100, "jnz">;
def JC  : ICMCJumpInstr<0b0101, "jc">;
def JNC : ICMCJumpInstr<0b0110, "jnc">;
def JGR : ICMCJumpInstr<0b0111, "jgr">;
def JLE : ICMCJumpInstr<0b1000, "jle">;
def JEG : ICMCJumpInstr<0b1001, "jeg">;
def JEL : ICMCJumpInstr<0b1010, "jel">;
def JOV : ICMCJumpInstr<0b1011, "jov">;
def JNOV: ICMCJumpInstr<0b1100, "jnov">;
def JN  : ICMCJumpInstr<0b1101, "jn">;
def JDZ : ICMCJumpInstr<0b1110, "jdz">;

def CALL : ICMCCallInstr<0b0000, "call">;
def CEQ  : ICMCCallInstr<0b0001, "ceq">;
def CNE  : ICMCCallInstr<0b0010, "cne">;
def CZ   : ICMCCallInstr<0b0011, "cz">;
def CNZ  : ICMCCallInstr<0b0100, "cnz">;
def CC   : ICMCCallInstr<0b0101, "cc">;
def CNC  : ICMCCallInstr<0b0110, "cnc">;
def CGR  : ICMCCallInstr<0b0111, "cgr">;
def CLE  : ICMCCallInstr<0b1000, "cle">;
def CEG  : ICMCCallInstr<0b1001, "ceg">;
def CEL  : ICMCCallInstr<0b1010, "cel">;
def COV  : ICMCCallInstr<0b1011, "cov">;
def CNOV : ICMCCallInstr<0b1100, "cnov">;
def CN   : ICMCCallInstr<0b1101, "cn">;
def CDZ  : ICMCCallInstr<0b1110, "cdz">;

