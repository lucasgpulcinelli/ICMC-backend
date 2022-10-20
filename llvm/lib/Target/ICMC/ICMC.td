include "llvm/Target/Target.td"

include "ICMCRegisterInfo.td"
include "ICMCInstrInfo.td"
include "ICMCInstrFormats.td"
include "ICMCCallingConv.td"


def ICMCInstrInfo : InstrInfo;

def : ProcessorModel<"generic",  NoSchedModel, []>;

def ICMC : Target {
  let InstructionSet = ICMCInstrInfo;
}
