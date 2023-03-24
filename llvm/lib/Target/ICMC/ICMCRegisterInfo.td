
class ICMCRegister<bits<3> Enc, string n, list<string> alt = []> : Register<n> {
  let HWEncoding{2-0} = Enc;
  let AltNames = alt;
  let Namespace = "ICMC";
}

def R0  : ICMCRegister<0, "r0", ["R0"]>, DwarfRegNum<[0]>;
def R1  : ICMCRegister<1, "r1", ["R1"]>, DwarfRegNum<[1]>;
def R2  : ICMCRegister<2, "r2", ["R2"]>, DwarfRegNum<[2]>;
def R3  : ICMCRegister<3, "r3", ["R3"]>, DwarfRegNum<[3]>;
def R4  : ICMCRegister<4, "r4", ["R4"]>, DwarfRegNum<[4]>;
def R5  : ICMCRegister<5, "r5", ["R5"]>, DwarfRegNum<[5]>;
def R6  : ICMCRegister<6, "r6", ["R6"]>, DwarfRegNum<[6]>;
def R7  : ICMCRegister<7, "r7", ["R7"]>, DwarfRegNum<[7]>;

def GPR : RegisterClass<"ICMC", [i16], 16, (add (sequence "R%u", 0, 7))>;

