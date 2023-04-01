#include "TargetInfo/ICMCTargetInfo.h"
#include "ICMCMCInstLower.h"

#include "llvm/MC/TargetRegistry.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/MC/MCStreamer.h"

using namespace llvm;

namespace {

class ICMCAsmPrinter : public AsmPrinter {
  ICMCMCInstLower MCInstLowering;

public:
  explicit ICMCAsmPrinter(TargetMachine &TM,
                          std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)),
        MCInstLowering() {}

  StringRef getPassName() const override { return "ICMC Assembly Printer"; }
  void emitInstruction(const MachineInstr *MI) override;

  bool runOnMachineFunction(MachineFunction &MF) override;
};

} // end anonymous namespace


void ICMCAsmPrinter::emitInstruction(const MachineInstr *MI) {
  MCInst I;
  MCInstLowering.lower(MI, I);
  EmitToStreamer(*OutStreamer, I);
}

bool ICMCAsmPrinter::runOnMachineFunction(MachineFunction &MF) {
  return AsmPrinter::runOnMachineFunction(MF);
}


extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeICMCAsmPrinter() {
  RegisterAsmPrinter<ICMCAsmPrinter> X(getTheICMCTarget());
}

