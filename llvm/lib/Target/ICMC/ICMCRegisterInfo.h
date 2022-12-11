#define GET_REGINFO_ENUM
#define GET_REGINFO_HEADER
#include "ICMCGenRegisterInfo.inc"

namespace llvm {

class ICMCRegisterInfo : public ICMCGenRegisterInfo {
public:
    ICMCRegisterInfo() : ICMCGenRegisterInfo(0, ICMC::SP, 0, 0, ICMC::PC) {}

    const uint16_t *getCalleeSavedRegs(const MachineFunction *MF) const override;

    const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const override;

    BitVector getReservedRegs(const MachineFunction &MF) const override;

    void eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
        unsigned FIOperandNum, RegScavenger *RS = nullptr) const override;

    Register getFrameRegister(const MachineFunction &MF) const override;
};

}; // namespace llvm
