include "ICMCInstrFormats.td"

def ADD  : ICMCALUInstr<0b100000, 0, "add">;
def ADDC : ICMCALUInstr<0b100000, 1, "addc">;
def SUB  : ICMCALUInstr<0b100001, 0, "sub">;
def SUBC : ICMCALUInstr<0b100001, 1, "subc">;
def MUL  : ICMCALUInstr<0b100010, 0, "mul">;
def DIV  : ICMCALUInstr<0b100011, 0, "div">;
def MOD  : ICMCALUInstr<0b100101, 0, "mod">;
def AND  : ICMCALUInstr<0b010010, 0, "and">;
def OR   : ICMCALUInstr<0b010011, 0, "or">;
def XOR  : ICMCALUInstr<0b010100, 0, "xor">;

def NOT : ICMC2RegsInstr<0b010101, 0, "not">;
def CMP : ICMC2RegsInstr<0b010110, 0, "cmp">;

def INC : ICMC1RegInstr<0b100100, 0, 0, "inc">;
def DEC : ICMC1RegInstr<0b100100, 1, 0, "dec">;


def CLEARC : ICMCNoParametersInstr<0b0010000000000000, 1, "clearc">;
def SETC   : ICMCNoParametersInstr<0b0010001000000000, 1, "setc">;
def HALT   : ICMCNoParametersInstr<0b0011110000000000, 1, "halt">;
def NOP   : ICMCNoParametersInstr<0b0000000000000000, 0, "nop">;
def BREAKP : ICMCNoParametersInstr<0b0011100000000000, 1, "breakp">;
def RTS    : ICMCNoParametersInstr<0b0001000000000000, 1, "rts">;

def PUSH : ICMCPushRegistersInstr<0b000101, "push">;
def POP  : ICMCPopRegistersInstr<0b000110, "pop">;

def LOADN  : ICMCLoadStoreInstr<0b111000, (ins simm16:$imm), (outs GPR:$reg),
                                "loadn $reg, $imm">;

def LOAD   : ICMCLoadStoreInstr<0b110000, (ins memsimm16:$imm),
                                (outs GPR:$reg), "load $reg, $imm">;


def STORE  : ICMCLoadStoreInstr<0b110001, (ins GPR:$reg),
                                (outs memsimm16:$imm), "store $imm, $reg">;

def LOADI  : ICMC2RegsInstr<0b111100, 0, "loadi">;
def STOREI : ICMC2RegsInstr<0b111101, 1, "storei">;

def MOV : ICMC2RegsInstr<0b110011, 0, "mov">;

def ROTL    : ICMCShiftInstr<0b100,  "rotl">;
def ROTR    : ICMCShiftInstr<0b110, "rotr">;

def SHIFTL0 : ICMCShiftInstr<0b000, "shiftl0">;
def SHIFTL1 : ICMCShiftInstr<0b001, "shiftl1">;
def SHIFTR0 : ICMCShiftInstr<0b010, "shiftr0">;
def SHIFTR1 : ICMCShiftInstr<0b011, "shiftr1">;

def INCHAR  : ICMC1RegInstr <0b110101, 0, 1, "inchar">;
def OUTCHAR : ICMC2RegsInstr<0b110010, 1, "outchar">;

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

