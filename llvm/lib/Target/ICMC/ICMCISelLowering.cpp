#include "ICMCISelLowering.h"
#include "ICMCSubtarget.h"
#include "ICMCRegisterInfo.h"

using namespace llvm;

ICMCTargetLowering::ICMCTargetLowering(const TargetMachine &TM,
                                       const ICMCSubtarget &Subtarget)
      : TargetLowering(TM) {

    addRegisterClass(MVT::i16, &ICMC::GPRRegClass);

    computeRegisterProperties(Subtarget.getRegisterInfo());

    setOperationAction(ISD::ADD, MVT::i16, Legal);
}

