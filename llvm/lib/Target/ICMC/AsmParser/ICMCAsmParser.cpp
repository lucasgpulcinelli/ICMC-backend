#include "ICMCInstrInfo.h"
#include "ICMCRegisterInfo.h"
#include "TargetInfo/ICMCTargetInfo.h"

#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/TargetRegistry.h"


using namespace llvm;

namespace {
class ICMCAsmParser : public MCTargetAsmParser {
  const MCSubtargetInfo &STI;
  MCAsmParser &Parser;
  const MCRegisterInfo *MRI;

  #define GET_ASSEMBLER_HEADER
  #include "ICMCGenAsmMatcher.inc"

public:
  ICMCAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
               const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, STI, MII), STI(STI), Parser(Parser) {
    MCAsmParserExtension::Initialize(Parser);
    MRI = getContext().getRegisterInfo();

    setAvailableFeatures(ComputeAvailableFeatures(STI.getFeatureBits()));
  }

  bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc,
                             SMLoc &EndLoc) override;

  OperandMatchResultTy
  tryParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override;

  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;

  bool ParseDirective(AsmToken DirectiveID) override;

  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

  MCAsmParser &getParser() const { return Parser; }
  MCAsmLexer &getLexer() const { return Parser.getLexer(); }
};
} // end anonymous namespace.

void ICMCAsmParser::convertToMapAndConstraints(unsigned Kind,
      const OperandVector &Operands) {
  llvm_unreachable("convertToMapAndConstraints not implemented");
}

FeatureBitset ICMCAsmParser::ComputeAvailableFeatures(
      const FeatureBitset &FB) const {
  llvm_unreachable("ComputeAvailableFeatures not implemented");
}

bool ICMCAsmParser::ParseRegister(unsigned &RegNo, SMLoc &StartLoc,
                                  SMLoc &EndLoc) {
  llvm_unreachable("ParseRegister not implemented");
}

OperandMatchResultTy
ICMCAsmParser::tryParseRegister(unsigned &RegNo, SMLoc &StartLoc,
                                SMLoc &EndLoc) {
  llvm_unreachable("tryParseRegister not implemented");
}

bool ICMCAsmParser::ParseInstruction(
      ParseInstructionInfo &Info, StringRef Name, SMLoc NameLoc,
      OperandVector &Operands) {
  llvm_unreachable("ParseInstruction not implemented");
}

bool ICMCAsmParser::ParseDirective(AsmToken DirectiveID) {
  llvm_unreachable("ParseDirective not implemented");
}

bool ICMCAsmParser::MatchAndEmitInstruction(
      SMLoc IDLoc, unsigned &Opcode, OperandVector &Operands, MCStreamer &Out,
      uint64_t &ErrorInfo, bool MatchingInlineAsm) {
  llvm_unreachable("MatchAndEmitInstruction not implemented");
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeICMCAsmParser() {
  RegisterMCAsmParser<ICMCAsmParser> X(getTheICMCTarget());
}

