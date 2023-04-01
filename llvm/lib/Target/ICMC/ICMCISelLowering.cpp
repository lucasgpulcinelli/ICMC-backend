#include "ICMCISelLowering.h"
#include "ICMCSubtarget.h"
#include "ICMCRegisterInfo.h"

#include "llvm/CodeGen/CallingConvLower.h"

using namespace llvm;

#include "ICMCGenCallingConv.inc"

ICMCTargetLowering::ICMCTargetLowering(const TargetMachine &TM,
                                       const ICMCSubtarget &Subtarget)
      : TargetLowering(TM) {

    addRegisterClass(MVT::i16, &ICMC::GPRRegClass);

    computeRegisterProperties(Subtarget.getRegisterInfo());

    setOperationAction(ISD::ADD, MVT::i16, Legal);
}

SDValue ICMCTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> & Ins, const SDLoc & DL,
    SelectionDAG & DAG, SmallVectorImpl<SDValue> & InVals) const {

  MachineFunction &MF = DAG.getMachineFunction();

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), ArgLocs,
      *DAG.getContext());

  SDValue ArgValue;
  for (auto &VA : ArgLocs) {
    if (VA.isRegLoc()) {
      EVT RegVT = VA.getLocVT();
      const TargetRegisterClass *RC = &ICMC::GPRRegClass;
      Register Reg = MF.addLiveIn(VA.getLocReg(), RC);
      ArgValue = DAG.getCopyFromReg(Chain, DL, Reg, RegVT);

      switch(VA.getLocInfo()){
        default:
          llvm_unreachable("most LocInfos not implemented");
        case CCValAssign::Full:
          break;
      }

      InVals.push_back(ArgValue);
    } else {
      assert(VA.isMemLoc());
      llvm_unreachable("MemLoc formal argument lowering not implemented");
    }
  }

  return Chain;
}

SDValue ICMCTargetLowering::LowerReturn(SDValue Chain,
    CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::OutputArg> & Outs,
    const SmallVectorImpl<SDValue> & OutVals,
    const SDLoc & DL, SelectionDAG & DAG) const {

  SmallVector<CCValAssign, 16> RVLocs;
  MachineFunction &MF = DAG.getMachineFunction();

  CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, *DAG.getContext());

  CCInfo.AnalyzeReturn(Outs, RetCC_ICMC_BUILTIN);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  for (unsigned i = 0; i != RVLocs.size(); i++) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Flag);

    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain;

  if (Flag.getNode()) {
    RetOps.push_back(Flag);
  }

  return DAG.getNode(ICMCISD::RET_FLAG, DL, MVT::Other, RetOps);
}

