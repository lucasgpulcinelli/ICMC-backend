#include "Relocations.h"
#include "Target.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/Support/Endian.h"

using namespace llvm;
using namespace llvm::object;
using namespace llvm::ELF;
using namespace llvm::support::endian;
using namespace lld;
using namespace lld::elf;


namespace {
class ICMC final : public TargetInfo {
public:
  RelExpr getRelExpr(RelType type, const Symbol &s,
                     const uint8_t *loc) const override;
  void relocate(uint8_t *loc, const Relocation &rel,
                uint64_t val) const override;
};
} // namespace

RelExpr
ICMC::getRelExpr(RelType type, const Symbol &s, const uint8_t *loc) const {
  return R_ABS;
}

void ICMC::relocate(uint8_t *loc, const Relocation &rel, uint64_t val) const {
  switch(rel.type){
  case R_ICMC_16:
    write16be(loc, (val-0x110b4)>>1);
    break;
  }
}

TargetInfo *elf::getICMCTargetInfo() {
  static ICMC target;
  return &target;
}
