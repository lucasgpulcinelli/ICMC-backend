#ifndef LLVM_ICMC_ASM_BACKEND_H
#define LLVM_ICMC_ASM_BACKEND_H

#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCAsmBackend.h"

namespace llvm {

class MCAssembler;
class MCContext;
struct MCFixupKindInfo;

/*
 * ICMCAsmBackend: defines the general information regarding the final object
 * file the assembler will produce. In our case, most of the information is
 * relayed to the ELFObjectWriter.
 */
class ICMCAsmBackend : public MCAsmBackend {
public:
  ICMCAsmBackend(Triple::OSType OSType)
      : MCAsmBackend(support::big), OSType(OSType) {}

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override;

  /*
   * applies a fixup, meaning a change in the binary code (via Data). Usually
   * the Fixups are memory locations, for instance a jump or load of global
   * variable, that is added at the final stages of compilation because the
   * compiler may not be able to know the value at the time (imagine a call to a
   * function that was declared, but not implemented yet).
   */
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
