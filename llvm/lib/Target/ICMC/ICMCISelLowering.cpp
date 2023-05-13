#include "ICMCISelLowering.h"
#include "ICMCSubtarget.h"
#include "ICMCRegisterInfo.h"

#include "llvm/CodeGen/CallingConvLower.h"

using namespace llvm;

#include "ICMCGenCallingConv.inc"

static const MCPhysReg GPRRegList[] = {ICMC::R0, ICMC::R1, ICMC::R2, ICMC::R3};

ICMCTargetLowering::ICMCTargetLowering(const TargetMachine &TM,
                                       const ICMCSubtarget &Subtarget)
      : TargetLowering(TM), Subtarget(Subtarget) {

  addRegisterClass(MVT::i16, &ICMC::GPRRegClass);

  computeRegisterProperties(Subtarget.getRegisterInfo());

  setOperationAction(ISD::ADDC, MVT::i16, Legal);
  setOperationAction(ISD::SUBC, MVT::i16, Legal);
  setOperationAction(ISD::ADDE, MVT::i16, Legal);
  setOperationAction(ISD::SUBE, MVT::i16, Legal);

  setOperationAction(ISD::SRA, MVT::i4, Legal);
  setOperationAction(ISD::SHL, MVT::i4, Legal);
  setOperationAction(ISD::ROTL, MVT::i4, Legal);
  setOperationAction(ISD::ROTR, MVT::i4, Legal);

  setOperationAction(ISD::SRA, MVT::i16, Custom);
  setOperationAction(ISD::SHL, MVT::i16, Custom);
  setOperationAction(ISD::ROTL, MVT::i16, Custom);
  setOperationAction(ISD::ROTR, MVT::i16, Custom);

  setOperationAction(ISD::SDIV, MVT::i16, Expand);
  setOperationAction(ISD::SREM, MVT::i16, Expand);
  setOperationAction(ISD::SDIVREM, MVT::i16, Expand);
  setOperationAction(ISD::UDIVREM, MVT::i16, Expand);
  setOperationAction(ISD::UDIV, MVT::i16, Legal);
  setOperationAction(ISD::UREM, MVT::i16, Legal);

  setOperationAction(ISD::MUL, MVT::i16, Legal);

  setOperationAction(ISD::SETCC, MVT::i16, Custom);
  setOperationAction(ISD::SELECT_CC, MVT::i16, Custom);
  setOperationAction(ISD::BR_CC, MVT::i16, Custom);
}

template<typename T>
void ICMCTargetLowering::analyzeArguments(
    const Function *F, const DataLayout *TD,
    const SmallVectorImpl<T> &Args,
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
            "calling convention can only manage i16 types");
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

SDValue ICMCTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
    SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &DL = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  CallingConv::ID CallConv = CLI.CallConv;
  bool IsVarArg = false; //no support for var args

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());


  const Function *F = nullptr;
  if (const GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee)) {
    const GlobalValue *GV = G->getGlobal();
    if (isa<Function>(GV))
      F = cast<Function>(GV);
    Callee =
        DAG.getTargetGlobalAddress(GV, DL, getPointerTy(DAG.getDataLayout()));
  } else if (const ExternalSymbolSDNode *ES =
                 dyn_cast<ExternalSymbolSDNode>(Callee)) {
    Callee = DAG.getTargetExternalSymbol(ES->getSymbol(),
                                         getPointerTy(DAG.getDataLayout()));
  }

  analyzeArguments(F, &DAG.getDataLayout(), Outs, ArgLocs, CCInfo);

  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;

  unsigned AI, AE;
  bool HasStackArgs = false;
  for (AI = 0, AE = ArgLocs.size(); AI != AE; ++AI) {
    CCValAssign &VA = ArgLocs[AI];
    EVT RegVT = VA.getLocVT();
    SDValue Arg = OutVals[AI];

    // Promote the value if needed. With Clang this should not happen.
    switch (VA.getLocInfo()) {
    default:
      llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full:
      break;
    case CCValAssign::SExt:
      Arg = DAG.getNode(ISD::SIGN_EXTEND, DL, RegVT, Arg);
      break;
    case CCValAssign::ZExt:
      Arg = DAG.getNode(ISD::ZERO_EXTEND, DL, RegVT, Arg);
      break;
    case CCValAssign::AExt:
      Arg = DAG.getNode(ISD::ANY_EXTEND, DL, RegVT, Arg);
      break;
    case CCValAssign::BCvt:
      Arg = DAG.getNode(ISD::BITCAST, DL, RegVT, Arg);
      break;
    }

    // Stop when we encounter a stack argument, we need to process them
    // in reverse order in the loop below.
    if (VA.isMemLoc()) {
      HasStackArgs = true;
      break;
    }

    // Arguments that can be passed on registers must be kept in the RegsToPass
    // vector.
    RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
  }

  SmallVector<SDValue, 8> MemOpChains;
  if (HasStackArgs) {
    MachineFunction &MF = DAG.getMachineFunction();

    for (; AI != AE; AI++) {
      CCValAssign &VA = ArgLocs[AI];
      SDValue Arg = OutVals[AI];

      assert(VA.isMemLoc());

      MemOpChains.push_back(
          DAG.getStore(Chain, DL, Arg, DAG.getRegister(ICMC::SP, getPointerTy(DAG.getDataLayout())),
                       MachinePointerInfo::getStack(MF, VA.getLocMemOffset())));
    }
  }

  // Build a sequence of copy-to-reg nodes chained together with token chain and
  // flag operands which copy the outgoing args into registers.  The InFlag in
  // necessary since all emited instructions must be stuck together.
  SDValue InFlag;
  for (auto Reg : RegsToPass) {
    Chain = DAG.getCopyToReg(Chain, DL, Reg.first, Reg.second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // Returns a chain & a flag for retval copy to use.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are known live
  // into the call.
  for (auto Reg : RegsToPass) {
    Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));
  }

  // Add a register mask operand representing the call-preserved registers.
  const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
  const uint32_t *Mask =
      TRI->getCallPreservedMask(DAG.getMachineFunction(), CallConv);
  assert(Mask && "Missing call preserved mask for calling convention");
  Ops.push_back(DAG.getRegisterMask(Mask));

  if (InFlag.getNode()) {
    Ops.push_back(InFlag);
  }

  Chain = DAG.getNode(ICMCISD::CALL, DL, NodeTys, Ops);
  InFlag = Chain.getValue(1);

  if(ArgLocs.size() > 4){
    if (!MemOpChains.empty()) {
      Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);
    }

    Chain = DAG.getCALLSEQ_END(
        Chain, DAG.getIntPtrConstant(ArgLocs.size() - 4, DL, true),
        DAG.getUNDEF(MVT::i16), InFlag, DL);

    if (!Ins.empty()) {
      InFlag = Chain.getValue(1);
    }
  }


  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return lowerCallResult(Chain, InFlag, CallConv, false, Ins, DL, DAG,
                         InVals);
}


SDValue ICMCTargetLowering::lowerCallResult(
    SDValue Chain, SDValue InFlag, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_ICMC_BUILTIN);

  // Copy all of the result registers out of their specified physreg.
  for (CCValAssign const &RVLoc : RVLocs) {
    Chain = DAG.getCopyFromReg(Chain, DL, RVLoc.getLocReg(), RVLoc.getValVT(),
                               InFlag)
                .getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}

std::pair<unsigned, const TargetRegisterClass *>
ICMCTargetLowering::getRegForInlineAsmConstraint(
    const TargetRegisterInfo *TRI, StringRef Constraint, MVT VT) const {

  if(Constraint.size() == 1 && Constraint[0] == 'r'){
    return std::make_pair(0U, &ICMC::GPRRegClass);
  }

  return TargetLowering::getRegForInlineAsmConstraint(
      Subtarget.getRegisterInfo(), Constraint, VT);
}


ICMCTargetLowering::ConstraintType
ICMCTargetLowering::getConstraintType(StringRef Constraint) const {
  if (Constraint.size() == 1) {
    // See http://www.nongnu.org/avr-libc/user-manual/inline_asm.html
    switch (Constraint[0]) {
    default:
      break;
    case 'r': // Any register
      return C_RegisterClass;
    case 'i': // 16-bit unsigned constant
      return C_Immediate;
    }
  }

  return TargetLowering::getConstraintType(Constraint);
}


void ICMCTargetLowering::LowerAsmOperandForConstraint(
    SDValue Op, std::string &Constraint, std::vector<SDValue> &Ops,
    SelectionDAG &DAG) const {

  SDValue Result;
  SDLoc DL(Op);
  EVT Ty = Op.getValueType();

  const ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op);
  if (!C) {
    return;
  }

  uint64_t CUVal64 = C->getZExtValue();

  if(Constraint[0] == 'i') {
    if (!isUInt<16>(CUVal64)) {
      return;
    }
  } else {
    return;
  }

  Result = DAG.getTargetConstant(CUVal64, DL, Ty);
  if (Result.getNode()) {
    Ops.push_back(Result);
    return;
  }

  return TargetLowering::LowerAsmOperandForConstraint(Op, Constraint, Ops, DAG);
}

SDValue ICMCTargetLowering::LowerOperation(SDValue Op,
                                           SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  case ISD::SETCC:
    return LowerSETCC(Op, DAG);
  case ISD::SELECT_CC:
    return LowerSELECT_CC(Op, DAG);
  case ISD::BR_CC:
    return LowerBR_CC(Op, DAG);
  default:
    llvm_unreachable("LowerOperation not implemented for this opcode!");
  }
}

static ICMCCC::CondCodes intCCToICMCCC(ISD::CondCode CC) {
  switch (CC) {
  default:
    printf("%d\n", CC);
    llvm_unreachable("Unknown condition code!");
  case ISD::SETEQ:
    return ICMCCC::COND_EQ;
  case ISD::SETNE:
    return ICMCCC::COND_NE;
  case ISD::SETUGT:
    return ICMCCC::COND_GT;
  case ISD::SETULT:
    return ICMCCC::COND_LT;
  case ISD::SETUGE:
    return ICMCCC::COND_GE;
  case ISD::SETULE:
    return ICMCCC::COND_LE;
  }
}

SDValue ICMCTargetLowering::getICMCCmp(SDValue LHS, SDValue RHS,
                                       ISD::CondCode CC, SDValue &ICMCcc,
                                       SelectionDAG &DAG, SDLoc DL) const {
  SDValue Cmp = DAG.getNode(ICMCISD::CMP, DL, MVT::Glue, LHS, RHS);

  ICMCcc = DAG.getConstant(intCCToICMCCC(CC), DL, MVT::i16);
  return Cmp;
}

SDValue ICMCTargetLowering::LowerSETCC(SDValue Op, SelectionDAG &DAG) const {
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(2))->get();
  SDLoc DL(Op);

  SDValue TargetCC;
  SDValue Cmp = getICMCCmp(LHS, RHS, CC, TargetCC, DAG, DL);

  SDValue TrueV = DAG.getConstant(1, DL, Op.getValueType());
  SDValue FalseV = DAG.getConstant(0, DL, Op.getValueType());
  SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
  SDValue Ops[] = {TrueV, FalseV, TargetCC, Cmp};

  return DAG.getNode(ICMCISD::SELECT_CC, DL, VTs, Ops);
}

SDValue ICMCTargetLowering::LowerSELECT_CC(SDValue Op,
                                           SelectionDAG &DAG) const {
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  SDValue TrueV = Op.getOperand(2);
  SDValue FalseV = Op.getOperand(3);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
  SDLoc dl(Op);

  SDValue TargetCC;
  SDValue Cmp = getICMCCmp(LHS, RHS, CC, TargetCC, DAG, dl);

  SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
  SDValue Ops[] = {TrueV, FalseV, TargetCC, Cmp};

  return DAG.getNode(ICMCISD::SELECT_CC, dl, VTs, Ops);
}

SDValue ICMCTargetLowering::LowerBR_CC(SDValue Op,
                                       SelectionDAG &DAG) const {
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS = Op.getOperand(2);
  SDValue RHS = Op.getOperand(3);
  SDValue Dest = Op.getOperand(4);
  SDLoc dl(Op);

  SDValue TargetCC;
  SDValue Cmp = getICMCCmp(LHS, RHS, CC, TargetCC, DAG, dl);

  return DAG.getNode(ICMCISD::BRCOND, dl, MVT::Other, Chain, Dest, TargetCC,
                     Cmp);
}

MachineBasicBlock *ICMCTargetLowering::EmitInstrWithCustomInserter(
      MachineInstr &MI, MachineBasicBlock *MBB) const {

  const ICMCInstrInfo &TII = (const ICMCInstrInfo &)*MI.getParent()
                                ->getParent()
                                ->getSubtarget()
                                .getInstrInfo();
  DebugLoc dl = MI.getDebugLoc();

  // To "insert" a SELECT instruction, we insert the diamond
  // control-flow pattern. The incoming instruction knows the
  // destination vreg to set, the condition code register to branch
  // on, the true/false values to select between, and a branch opcode
  // to use.

  MachineFunction *MF = MBB->getParent();
  const BasicBlock *LLVM_BB = MBB->getBasicBlock();
  MachineBasicBlock *FallThrough = MBB->getFallThrough();

  // If the current basic block falls through to another basic block,
  // we must insert an unconditional branch to the fallthrough destination
  // if we are to insert basic blocks at the prior fallthrough point.
  if (FallThrough != nullptr) {
    BuildMI(MBB, dl, TII.get(ICMC::JMP)).addMBB(FallThrough);
  }

  MachineBasicBlock *trueMBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *falseMBB = MF->CreateMachineBasicBlock(LLVM_BB);

  MachineFunction::iterator I;
  for (I = MF->begin(); I != MF->end() && &(*I) != MBB; ++I)
    ;
  if (I != MF->end())
    ++I;
  MF->insert(I, trueMBB);
  MF->insert(I, falseMBB);

  // Transfer remaining instructions and all successors of the current
  // block to the block which will contain the Phi node for the
  // select.
  trueMBB->splice(trueMBB->begin(), MBB,
                  std::next(MachineBasicBlock::iterator(MI)), MBB->end());
  trueMBB->transferSuccessorsAndUpdatePHIs(MBB);

  ICMCCC::CondCodes CC = (ICMCCC::CondCodes)MI.getOperand(3).getImm();
  BuildMI(MBB, dl, TII.getBrCond(CC)).addMBB(trueMBB);
  BuildMI(MBB, dl, TII.get(ICMC::JMP)).addMBB(falseMBB);
  MBB->addSuccessor(falseMBB);
  MBB->addSuccessor(trueMBB);

  // Unconditionally flow back to the true block
  BuildMI(falseMBB, dl, TII.get(ICMC::JMP)).addMBB(trueMBB);
  falseMBB->addSuccessor(trueMBB);

  // Set up the Phi node to determine where we came from
  BuildMI(*trueMBB, trueMBB->begin(), dl, TII.get(ICMC::PHI),
          MI.getOperand(0).getReg())
      .addReg(MI.getOperand(1).getReg())
      .addMBB(MBB)
      .addReg(MI.getOperand(2).getReg())
      .addMBB(falseMBB);

  MI.eraseFromParent(); // The pseudo instruction is gone now.
  return trueMBB;
}
