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
class ICMCOpcode<bits<6> val> {
    bits<6> Value = val;
}
def OPC_ADD : ICMCOpcode<0b100000>;
def OPC_RTS : ICMCOpcode<0b000100>;


class ICMCInst<dag outs, dag ins, string opcodestr, string argstr,
  list<dag> pattern, InstFormat format> : Instruction {
    field bits<16> Inst;
    field bits<16> SoftFail = 0;
    let Size = 4;

    bits<6> Opcode = 0;

    let Inst{5-0} = Opcode;

    let Namespace = "ICMC";

    dag OutOperandList = outs;
    dag InOperandList = ins;
    let AsmString = opcodestr # "\t" # argstr;
    let Pattern = pattern;

    let TSFlags{4-0} = format.Value;
}

// Instruction formats 
class ICMCInstR<bits<1> c, ICMCOpcode opcode, dag outs,
  dag ins, string opcodestr, string argstr> : ICMCInst<outs, ins, opcodestr, argstr, [], InstFormatR> {
    bits<3> rs2;
    bits<3> rs1;
    bits<3> rd;

    let Inst{15} = c;
    let Inst{14-12} = rs2;
    let Inst{11-9} = rs1;
    let Inst{8-6} = rd;
    let Opcode = opcode.Value;
}

class ICMCInstRet<ICMCOpcode opcode, dag outs,
  dag ins, string opcodestr, string argstr> : ICMCInst<outs, ins, opcodestr, argstr, [], InstFormatR> {

    let Opcode = opcode.Value;
}



