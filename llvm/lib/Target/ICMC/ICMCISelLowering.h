#ifndef LLVM_ICMC_ISEL_LOWERING_H
#define LLVM_ICMC_ISEL_LOWERING_H

#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

namespace ICMCISD {

enum NodeType {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,

  RET_FLAG,
  CALL,
  CALLSEQ_END
};

} // end namespace ICMCISD

class ICMCSubtarget;
class ICMCTargetMachine;

class ICMCTargetLowering : public TargetLowering {
private:
  const ICMCSubtarget& Subtarget;

public:
  explicit ICMCTargetLowering(const TargetMachine &TM,
                             const ICMCSubtarget &Subtarget);

  template<typename T>
  static void analyzeArguments(const Function *F, const DataLayout *TD,
                               const SmallVectorImpl<T> &Args,
                               SmallVectorImpl<CCValAssign> &ArgLocs,
                               CCState &CCInfo);

  SDValue LowerFormalArguments(
      SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
      const SmallVectorImpl<ISD::InputArg> & Ins, const SDLoc & DL,
      SelectionDAG & DAG, SmallVectorImpl<SDValue> & InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> & Outs,
                      const SmallVectorImpl<SDValue> & OutVals,
                      const SDLoc & DL, SelectionDAG & DAG) const override;

  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  SDValue lowerCallResult(SDValue Chain, SDValue InFlag,
                          CallingConv::ID CallConv, bool IsVarArg,
                          const SmallVectorImpl<ISD::InputArg> &Ins,
                          const SDLoc &DL, SelectionDAG &DAG,
                          SmallVectorImpl<SDValue> &InVals) const;
};

} // end namespace llvm

#endif

