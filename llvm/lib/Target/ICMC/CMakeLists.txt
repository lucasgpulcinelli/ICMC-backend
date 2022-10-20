add_llvm_component_group(ICMC)

set(LLVM_TARGET_DEFINITIONS ICMC.td)

tablegen(LLVM ICMCGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM ICMCGenCallingConv.inc -gen-callingconv)
tablegen(LLVM ICMCGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM ICMCGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM ICMCGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM ICMCGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM ICMCGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(ICMCCommonTableGen)


add_llvm_target(ICMCCodeGen
  ICMCAsmPrinter.cpp
  ICMCInstrInfo.cpp
  ICMCISelLowering.cpp
  ICMCRegisterInfo.cpp
  ICMCISelDAGToDAG.cpp
  ICMCTargetMachine.cpp

  LINK_COMPONENTS
  AsmPrinter
  CodeGen
  Core
  MC
  SelectionDAG
  Support
  Target

  ADD_TO_COMPONENT
  ICMC
)

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
