#include "ICMCInstrInfo.h"
#include "ICMCRegisterInfo.h"
#include "MCTargetDesc/ICMCMCTargetDesc.h"
#include "TargetInfo/ICMCTargetInfo.h"

#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/TargetRegistry.h"


using namespace llvm;

namespace {
class ICMCAsmParser : public MCTargetAsmParser {
  const MCSubtargetInfo &STI;
  MCAsmParser &Parser;
  const MCRegisterInfo *MRI;

  #define GET_ASSEMBLER_HEADER
  #include "ICMCGenAsmMatcher.inc"

  bool tryParseRegisterOperand(OperandVector &Operands);

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

  bool parseOperand(OperandVector &Operands);

  bool ParseDirective(AsmToken DirectiveID) override;

  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

  MCAsmParser &getParser() const { return Parser; }
  MCAsmLexer &getLexer() const { return Parser.getLexer(); }
};

class ICMCOperand : public MCParsedAsmOperand {
private:
  enum KindTy { k_Immediate, k_Register, k_Token, k_Memri } Kind;

  union {
    StringRef Token;
    unsigned Register;
  };

  SMLoc Start, End;

public:
  ICMCOperand(StringRef Token, SMLoc const &S)
      : Kind(k_Token), Token(Token), Start(S), End(S) {}
  ICMCOperand(unsigned Register, SMLoc const &S, SMLoc const &E)
      : Kind(k_Register), Register(Register), Start(S), End(E) {}

  static std::unique_ptr<ICMCOperand> CreateToken(StringRef Str, SMLoc S) {
    return std::make_unique<ICMCOperand>(Str, S);
  }

  static std::unique_ptr<ICMCOperand> CreateReg(unsigned RegNum, SMLoc S,
                                               SMLoc E) {
    return std::make_unique<ICMCOperand>(RegNum, S, E);
  }

  bool isToken() const override {
    return Kind == k_Token;
  }

  bool isImm() const override {
    return Kind == k_Immediate;
  }

  bool isReg() const override {
    return Kind == k_Register;
  }

  unsigned getReg() const override {
    if (Kind != k_Register) {
      return ICMC::NoRegister;
    }

    return Register;
  }

  bool isMem() const override {
    return Kind == k_Memri;
  }

  SMLoc getStartLoc() const override {
    return Start;
  }

  SMLoc getEndLoc() const override {
    return End;
  }

  void print(raw_ostream &OS) const override {
    switch (Kind) {
    case k_Token:
      OS << "Token: \"" << getToken() << "\"";
      break;
    case k_Register:
      OS << "Register: " << getReg();
      break;
    case k_Immediate:
      OS << "Immediate: \"" << "" << "\"";
      break;
    case k_Memri: {
      // only manually print the size for non-negative values,
      // as the sign is inserted automatically.
      OS << "Memri: \"" << getReg() << '+' << "" << "\"";
      break;
    }
    }
    OS << "\n";
  }

  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(Kind == k_Register && "Unexpected operand kind");
    assert(N == 1 && "Invalid number of operands!");

    Inst.addOperand(MCOperand::createReg(getReg()));
  }

  StringRef getToken() const {
    assert(Kind == k_Token && "kind of operand is not a token");

    return Token;
  }
};
} // end anonymous namespace.


#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "ICMCGenAsmMatcher.inc"


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

  Operands.push_back(ICMCOperand::CreateToken(Name, NameLoc));

  if(getLexer().is(AsmToken::EndOfStatement)){
    Parser.Lex();
    return false;
  }

  while (true) {
    // parse the operand following the current token
    if (parseOperand(Operands)) {
      SMLoc Loc = getLexer().getLoc();
      Parser.eatToEndOfStatement();
      return Error(Loc, "unexpected token in argument list");
    }

    // if we reach the end of statement, no comma is permitted
    if (getLexer().is(AsmToken::EndOfStatement)) {
        break;
    }

    // but if there are more tokens, make sure there is a comma separating them
    if (getLexer().is(AsmToken::Comma)) {
      Parser.Lex();
    } else {
      SMLoc Loc = getLexer().getLoc();
      Parser.eatToEndOfStatement();
      return Error(Loc, "expected comma between instruction operands");
    }
  }

  // parse the final end of statement token
  Parser.Lex();
  return false;
}

bool ICMCAsmParser::parseOperand(OperandVector &Operands) {
  switch (getLexer().getKind()) {
  case AsmToken::Identifier:
    //register name or label
    if (!tryParseRegisterOperand(Operands)) {
      return false;
    }
    llvm_unreachable("label identification not implemented");

  case AsmToken::Hash:
    // memory address or imm
    llvm_unreachable("immediate and memory address parsing not implemented");
  default:
    return Error(Parser.getTok().getLoc(), "unexpected operand token");
  }

  return true;
}

bool ICMCAsmParser::tryParseRegisterOperand(OperandVector &Operands) {
  if(Parser.getTok().isNot(AsmToken::Identifier)){
      return true;
  }

  // try to match with a register name as defined in tablegen
  StringRef Name = Parser.getTok().getString();
  int RegNo = MatchRegisterName(Name);

  if (RegNo == ICMC::NoRegister) {
    // not a register token
    return true;
  }

  AsmToken const &T = Parser.getTok();
  Operands.push_back(ICMCOperand::CreateReg(RegNo, T.getLoc(), T.getEndLoc()));
  Parser.Lex(); // consume the token for the register, we just used it.

  return false;
}

bool ICMCAsmParser::ParseDirective(AsmToken DirectiveID) {
  llvm_unreachable("ParseDirective not implemented");
}

bool ICMCAsmParser::MatchAndEmitInstruction(
      SMLoc Loc, unsigned &Opcode, OperandVector &Operands, MCStreamer &Out,
      uint64_t &ErrorInfo, bool MatchingInlineAsm) {

  MCInst Inst;
  unsigned MatchResult =
      MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm);

  switch (MatchResult) {
  case Match_Success:
    Inst.setLoc(Loc);
    Out.emitInstruction(Inst, STI);
    return false;

  case Match_MissingFeature:
    return Error(Loc, "missing feature in selected CPU");
  case Match_InvalidOperand:
    return Error(Loc, "invalid operand information");
  case Match_MnemonicFail:
    return Error(Loc, "invalid instruction");
  default:
    return true;
  }
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeICMCAsmParser() {
  RegisterMCAsmParser<ICMCAsmParser> X(getTheICMCTarget());
}

