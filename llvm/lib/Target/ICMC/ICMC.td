include "llvm/Target/Target.td"

include "ICMCRegisterInfo.td"
include "ICMCInstrInfo.td"
include "ICMCCallingConv.td"


// ICMCAsmWriter defines the assembly writer for the architecture. Used for
// inline assembly and -S
def ICMCAsmWriter : AsmWriter {
  string AsmWriterClassName = "InstPrinter";
  bit isMCAsmWriter = 1;
}

// the instruction info instatiation itself.
def ICMCInstrInfo : InstrInfo;

// the generic processor model for ICMC, is used to define subarchitectures with
// certain capabilities, as well as optimzations for instruction scheduling and
// cycle count for instructions.
def : ProcessorModel<"generic",  NoSchedModel, []>;

// The ICMC target.
def ICMC : Target {
  let InstructionSet = ICMCInstrInfo;
}
