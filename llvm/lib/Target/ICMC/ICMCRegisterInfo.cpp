#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/IR/Function.h"

#include "ICMCFrameLowering.h"

#include "ICMCRegisterInfo.h"

#define GET_REGINFO_TARGET_DESC
#include "ICMCGenRegisterInfo.inc"

namespace llvm {

const uint16_t *ICMCRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
    return ICMC_CSR_SaveList;
};

const uint32_t *ICMCRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                   CallingConv::ID) const {
    return ICMC_CSR_RegMask;
};

BitVector ICMCRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
    BitVector Reserved(getNumRegs());

    markSuperRegs(Reserved, ICMC::SP);
    markSuperRegs(Reserved, ICMC::PC);

    return Reserved;
};

void ICMCRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
    unsigned FIOperandNum, RegScavenger *RS) const {

};

Register ICMCRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
    return ICMC::SP;
};

}// end namespace llvm

