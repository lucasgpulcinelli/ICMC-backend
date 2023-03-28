
// operand definitions

def simm16 : Operand<i16>;

def memsimm16 : Operand<i16> {
    let EncoderMethod = "encodeMemoryLabel";
}

def simm4 : Operand<i4>;

// Instructions to define global variables and their values

def VAR : Instruction {
    bits<16> imm;
    bits<0> Inst;

    let Namespace = "ICMC";
    let Size = 0;

    dag InOperandList = (ins simm16:$imm);
    dag OutOperandList = (outs);

    let AsmString = "var $imm";
}

// actual Instructions definitions

class ICMCLoadStoreInstr<bits<6> Opcode, dag ins, dag outs, string Asm>
      : Instruction {
    bits<32> Inst;
    bits<3> reg;
    bits<16> imm;

    let Namespace = "ICMC";
    let Size = 4;
    let Inst{31-26} = Opcode;
    let Inst{25-23} = reg;
    let Inst{22-16} = 0b0000000;
    let Inst{15-0} = imm;

    dag InOperandList = ins;
    dag OutOperandList = outs;
    let AsmString = Asm;
}

class ICMCShiftInstr<bits<3> SubOpcode, string Mnemonic>: Instruction {
    bits<16> Inst;
    bits<3> rd;
    bits<4> imm;

    let Namespace = "ICMC";
    let Size = 2;
    let Inst{15-10} = 0b010000;
    let Inst{9-7} = rd;
    let Inst{6-4} = SubOpcode;
    let Inst{3-0} = imm;

    dag InOperandList = (ins simm4:$imm);
    dag OutOperandList = (outs GPR:$rd);

    let AsmString = Mnemonic # " $rd, $imm";
}

class ICMCALUInstr<bits<6> Opcode, bits<1> WithCarry, string Mnemonic>
      : Instruction {

    bits<16> Inst;
    bits<3> rd;
    bits<3> rs1;
    bits<3> rs2;

    let Namespace = "ICMC";
    let Size = 2;
    let Inst{15-10} = Opcode;
    let Inst{9-7} = rd;
    let Inst{6-4} = rs1;
    let Inst{3-1} = rs2;
    let Inst{0} = WithCarry;

    dag InOperandList = (ins GPR:$rs1, GPR:$rs2);
    dag OutOperandList = (outs GPR:$rd);

    let AsmString = Mnemonic # " $rd, $rs1, $rs2";
}

class ICMC2RegsInstr<bits<6> Opcode, bits<1> sideEffects, string Mnemonic>
      : Instruction {

    bits<16> Inst;
    bits<3> rd;
    bits<3> rs1;

    let Namespace = "ICMC";
    let hasSideEffects = sideEffects;
    let Size = 2;
    let Inst{15-10} = Opcode;
    let Inst{9-7} = rd;
    let Inst{6-4} = rs1;
    let Inst{3-0} = 0b0000;

    dag InOperandList = (ins GPR:$rs1);
    dag OutOperandList = (outs GPR:$rd);

    let AsmString = Mnemonic # " $rd, $rs1";
}

class ICMC1RegInstr<bits<6> Opcode, bits<1> SubOpcode, bits<1> sideEffects,
                    string Mnemonic> : Instruction {

    bits<16> Inst;
    bits<3> rd;

    let Namespace = "ICMC";
    let hasSideEffects = sideEffects;
    let Size = 2;
    let Inst{15-10} = Opcode;
    let Inst{9-7} = rd;
    let Inst{6} = SubOpcode;
    let Inst{5-0} = 0b000000;

    dag InOperandList = (ins);
    dag OutOperandList = (outs GPR:$rd);

    let AsmString = Mnemonic # " $rd";
}

class ICMCNoParametersInstr<bits<16> Opcode, bits<1> sideEffects,
                            string Mnemonic> : Instruction {

    bits<16> Inst = Opcode;

    let hasSideEffects = sideEffects;
    let Namespace = "ICMC";
    let Size = 2;
    let AsmString = Mnemonic;

    let InOperandList = (ins);
    let OutOperandList = (outs);
}

class ICMCPushRegistersInstr<bits<6> Opcode, string Mnemonic> : Instruction {
    bits<16> Inst;
    bits<3> rs;
    let hasSideEffects = 1;
    let mayStore = 1;

    let Namespace = "ICMC";
    let Size = 2;
    let Inst{15-10} = Opcode;
    let Inst{9-7} = rs;
    let Inst{6-0} = 0b0000000;

    let AsmString = Mnemonic # " $rs";

    dag InOperandList = (ins GPR:$rs);
    let OutOperandList = (outs);
}

class ICMCPopRegistersInstr<bits<6> Opcode, string Mnemonic> : Instruction {
    bits<16> Inst;
    bits<3> rd;
    let hasSideEffects = 1;
    let mayLoad = 1;

    let Namespace = "ICMC";
    let Size = 2;
    let Inst{15-10} = Opcode;
    let Inst{9-7} = rd;
    let Inst{6-0} = 0b0000000;

    let AsmString = Mnemonic # " $rd";

    let InOperandList = (ins);
    let OutOperandList = (outs GPR:$rd);
}

class ICMCBranchInstr<bits<6> Opcode, bits<4> SubOpcode, string Mnemonic>
      : Instruction {
    bits<32> Inst;
    bits<16> mem;

    let Namespace = "ICMC";
    let Size = 4;
    let Inst{31-26} = Opcode;
    let Inst{25-22} = SubOpcode;
    let Inst{21-16} = 0b000000;
    let Inst{15-0} = mem;

    let AsmString = Mnemonic # " $mem";

    let InOperandList = (ins memsimm16:$mem);
    let OutOperandList = (outs);
}

class ICMCJumpInstr<bits<4> SubOpcode, string Mnemonic>
      : ICMCBranchInstr<0b000010, SubOpcode, Mnemonic>;

class ICMCCallInstr<bits<4> SubOpcode, string Mnemonic>
      : ICMCBranchInstr<0b000011, SubOpcode, Mnemonic>;

