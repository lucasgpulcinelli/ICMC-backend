include "llvm/Target/Target.td"

include "ICMCRegisterInfo.td"


def ICMCInstrInfo : InstrInfo;

def : ProcessorModel<"generic",  NoSchedModel, []>;

def ICMC : Target {
  let InstructionSet = ICMCInstrInfo;
}
