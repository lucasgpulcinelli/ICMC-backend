

def R0  : Register<"r0">, DwarfRegNum<[0]>;
def R1  : Register<"r1">, DwarfRegNum<[1]>;
def R2  : Register<"r2">, DwarfRegNum<[2]>;
def R3  : Register<"r3">, DwarfRegNum<[3]>;
def R4  : Register<"r4">, DwarfRegNum<[4]>;
def R5  : Register<"r5">, DwarfRegNum<[5]>;
def R6  : Register<"r5">, DwarfRegNum<[6]>;
def R7  : Register<"r7">, DwarfRegNum<[7]>;


def SP : RegisterClass<"ICMC", [i16], 16, (add R0, R1, R2, R3, R4, R5, R6, R7)>;
