#ifndef LLVM_ICMC_INSTR_INFO_H
#define LLVM_ICMC_INSTR_INFO_H

#include "ICMCRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "ICMCGenInstrInfo.inc"

namespace llvm {

class ICMCSubtarget;

class ICMCInstrInfo : public ICMCGenInstrInfo {
  const ICMCRegisterInfo RI;
  virtual void anchor();

public:
  ICMCInstrInfo(const ICMCSubtarget &);

  const ICMCRegisterInfo &getRegisterInfo() const { return RI; }
};

} // end namespace llvm

#endif
