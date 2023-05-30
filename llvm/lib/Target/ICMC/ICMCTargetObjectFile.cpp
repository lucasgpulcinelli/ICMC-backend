#include "ICMCTargetObjectFile.h"

using namespace llvm;

void ICMCTargetObjectFile::Initialize(MCContext &Ctx, const TargetMachine &TM) {
  Base::Initialize(Ctx, TM);
}

MCSection *ICMCTargetObjectFile::SelectSectionForGlobal(
    const GlobalObject *GO, SectionKind Kind, const TargetMachine &TM) const {
  return Base::SelectSectionForGlobal(GO, Kind, TM);
}
