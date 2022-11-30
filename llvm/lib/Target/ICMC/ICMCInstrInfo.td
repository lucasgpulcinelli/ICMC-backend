include "ICMCInstrFormats.td"

// procedure return
def ret : SDNode<"RISCWISD::Ret", SDTNone,
        [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

// add instruction
let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class ALU <bits<7> funct7, bits<3> funct3, string opcodestr>
    : RWInstR<funct7, funct3, OPC_OP, (outs SP:$rd), (ins SP:$rs1, SP:$rs2),
        opcodestr, "$rd, $rs1, $rs2">;

def ADD  : ALU<0b0000000, 0b000, "add">, Sched<[WriteIALU, ReadIALU, ReadIALU]>;

