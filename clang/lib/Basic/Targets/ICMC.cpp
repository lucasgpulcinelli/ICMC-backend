//===--- ICMC.cpp - Implement ICMC target feature support -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements ICMCTargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "ICMC.h"
#include "clang/Basic/MacroBuilder.h"
#include "llvm/ADT/StringSwitch.h"

using namespace clang;
using namespace clang::targets;

const char *const ICMCTargetInfo::GCCRegNames[] = {
    // Integer registers
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
};

const TargetInfo::GCCRegAlias GCCRegAliases[] = {
    {{"R0"}, "r0"}, {{"R1"}, "r1"}, {{"R2"}, "r2"}, {{"R3"}, "r3"},
    {{"R4"}, "r4"}, {{"R5"}, "r5"}, {{"R6"}, "r6"}, {{"R7"}, "r7"},
};

ArrayRef<const char *> ICMCTargetInfo::getGCCRegNames() const {
  return llvm::makeArrayRef(GCCRegNames);
}

ArrayRef<TargetInfo::GCCRegAlias> ICMCTargetInfo::getGCCRegAliases() const {
  return llvm::makeArrayRef(GCCRegAliases);
}

void ICMCTargetInfo::getTargetDefines(const LangOptions &Opts,
                                      MacroBuilder &Builder) const {
  // Define the __ICMC__ macro when building for this target
  Builder.defineMacro("__ICMC__");
}