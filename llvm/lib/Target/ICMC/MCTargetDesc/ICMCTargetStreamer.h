#ifndef LLVM_ICMC_TARGET_STREAMER_H
#define LLVM_ICMC_TARGET_STREAMER_H

#include "llvm/MC/MCStreamer.h"

namespace llvm {

class ICMCTargetStreamer : public MCTargetStreamer {
public:
  ICMCTargetStreamer(MCStreamer &S);
  ~ICMCTargetStreamer() override;
};

} // end namespace llvm

#endif
