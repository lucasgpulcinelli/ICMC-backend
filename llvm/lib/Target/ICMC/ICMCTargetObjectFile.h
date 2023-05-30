#ifndef LLVM_ICMC_TARGET_OBJECT_FILE_H
#define LLVM_ICMC_TARGET_OBJECT_FILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/MC/SectionKind.h"

namespace llvm {

class ICMCTargetObjectFile : public TargetLoweringObjectFileELF {
  typedef TargetLoweringObjectFileELF Base;

public:
  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;

  MCSection *SelectSectionForGlobal(const GlobalObject *GO, SectionKind Kind,
                                    const TargetMachine &TM) const override;
};

} // end namespace llvm

#endif
