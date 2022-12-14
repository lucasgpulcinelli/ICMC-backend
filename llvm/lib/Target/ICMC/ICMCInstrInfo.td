include "ICMCInstrFormats.td"

def ret : SDNode<"ICMC::RTS", SDTNone,
        [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

// add instruction
let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class ALU <bits<1> c, string opcodestr>
    : ICMCInstR<c, OPC_ADD, (outs Generic:$rd), (ins Generic:$rs1, Generic:$rs2),
        opcodestr, "$rd, $rs1, $rs2">;

def ADD  : ALU<0b0, "add">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

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
def : PatSP<ret, RTS>;
