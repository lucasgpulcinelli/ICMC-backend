#ifndef LLVM_ICMC_MACHINE_FUNCTION_INFO_H
#define LLVM_ICMC_MACHINE_FUNCTION_INFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

class ICMCMachineFunctionInfo : public MachineFunctionInfo {
  unsigned CalleeSavedFrameSize = 0;
  
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
};

} // end namespace llvm

#endif
