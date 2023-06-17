#ifndef LLVM_ICMC_ISEL_LOWERING_H
#define LLVM_ICMC_ISEL_LOWERING_H

#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

namespace ICMCISD {

// enum NodeType defines custom DAG node types for our architecture. All of
// these are in ICMCInstrFormats.td.
enum NodeType {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,

  RET_FLAG,
  CALL,
  CALLSEQ_END,
  CMP,
  SELECT_CC,
  BRCOND,
  WRAPPER
};

} // end namespace ICMCISD

class ICMCSubtarget;
class ICMCTargetMachine;

/*
 * ICMCTargetLowering: defines how to transform the high level IR based DAG into
 * a suitable DAG for instruction selection. This is where most of the quirks of
 * the architecture shine during compilation.
 */
class ICMCTargetLowering : public TargetLowering {
private:
  const ICMCSubtarget &Subtarget;

public:
  // ICMCTargetLowering is a constructor that defines which IR operations are
  // directly supported (legal) and with which types, which should be expanded
  // into simpler operations, and which have custom behaviour (do not have a one
  // to one match with IR, but can be changed to have). Note that only some
  // operations can be expanded.
  explicit ICMCTargetLowering(const TargetMachine &TM,
                              const ICMCSubtarget &Subtarget);

  // getTargetNodeName gets the name, as a string, for custom nodes defined in
  // the target. Is used for debug output during the whole compilation.
  const char *getTargetNodeName(unsigned Opcode) const override;

  // analyzeArguments allocs registers and the stack for use as arguments in a
  // function. Is used by LowerFormalArguments.
  template <typename T>
  static void analyzeArguments(const Function *F, const DataLayout *TD,
                               const SmallVectorImpl<T> &Args,
                               SmallVectorImpl<CCValAssign> &ArgLocs,
                               CCState &CCInfo);

  // LowerFormalArguments how a function receives the arguments from a call: it
  // allocates the stack and registers, and says to the function recieving these
  // arguments where they are.
  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  // LowerReturn does the inverse of LowerFormalArguments: it allocates the
  // return register and creates the root of the DAG, aka the RET_FLAG node.
  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;

  // LowerCall defines how a function is called. This is one of the main DAG
  // creation function.
  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  // lowerCallResult is a part of LowerCall, doing the process of recieving the
  // result of the Called function.
  SDValue lowerCallResult(SDValue Chain, SDValue InFlag,
                          CallingConv::ID CallConv, bool IsVarArg,
                          const SmallVectorImpl<ISD::InputArg> &Ins,
                          const SDLoc &DL, SelectionDAG &DAG,
                          SmallVectorImpl<SDValue> &InVals) const;

  // LowerOperation is the hook for lowering custom DAG nodes. This function, as
  // well as the tablegen that matches to these custom nodes, generate the main
  // quirks for the architecture, such as conditional branches, logical
  // operators on registers, and other things that are usually not one to one
  // with IR.
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  // getRegForInlineAsmConstraint is used during compilation of inline assembly,
  // responsible for assigning the register class for a certain asm constraint.
  std::pair<unsigned, const TargetRegisterClass *>
  getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                               StringRef Constraint, MVT VT) const override;

  // getConstraintType is used during compilation of inline assembly, matching
  // the constraint character to the type of constraint (for instance, r is a
  // register, i is an immediate, etcetera). These, even the simple ones, need
  // to be defined in the specific architecture because we are dealing with
  // assembly with maybe many register classes to choose from, immediates with
  // different sizes, etc.
  ConstraintType getConstraintType(StringRef Constraint) const override;

  // LowerAsmOperandForConstraint is the main inline asm lowering method, used
  // for lowering the operands of an instruction for use in the inline asm node.
  void LowerAsmOperandForConstraint(SDValue Op, std::string &Constraint,
                                    std::vector<SDValue> &Ops,
                                    SelectionDAG &DAG) const override;

  // EmitInstrWithCustomInserter is a hook to, as if the node was a pseudo
  // instruction after ISEL, expland a node into a more complex series of
  // operations.
  MachineBasicBlock *
  EmitInstrWithCustomInserter(MachineInstr &MI,
                              MachineBasicBlock *MBB) const override;

private:
  // lowerX is the method called by LowerOperation for the X custom node.

  SDValue lowerSETCC(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerSELECTCC(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerBRCC(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;

  // getICMCCmp gets the node for a compare, and changes the ICMCcc (the type of
  // comparision) to the type accepted by the CMP node returned. Is used in
  // custom nodes' lowerX that deal with compares.
  SDValue getICMCCmp(SDValue LHS, SDValue RHS, ISD::CondCode CC,
                     SDValue &ICMCcc, SelectionDAG &DAG, SDLoc DL) const;
};

} // end namespace llvm

#endif
