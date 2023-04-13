
class ICMCRegister<bits<3> Enc, string name> : Register<name> {
  let HWEncoding{2-0} = Enc;
  let Namespace = "ICMC";
}

def R0  : ICMCRegister<0, "r0">, DwarfRegNum<[0]>;
def R1  : ICMCRegister<1, "r1">, DwarfRegNum<[1]>;
def R2  : ICMCRegister<2, "r2">, DwarfRegNum<[2]>;
def R3  : ICMCRegister<3, "r3">, DwarfRegNum<[3]>;
def R4  : ICMCRegister<4, "r4">, DwarfRegNum<[4]>;
def R5  : ICMCRegister<5, "r5">, DwarfRegNum<[5]>;
def R6  : ICMCRegister<6, "r6">, DwarfRegNum<[6]>;
def R7  : ICMCRegister<7, "r7">, DwarfRegNum<[7]>;

def SP  : ICMCRegister<0, "sp">, DwarfRegNum<[8]>;

def GPR   : RegisterClass<"ICMC", [i16], 16, (add (sequence "R%u", 0, 7))>;
def GPRSP : RegisterClass<"ICMC", [i16], 16, (add SP)>;
