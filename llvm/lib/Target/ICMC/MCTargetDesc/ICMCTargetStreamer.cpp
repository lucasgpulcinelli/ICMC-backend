#include "ICMCTargetStreamer.h"

using namespace llvm;

ICMCTargetStreamer::ICMCTargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

ICMCTargetStreamer::~ICMCTargetStreamer() = default;

