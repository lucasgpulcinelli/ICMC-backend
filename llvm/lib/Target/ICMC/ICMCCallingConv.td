

def RetCC_ICMC_BUILTIN : CallingConv<[
  CCIfType<[i16], CCAssignToReg<[R6, R7]>>
]>;

def CSR_Normal : CalleeSavedRegs<(add R6, R7)>;

