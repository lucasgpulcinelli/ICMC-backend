#include "ICMCISelLowering.h"
#include "ICMCSubtarget.h"
#include "ICMCRegisterInfo.h"

#include "llvm/CodeGen/CallingConvLower.h"

using namespace llvm;

#include "ICMCGenCallingConv.inc"

static const MCPhysReg GPRRegList[] = {ICMC::R0, ICMC::R1, ICMC::R2, ICMC::R3,
                                       ICMC::R4, ICMC::R5, ICMC::R6, ICMC::R7};

ICMCTargetLowering::ICMCTargetLowering(const TargetMachine &TM,
                                       const ICMCSubtarget &Subtarget)
      : TargetLowering(TM) {

    addRegisterClass(MVT::i16, &ICMC::GPRRegClass);

    computeRegisterProperties(Subtarget.getRegisterInfo());

    setOperationAction(ISD::ADD, MVT::i16, Legal);
}

void ICMCTargetLowering::analyzeArguments(
    const Function *F, const DataLayout *TD,
    const SmallVectorImpl<ISD::InputArg> &Args,
    SmallVectorImpl<CCValAssign> &ArgLocs, CCState &CCInfo){
  ArrayRef<MCPhysReg> RegList = makeArrayRef(GPRRegList);

  bool UseStack = false;
  int RegLastIdx = -1;
  for (unsigned i = 0; i != Args.size();) {
    MVT VT = Args[i].VT;

    unsigned ArgIndex = Args[i].OrigArgIndex;
    unsigned TotalBytes = VT.getStoreSize();
    unsigned j = i + 1;
    for (; j != Args.size(); ++j) {
      if (Args[j].OrigArgIndex != ArgIndex)
        break;
      TotalBytes += Args[j].VT.getStoreSize();
    }
    // Round up to even number of bytes.
    TotalBytes = alignTo(TotalBytes, 2);
    // Skip zero sized arguments
    if (TotalBytes == 0)
      continue;

    unsigned RegIdx = RegLastIdx + TotalBytes/2;
    RegLastIdx = RegIdx;

    if (RegIdx >= RegList.size()) {
      UseStack = true;
    }
    for (; i != j; ++i) {
      MVT VT = Args[i].VT;

      unsigned Reg;

      if(UseStack){
        auto* Evt = EVT(VT).getTypeForEVT(CCInfo.getContext());
        unsigned Offset = CCInfo.AllocateStack(TD->getTypeAllocSize(Evt),
                                               TD->getABITypeAlign(Evt));
        CCInfo.addLoc(
            CCValAssign::getMem(i, VT, Offset, VT, CCValAssign::Full));
        continue;
      }

      if (VT == MVT::i16) {
        Reg = CCInfo.AllocateReg(RegList[RegIdx]);
      } else {
        llvm_unreachable(
            "calling convention can only manage i8 and i16 types");
      }
      assert(Reg && "register not available in calling convention");
      CCInfo.addLoc(CCValAssign::getReg(i, VT, Reg, VT, CCValAssign::Full));
      RegIdx -= VT.getStoreSize();
    }
  }
}

SDValue ICMCTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> & Ins, const SDLoc & DL,
    SelectionDAG & DAG, SmallVectorImpl<SDValue> & InVals) const {

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  auto DataLayout = DAG.getDataLayout();

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), ArgLocs,
      *DAG.getContext());

  analyzeArguments(&MF.getFunction(), &DAG.getDataLayout(), Ins, ArgLocs,
                   CCInfo);

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

      EVT LocVT = VA.getLocVT();

      // Create the frame index object for this incoming parameter.
      int FI = MFI.CreateFixedObject(LocVT.getSizeInBits() / 8,
                                     VA.getLocMemOffset(), true);

      // Create the SelectionDAG nodes corresponding to a load
      // from this parameter.
      SDValue FIN = DAG.getFrameIndex(FI, getPointerTy(DataLayout));
      InVals.push_back(DAG.getLoad(LocVT, DL, Chain, FIN,
                                   MachinePointerInfo::getFixedStack(MF, FI)));
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

