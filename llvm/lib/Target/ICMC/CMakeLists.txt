add_llvm_component_group(ICMC)

set(LLVM_TARGET_DEFINITIONS ICMC.td)

tablegen(LLVM ICMCGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM ICMCGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM ICMCGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM ICMCGenCallingConv.inc -gen-callingconv)
tablegen(LLVM ICMCGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM ICMCGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM ICMCGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM ICMCGenAsmWriter.inc -gen-asm-writer)

add_public_tablegen_target(ICMCCommonTableGen)


add_llvm_target(ICMCCodeGen
  ICMCTargetObjectFile.cpp
  ICMCExpandPseudoInsts.cpp
  ICMCInstrInfo.cpp
  ICMCRegisterInfo.cpp
  ICMCAsmPrinter.cpp
  ICMCMCInstLower.cpp
  ICMCFrameLowering.cpp
  ICMCISelLowering.cpp
  ICMCSubtarget.cpp
  ICMCTargetMachine.cpp
  ICMCISelDAGToDAG.cpp


  LINK_COMPONENTS
  Analysis
  AsmPrinter
  CodeGen
  Core
  MC
  SelectionDAG
  Support
  Target
  TransformUtils

  ADD_TO_COMPONENT
  ICMC
)

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
add_subdirectory(AsmParser)

