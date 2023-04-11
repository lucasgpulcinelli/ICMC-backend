#ifndef LLVM_ICMC_ISEL_LOWERING_H
#define LLVM_ICMC_ISEL_LOWERING_H

#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

namespace ICMCISD {

enum NodeType {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,

  RET_FLAG
};

} // end namespace ICMCISD

class ICMCSubtarget;
class ICMCTargetMachine;

class ICMCTargetLowering : public TargetLowering {
public:
  explicit ICMCTargetLowering(const TargetMachine &TM,
                             const ICMCSubtarget &Subtarget);

  static void analyzeArguments(const Function *F, const DataLayout *TD,
                               const SmallVectorImpl<ISD::InputArg> &Args,
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
};

} // end namespace llvm

#endif

