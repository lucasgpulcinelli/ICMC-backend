#include "ICMCInstrInfo.h"

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "ICMCGenInstrInfo.inc"

void ICMCInstrInfo::anchor() {}

ICMCInstrInfo::ICMCInstrInfo(const ICMCSubtarget &ST)
    : ICMCGenInstrInfo(), RI(ST) {}

