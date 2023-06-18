
// the return calling convention: use only the r0 register to return data.
def RetCC_ICMC_BUILTIN : CallingConv<[
  CCIfType<[i16], CCAssignToReg<[R0]>>
]>;

// CalleeSavedRegisters: r4-r7 can be used between calls (are usually pushed
// and popped, or not used)
def CSR_Normal : CalleeSavedRegs<(add (sequence "R%u", 4, 7))>;

