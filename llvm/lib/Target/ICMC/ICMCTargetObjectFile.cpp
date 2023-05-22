#include "ICMCTargetObjectFile.h"

using namespace llvm;

void ICMCTargetObjectFile::Initialize(MCContext &ctx, const TargetMachine &TM) {
  Base::Initialize(ctx, TM);
}

MCSection *ICMCTargetObjectFile::SelectSectionForGlobal(
    const GlobalObject *GO, SectionKind Kind, const TargetMachine &TM) const {
  return Base::SelectSectionForGlobal(GO, Kind, TM);
}

