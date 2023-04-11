

def RetCC_ICMC_BUILTIN : CallingConv<[
  CCIfType<[i16], CCAssignToReg<[R0, R1, R2, R3, R4, R5, R6, R7]>>
]>;

def CSR_Normal : CalleeSavedRegs<(add R0, R1, R2, R3, R4, R5, R6, R7)>;

