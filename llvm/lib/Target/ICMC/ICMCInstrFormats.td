
//------------------------------------------------------------------------------
//-operand definitions----------------------------------------------------------
//------------------------------------------------------------------------------
// these define the types of operands the architecture have.


// generic integer immediate
def uimm16 : Operand<i16>, ImmLeaf<i16, [{return isInt<16>(Imm);}]>;

// memory immediate
def memuimm16 : Operand<iPTR>, ImmLeaf<iPTR, [{return isInt<16>(Imm);}]> {
  let EncoderMethod = "encodeMemoryLabel";
}

// jump destination operand
def jbb_16 : Operand<OtherVT>;

//------------------------------------------------------------------------------
//-pattern leaves---------------------------------------------------------------
//------------------------------------------------------------------------------
// these define special DAG pattern values.


// leaves to identify types of conditions, used in the custom nodes related to
// conditions
def ICMC_COND_EQ : PatLeaf<(i16 0)>;
def ICMC_COND_NE : PatLeaf<(i16 1)>;
def ICMC_COND_GR : PatLeaf<(i16 2)>;
def ICMC_COND_LT : PatLeaf<(i16 3)>;
def ICMC_COND_GE : PatLeaf<(i16 4)>;
def ICMC_COND_LE : PatLeaf<(i16 5)>;

//------------------------------------------------------------------------------
//-complex patterns-------------------------------------------------------------
//------------------------------------------------------------------------------
// these define input DAG patterns for operands that need special
// transformations during the DAG creation phase.


// stack memory frame index pattern (to use MOV X, SP and an ADD)
def spmem : ComplexPattern<i16, 1, "selectSPmem", [frameindex]>;

//------------------------------------------------------------------------------
//-custom nodes and type profiles-----------------------------------------------
//------------------------------------------------------------------------------
// these define nodes that need special transformations during the lowering
// phases (matching some IR node marked as "Custom" or architecture specific
// nodes).


// return statement flag
def ICMCretflag : SDNode<"ICMCISD::RET_FLAG", SDTNone,
                         [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;


// calls only need one argument: the location to jump to.
// TODO: we can use something like in the JMPs to add conditional calls, but
// this would be a minor optimization.
def SDT_ICMCCall : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;

// ICMCCall defines a function call node
def ICMCcall : SDNode<"ICMCISD::CALL", SDT_ICMCCall, [SDNPHasChain,
                      SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;


// TODO: I think CallSeqEnd uses only one argument, see what's up with that
def SDT_ICMCCallSeqEnd : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;

// ICMCCallSeqEnd defines what happens right after a call returns: clean up
// the stack arguments we sent to the function.
def ICMCCallseqEnd : SDNode<"ISD::CALLSEQ_END", SDT_ICMCCallSeqEnd,
                            [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;


// brcond has two operands: a jbb_16 with the location to jump to, and a
// condition for jumping
def SDT_ICMCBrcond
    : SDTypeProfile<0, 2, [SDTCisVT<0, OtherVT>, SDTCisVT<1, i16>]>;

// ICMCbrcond defines all conditional branch nodes.
def ICMCbrcond
    : SDNode<"ICMCISD::BRCOND", SDT_ICMCBrcond, [SDNPHasChain, SDNPInGlue]>;


// cmp has two operands of the same type: the things we are comparing
def SDT_ICMCCmp : SDTypeProfile<0, 2, [SDTCisSameAs<0, 1>]>;

// ICMCCmp defines a standalone comparision between two values.
def ICMCCmp : SDNode<"ICMCISD::CMP", SDT_ICMCCmp, [SDNPOutGlue]>;


// selectCC has three operands: the two things we are comparing and the logical
// operator
def SDT_ICMCSelectCC
    : SDTypeProfile<1, 3,
                    [SDTCisSameAs<0, 1>, SDTCisSameAs<1, 2>, SDTCisSameAs<2, 3>]>;

// ICMCselectcc defines a comparision with a certain operation, for instance
// r0, r1, <greater than>, and returns 1 if the condition is met and 0
// otherwise. This differs from ICMCCmp because it has information of the
// operation, and is essential for logical comparisions to be implemented.
def ICMCselectcc : SDNode<"ICMCISD::SELECT_CC", SDT_ICMCSelectCC, [SDNPInGlue]>;


// the wrapper has one operand: the memory location it holds.
def SDT_ICMCWrapper: SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>, SDTCisPtrTy<0>]>;

// ICMCWrapper defines a special node for use with global addresses.
def ICMCWrapper : SDNode<"ICMCISD::WRAPPER", SDT_ICMCWrapper>;

//------------------------------------------------------------------------------
//-the main stuff: instruction classes------------------------------------------
//------------------------------------------------------------------------------
// these define the instruction classes used in ICMCInstrInfo.td, as well as
// the binary formats for these kinds of instructions.
// TODO: we can simplify these classes, most of the specialized ones (except
// shifts) could possibly be deleted entirely with better code in the InstrInfo
// tablegen.


// ICMCInst defines a generic instruction, having an asm representation with
// operands, input and output DAG operands, and selection patterns from IR.
class ICMCInst<string asmStr, dag outs, dag ins,
               list<dag> pattern = []> : Instruction {
  let Namespace = "ICMC";
  let AsmString = asmStr;
  dag InOperandList = ins;
  dag OutOperandList = outs;
  list<dag> Pattern = pattern;
}

// ICMC16Inst defines a simple 16 bit instruction, with a certain opcode.
class ICMC16Inst<bits<6> opcode, string asmStr, dag outs, dag ins,
                 list<dag> pattern = []>
    : ICMCInst<asmStr, outs, ins, pattern> {
  bits<16> Inst;

  let Size = 2;
  let Inst{15-10} = opcode;
}

// ICMC32Inst defines a simple 32 bit instruction, with a certain opcode and
// immediate (because all 32 instruction have one).
class ICMC32Inst<bits<6> opcode, string asmStr, dag outs, dag ins,
                 list<dag> pattern = []>
    : ICMCInst<asmStr, outs, ins, pattern> {
  bits<32> Inst;
  bits<16> imm;

  let Size = 4;
  let Inst{31-26} = opcode;
  let Inst{15-0} = imm;
}

// ICMCLoadStoreInstr defines a load or store immediate instruction.
class ICMCLoadStoreInstr<bits<6> opcode, string asmStr, dag outs, dag ins,
                         list<dag> pattern = []>
    : ICMC32Inst<opcode, asmStr, outs, ins, pattern> {
  bits<3> reg;

  let Inst{25-23} = reg;
  let Inst{22-16} = 0;
}

// ICMCShiftInstr defines a shift-like instruction, with an input register, a 4
// bit immediate, and an output register.
class ICMCShiftInstr<bits<3> subOpcode, string Mnemonic,
                     list<dag> pattern = []>
    : ICMC16Inst<0b010000, Mnemonic # " $rd, $imm", (outs GPR:$rd),
                 (ins GPR:$rs, uimm16:$imm), pattern> {
  bits<3> rs;
  bits<3> rd;
  bits<4> imm;

  let Inst{9-7} = rd;
  let Inst{6-4} = subOpcode;
  let Inst{3-0} = imm;
}

// ICMCNoParams16Instr is the simplest instruction kind: one that has just an
// opcode and no operands.
class ICMCNoParams16Instr<bits<6> opcode, string Mnemonic,
                          list<dag> pattern = []>
    : ICMC16Inst<opcode, Mnemonic, (outs), (ins), pattern> {
  let Inst{9-0} = 0;
}

// ICMC1Reg16Instr is an instruction with a single output register.
class ICMC1Reg16Instr<bits<6> opcode, string Mnemonic,
                      list<dag> pattern = []>
    : ICMC16Inst<opcode, Mnemonic # " $reg", (outs GPR:$reg), (ins),
                 pattern> {
  bits<3> reg;

  let Inst{9-7} = reg;
  let Inst{6-0} = 0;
}

// ICMC2Regs16Instr is an instruction with one input and one output registers.
class ICMC2Regs16Instr<bits<6> opcode, string Mnemonic, list<dag> pattern = []>
    : ICMC16Inst<opcode, Mnemonic # " $rd, $rs", (outs GPR:$rd), (ins GPR:$rs),
                 pattern> {
  bits<3> rd;
  bits<3> rs;

  let Inst{9-7} = rd;
  let Inst{6-4} = rs;
  let Inst{3-0} = 0;
}

// ICMCALUInstr is the same as a "3RegsInstr", with two input registers and one
// output.
class ICMCALUInstr<bits<6> opcode, string Mnemonic,
                   list<dag> pattern = []>
    : ICMC16Inst<opcode, Mnemonic # " $rd, $rs1, $rs2", (outs GPR:$rd),
                 (ins GPR:$rs1, GPR:$rs2), pattern>{
  bits<3> rd;
  bits<3> rs1;
  bits<3> rs2;

  let Inst{9-7} = rd;
  let Inst{6-4} = rs1;
  let Inst{3-1} = rs2;
  let Inst{0} = 0;
}

// ICMCBranchInstr is a generic branch (jmp/call) instruction, with an opcode,
// subOpcode and memory label immediate.
class ICMCBranchInstr<bits<6> opcode, bits<4> subOpcode, string Mnemonic,
                      dag outs, dag ins, list<dag> pattern = []>
    : ICMC32Inst<opcode, Mnemonic # " $imm", outs, ins,
                 pattern> {
  let Inst{31-26} = opcode;
  let Inst{25-22} = subOpcode;
  let Inst{21-16} = 0;
}

// ICMCJumpInstr is a generic jump instruction.
class ICMCJumpInstr<bits<4> subOpcode, string Mnemonic, list<dag> pattern = []>
    : ICMCBranchInstr<0b000010, subOpcode, Mnemonic, (outs),
                      (ins jbb_16:$imm), pattern> {
  let isBarrier = 1;
  let isBranch = 1;
  let isTerminator = 1;
}

// ICMCCallInstr is a generic call instruction.
class ICMCCallInstr<bits<4> subOpcode, string Mnemonic, list<dag> pattern = []>
    : ICMCBranchInstr<0b000011, subOpcode, Mnemonic, (outs),
                      (ins memuimm16:$imm), pattern> {
  let isCall = 1;
}

