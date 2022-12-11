add_llvm_component_group(ICMC)

set(LLVM_TARGET_DEFINITIONS ICMC.td)

tablegen(LLVM ICMCGenRegisterInfo.inc -gen-register-info)

add_public_tablegen_target(ICMCCommonTableGen)


add_llvm_target(ICMCCodeGen
  ICMCAsmPrinter.cpp
  ICMCTargetMachine.cpp
  ICMCTargetInfo.cpp
  ICMCRegisterInfo.cpp

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

