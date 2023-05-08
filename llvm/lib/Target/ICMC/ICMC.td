include "llvm/Target/Target.td"

include "ICMCRegisterInfo.td"
include "ICMCInstrInfo.td"
include "ICMCCallingConv.td"


def ICMCAsmWriter : AsmWriter {
  string AsmWriterClassName = "InstPrinter";
  bit isMCAsmWriter = 1;
}

def ICMCInstrInfo : InstrInfo;

def : ProcessorModel<"generic",  NoSchedModel, []>;

def ICMC : Target {
  let InstructionSet = ICMCInstrInfo;
}
