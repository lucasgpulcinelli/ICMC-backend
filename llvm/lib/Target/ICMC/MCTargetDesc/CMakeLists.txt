add_llvm_component_library(LLVMICMCMCTargetDesc
  ICMCMCCodeEmitter.cpp
  ICMCELFObjectWriter.cpp
  ICMCAsmBackend.cpp
  ICMCMCAsmInfo.cpp
  ICMCInstrPrinter.cpp
  ICMCMCTargetDesc.cpp


  ADD_TO_COMPONENT
  ICMC
)

