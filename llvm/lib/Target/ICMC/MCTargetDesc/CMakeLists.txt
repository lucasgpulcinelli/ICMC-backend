add_llvm_component_library(LLVMICMCMCTargetDesc
  ICMCELFObjectWriter.cpp
  ICMCAsmBackend.cpp
  ICMCMCAsmInfo.cpp
  ICMCTargetStreamer.cpp
  ICMCInstrPrinter.cpp
  ICMCMCTargetDesc.cpp


  ADD_TO_COMPONENT
  ICMC
)

