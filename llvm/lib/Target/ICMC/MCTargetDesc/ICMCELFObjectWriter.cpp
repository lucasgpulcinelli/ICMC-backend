#include "ICMCELFObjectWriter.h"

#include "llvm/MC/MCELFObjectWriter.h"


namespace llvm {

class ICMCELFObjectWriter : public MCELFObjectTargetWriter {
public:
  ICMCELFObjectWriter(uint8_t OSABI);

  virtual ~ICMCELFObjectWriter() = default;

  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};

ICMCELFObjectWriter::ICMCELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(false, OSABI, ELF::EM_AVR, true) {}

unsigned ICMCELFObjectWriter::getRelocType(MCContext &Ctx, const MCValue &Target,
                                          const MCFixup &Fixup,
                                          bool IsPCRel) const {
    llvm_unreachable("getRelocType not implemented");
}

std::unique_ptr<MCObjectTargetWriter> createICMCELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<ICMCELFObjectWriter>(OSABI);
}

} // end namespace llvm

