#ifndef LLVM_ICMC_ELF_OBJECT_WRITER_H
#define LLVM_ICMC_ELF_OBJECT_WRITER_H

#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {

class MCObjectTargetWriter;

std::unique_ptr<MCObjectTargetWriter> createICMCELFObjectWriter(uint8_t OSABI);

} // end namespace llvm

#endif
