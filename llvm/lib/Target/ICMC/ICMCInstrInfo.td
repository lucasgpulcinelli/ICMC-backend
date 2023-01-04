include "ICMCInstrFormats.td"

def ret : SDNode<"ICMC::RTS", SDTNone,
        [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class ALU <bits<1> c, ICMCOpcode op, string opcodestr>
    : ICMCInstR<c, op, (outs Generic:$rd), (ins Generic:$rs1, Generic:$rs2),
        opcodestr, "$rd, $rs1, $rs2">;

// add instruction
def ADD  : ALU<0b0, OPC_ADD, "add">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

// addc instruction
def ADDC  : ALU<0b1, OPC_ADD,"addc">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

// sub instruction
def SUB  : ALU<0b0, OPC_SUB, "sub">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

// subc instruction
def SUBC  : ALU<0b1, OPC_SUB,"subc">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

// mul instruction
def MUL  : ALU<0b0, OPC_MUL,"mul">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

// and instruction
def AND  : ALU<0b0, OPC_AND,"and">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

// or instruction
def OR  : ALU<0b0, OPC_OR,"or">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

// xor instruction
def XOR  : ALU<0b0, OPC_XOR,"xor">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

// rts instruction
let isBarrier = 1, isReturn = 1, isTerminator = 1 in
class RET <string opcodestr>
    : ICMCInstRet<OPC_RTS, (outs), (ins),
        opcodestr, "">;

def RTS  : RET<"rts">, Sched<[]>;

// generic pattern classes
class PatSPSP<SDPatternOperator OpNode, ICMCInst Inst>
    : Pat<(OpNode Generic:$rs1, Generic:$rs2), (Inst Generic:$rs1, Generic:$rs2)>;

class PatSP<SDPatternOperator OpNode, ICMCInst Inst>
    : Pat<(OpNode), (Inst)>;

// simple operations
def : PatSPSP<add, ADD>;
def : PatSPSP<addc, ADDC>;
def : PatSPSP<sub, SUB>;
def : PatSPSP<subc, SUBC>;
def : PatSPSP<mul, MUL>;
def : PatSPSP<and, AND>;
def : PatSPSP<or, OR>;
def : PatSPSP<xor, XOR>;
def : PatSP<ret, RTS>;
