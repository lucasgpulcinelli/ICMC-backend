#include "ICMCMCInstLower.h"
#include "ICMCTargetMachine.h"
#include "MCTargetDesc/ICMCInstrPrinter.h"
#include "TargetInfo/ICMCTargetInfo.h"

#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

namespace {

/*
 * ICMCAsmPrinter: defines the assembly printer, aka the class responsible for
 * driving the clang -S process and inline assembly in C. Calls the instruction
 * printer and TargetStreamer.
 */
class ICMCAsmPrinter : public AsmPrinter {
  ICMCMCInstLower MCInstLowering;
  const MCRegisterInfo &MRI;

public:
  explicit ICMCAsmPrinter(TargetMachine &TM,
                          std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)), MCInstLowering(OutContext, *this),
        MRI(*TM.getMCRegisterInfo()) {}

  StringRef getPassName() const override { return "ICMC Assembly Printer"; }
  void emitInstruction(const MachineInstr *MI) override;

  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNum,
                       const char *ExtraCode, raw_ostream &O) override;

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

// PrintAsmOperand drives the inline assembly operand substitution process
bool ICMCAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNum,
                                     const char *ExtraCode, raw_ostream &O) {
  bool Error = AsmPrinter::PrintAsmOperand(MI, OpNum, ExtraCode, O);

  if (Error) {
    const MachineOperand &MO = MI->getOperand(OpNum);

    if (MO.isReg()) {
      O << ICMCInstPrinter::getPrettyRegisterName(MO.getReg(), MRI);
    } else if (MO.isImm()) {
      O << '#' << MO.getImm();
    } else {
      llvm_unreachable("kind of operand not supported in inline asm!");
    }
  }

  return false;
}

// one of the initialization functions, gives information on the assembly
// printer
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeICMCAsmPrinter() {
  RegisterAsmPrinter<ICMCAsmPrinter> X(getTheICMCTarget());
}
