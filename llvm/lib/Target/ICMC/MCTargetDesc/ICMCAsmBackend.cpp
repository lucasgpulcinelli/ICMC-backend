#include "ICMCAsmBackend.h"
#include "ICMCELFObjectWriter.h"

#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCELFObjectWriter.h"

using namespace llvm;

std::unique_ptr<MCObjectTargetWriter>
ICMCAsmBackend::createObjectTargetWriter() const {
  return createICMCELFObjectWriter(MCELFObjectTargetWriter::getOSABI(OSType));
}

void ICMCAsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
    const MCValue &Target, MutableArrayRef<char> Data, uint64_t Value,
    bool IsResolved, const MCSubtargetInfo *STI) const {
  llvm_unreachable("applyFixup not implemented");
}

unsigned ICMCAsmBackend::getNumFixupKinds() const {
  llvm_unreachable("getNumFixupKinds not implemented");
}

bool ICMCAsmBackend::fixupNeedsRelaxation(
    const MCFixup &Fixup, uint64_t Value, const MCRelaxableFragment *DF,
    const MCAsmLayout &Layout) const {
  llvm_unreachable("fixupNeedsRelaxation not implemented");
}

bool ICMCAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
    const MCSubtargetInfo *STI) const {
  assert((Count % 1) == 0 && "NOP instructions must be 1 byte");

  OS.write_zeros(Count);
  return true;
}

