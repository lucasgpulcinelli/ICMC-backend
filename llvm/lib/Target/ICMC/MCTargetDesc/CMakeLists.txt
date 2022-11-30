add_llvm_component_library(LLVMICMCDesc
  ICMCMCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  ICMC
)

