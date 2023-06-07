#ifndef LLVM_ICMC_MACHINE_FUNCTION_INFO_H
#define LLVM_ICMC_MACHINE_FUNCTION_INFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

/*
 * ICMCMachineFunctionInfo: defines extra information each function should have.
 * Used in the FrameAnalyzerPass to determine if a function should have a frame
 * index, for an optimization regarding frame sizes.
 */
class ICMCMachineFunctionInfo : public MachineFunctionInfo {
  unsigned CalleeSavedFrameSize = 0;
  bool HasFrameIndex = false;

public:
  explicit ICMCMachineFunctionInfo(MachineFunction &MF) {}

  MachineFunctionInfo *
  clone(BumpPtrAllocator &Allocator, MachineFunction &DestMF,
        const DenseMap<MachineBasicBlock *, MachineBasicBlock *> &Src2DstMBB)
      const override {
    return DestMF.cloneInfo<ICMCMachineFunctionInfo>(*this);
  }

  unsigned getCalleeSavedFrameSize() const { return CalleeSavedFrameSize; }
  void setCalleeSavedFrameSize(unsigned Bytes) { CalleeSavedFrameSize = Bytes; }
  void setHasFrameIndex() { HasFrameIndex = true; }
  bool hasFrameIndex() const { return HasFrameIndex; }
};

} // end namespace llvm

#endif
