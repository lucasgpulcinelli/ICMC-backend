include "llvm/Target/Target.td"

include "ICMCRegisterInfo.td"
include "ICMCInstrInfo.td"


def ICMCInstrInfo : InstrInfo;

def : ProcessorModel<"generic",  NoSchedModel, []>;

def ICMC : Target {
  let InstructionSet = ICMCInstrInfo;
}
