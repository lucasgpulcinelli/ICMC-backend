
#include "ICMCTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

namespace llvm {
Target &getTheICMCTarget() {
    static Target TheICMCTarget;
    return TheICMCTarget;
}
} // namespace llvm

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeICMCTargetInfo() {
    llvm::RegisterTarget<llvm::Triple::icmc> X(llvm::getTheICMCTarget(), "icmc", "ICMC's architecture", "ICMC");
}
