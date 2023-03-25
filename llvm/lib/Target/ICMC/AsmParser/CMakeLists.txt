add_llvm_component_library(LLVMICMCAsmParser
  ICMCAsmParser.cpp

  LINK_COMPONENTS
  MC
  MCParser
  Support

  ADD_TO_COMPONENT
  ICMC
)

