#include "ICMCTargetMachine.h"
#include "ICMCTargetInfo.h"

#include "llvm/Target/TargetMachine.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

static const char* ICMCDataLayout = "E-p:16:16";

ICMCTargetMachine::ICMCTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                  StringRef FS, const TargetOptions &Options,
                  Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                  CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, ICMCDataLayout, TT, CPU, FS, Options,
            RM.value_or(Reloc::Static),
            getEffectiveCodeModel(CM, CodeModel::Small), OL) {
    initAsmInfo();
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeICMCTarget(){
    RegisterTargetMachine<ICMCTargetMachine> X(getTheICMCTarget());
}

