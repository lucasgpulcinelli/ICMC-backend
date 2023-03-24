#ifndef LLVM_ICMC_SUBTARGET_H
#define LLVM_ICMC_SUBTARGET_H

#include "ICMCInstrInfo.h"
#include "ICMCFrameLowering.h"
#include "ICMCISelLowering.h"

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"


#define GET_SUBTARGETINFO_HEADER
#include "ICMCGenSubtargetInfo.inc"

namespace llvm {


class ICMCSubtarget : public ICMCGenSubtargetInfo {
  virtual void anchor();
  ICMCInstrInfo InstrInfo;
  ICMCFrameLowering FrameLowering;
  ICMCTargetLowering TLInfo;
  SelectionDAGTargetInfo TSInfo;

public:
  ICMCSubtarget(const Triple &TT, const std::string &CPU,
                const std::string &FS, const TargetMachine &TM);

  void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);

  const ICMCInstrInfo *getInstrInfo() const override { return &InstrInfo; }

  const ICMCFrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }

  const ICMCTargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }

  const ICMCRegisterInfo *getRegisterInfo() const override {
    return &InstrInfo.getRegisterInfo();
  }

  const SelectionDAGTargetInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
  }
};

} // end namespace llvm

#endif
