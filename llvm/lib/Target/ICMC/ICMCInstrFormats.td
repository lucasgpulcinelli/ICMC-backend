// define scheduler resources associated with def operands.
// 32 or 64-bit integer ALU operations
def WriteIALU       : SchedWrite;    
def ReadIALU        : SchedRead;


// format specifies the encoding used by the instruction
class InstFormat<bits<5> val> {
    bits<5> Value = val;
}
def InstFormatPseudo : InstFormat<0>;
def InstFormatR      : InstFormat<1>;


// base opcode map
class RISCWOpcode<bits<7> val> {
    bits<7> Value = val;
}
def OPC_OP : RISCWOpcode<0b0110011>;


class RWInst<dag outs, dag ins, string opcodestr, string argstr,
  list<dag> pattern, InstFormat format> : Instruction {
    field bits<32> Inst;
    field bits<32> SoftFail = 0;
    let Size = 4;

    bits<7> Opcode = 0;

    let Inst{6-0} = Opcode;

    let Namespace = "RISCW";

    dag OutOperandList = outs;
    dag InOperandList = ins;
    let AsmString = opcodestr # "\t" # argstr;
    let Pattern = pattern;

    let TSFlags{4-0} = format.Value;
}

// Instruction formats 
class RWInstR<bits<7> funct7, bits<3> funct3, RISCWOpcode opcode, dag outs,
  dag ins, string opcodestr, string argstr> : RWInst<outs, ins, opcodestr, argstr, [], InstFormatR> {
    bits<5> rs2;
    bits<5> rs1;
    bits<5> rd;

    let Inst{31-25} = funct7;
    let Inst{24-20} = rs2;
    let Inst{19-15} = rs1;
    let Inst{14-12} = funct3;
    let Inst{11-7} = rd;
    let Opcode = opcode.Value;
}




