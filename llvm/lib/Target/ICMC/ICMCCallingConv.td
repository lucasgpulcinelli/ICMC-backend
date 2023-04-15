

def RetCC_ICMC_BUILTIN : CallingConv<[
  CCIfType<[i16], CCAssignToReg<[R0]>>
]>;

def CSR_Normal : CalleeSavedRegs<(add (sequence "R%u", 4, 7))>;

