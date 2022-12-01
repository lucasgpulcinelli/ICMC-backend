
class ICMCRegister<string name, list<Register> subregs = []>
    : RegisterWithSubRegs<name, subregs> {

    let Namespace = "ICMC";
}

def R0  : ICMCRegister<"r0">, DwarfRegNum<[0]>;
def R1  : ICMCRegister<"r1">, DwarfRegNum<[1]>;
def R2  : ICMCRegister<"r2">, DwarfRegNum<[2]>;
def R3  : ICMCRegister<"r3">, DwarfRegNum<[3]>;
def R4  : ICMCRegister<"r4">, DwarfRegNum<[4]>;
def R5  : ICMCRegister<"r5">, DwarfRegNum<[5]>;
def R6  : ICMCRegister<"r5">, DwarfRegNum<[6]>;
def R7  : ICMCRegister<"r7">, DwarfRegNum<[7]>;


def SP : RegisterClass<"ICMC", [i16], 16, (add R0, R1, R2, R3, R4, R5, R6, R7)>;

