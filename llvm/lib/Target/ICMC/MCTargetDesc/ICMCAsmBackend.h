#ifndef LLVM_ICMC_ASM_BACKEND_H
#define LLVM_ICMC_ASM_BACKEND_H

#include "llvm/MC/MCAsmBackend.h"
#include "llvm/ADT/Triple.h"

namespace llvm {

class MCAssembler;
class MCContext;
struct MCFixupKindInfo;

class ICMCAsmBackend : public MCAsmBackend {
public:
  ICMCAsmBackend(Triple::OSType OSType)
      : MCAsmBackend(support::little), OSType(OSType) {}

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override;

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;

  unsigned getNumFixupKinds() const override;

  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override;

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;

private:
  Triple::OSType OSType;
};

} // end namespace llvm

#endif
