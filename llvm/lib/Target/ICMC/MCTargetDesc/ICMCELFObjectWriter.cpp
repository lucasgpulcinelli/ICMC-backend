#include "ICMCELFObjectWriter.h"

#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include <cassert>


namespace llvm {

class ICMCELFObjectWriter : public MCELFObjectTargetWriter {
public:
  ICMCELFObjectWriter(uint8_t OSABI);

  virtual ~ICMCELFObjectWriter() = default;

  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};

ICMCELFObjectWriter::ICMCELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(false, OSABI, ELF::EM_ICMC, true) {}

unsigned ICMCELFObjectWriter::getRelocType(MCContext &Ctx, const MCValue &Target,
                                          const MCFixup &Fixup,
                                          bool IsPCRel) const {
  assert(Fixup.getKind() == FK_Data_2 && "invalid fixup kind");
  return ELF::R_ICMC_16;
}

std::unique_ptr<MCObjectTargetWriter> createICMCELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<ICMCELFObjectWriter>(OSABI);
}

} // end namespace llvm

