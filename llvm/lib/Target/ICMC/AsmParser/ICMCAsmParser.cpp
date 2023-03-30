#include "ICMCInstrInfo.h"
#include "ICMCRegisterInfo.h"
#include "MCTargetDesc/ICMCMCTargetDesc.h"
#include "TargetInfo/ICMCTargetInfo.h"
#include "ICMCMCExpr.h"

#include "llvm/ADT/ArrayRef.h"
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
  bool tryParseExpression(OperandVector &Operands);
  bool tryParseSymbolref(OperandVector &Operands);

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

  bool parseStaticOperands(OperandVector &Operands);

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
    const MCExpr *Imm;
  };

  SMLoc Start, End;

public:
  ICMCOperand(StringRef Token, SMLoc const &S)
      : Kind(k_Token), Token(Token), Start(S), End(S) {}
  ICMCOperand(unsigned Register, SMLoc const &S, SMLoc const &E)
      : Kind(k_Register), Register(Register), Start(S), End(E) {}
  ICMCOperand(const MCExpr* Imm, SMLoc const &S, SMLoc const &E)
      : Kind(k_Immediate), Imm(Imm), Start(S), End(E) {}


  static std::unique_ptr<ICMCOperand> CreateToken(StringRef Str, SMLoc S) {
    return std::make_unique<ICMCOperand>(Str, S);
  }

  static std::unique_ptr<ICMCOperand> CreateReg(unsigned RegNum, SMLoc S,
                                               SMLoc E) {
    return std::make_unique<ICMCOperand>(RegNum, S, E);
  }

  static std::unique_ptr<ICMCOperand> CreateImm(const MCExpr* Imm, SMLoc S,
                                               SMLoc E) {
    return std::make_unique<ICMCOperand>(Imm, S, E);
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
      OS << "Immediate: \"" << getImm() << "\"";
      break;
    case k_Memri: {
      // only manually print the size for non-negative values,
      // as the sign is inserted automatically.
      OS << "Memri: \"" << getReg() << '+' << getImm() << "\"";
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

  void addImmOperands(MCInst &Inst, unsigned N) const {
    assert(Kind == k_Immediate && "Unexpected operand kind");
    assert(N == 1 && "Invalid number of operands!");

    const MCExpr *Expr = getImm();
    if (!Expr)
      Inst.addOperand(MCOperand::createImm(0));
    else if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr))
      Inst.addOperand(MCOperand::createImm(CE->getValue()));
    else
      Inst.addOperand(MCOperand::createExpr(Expr));
  }

  StringRef getToken() const {
    assert(Kind == k_Token && "kind of operand is not a token");

    return Token;
  }

  const MCExpr *getImm() const {
    assert((Kind == k_Immediate || Kind == k_Memri) && "Invalid access!");
    return Imm;
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

  // the static instruction has a different syntax and uses ICMCMCExpr
  if(Name == "static"){
    if(parseStaticOperands(Operands)){
      return true;
    }
    Parser.Lex();
  }

  while (true) {
    // parse the operand following the current token
    if (parseOperand(Operands)) {
      Parser.eatToEndOfStatement();
      return true;
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

bool ICMCAsmParser::parseStaticOperands(OperandVector& Operands) {
  uint16_t Offset, SubstValue;

  StringRef Identifier;
  if (Parser.parseIdentifier(Identifier)) {
    return Error(Parser.getTok().getLoc(), "expected Memory label");
  }

  const MCSymbol *Sym = getContext().getOrCreateSymbol(Identifier);

  auto Vec = std::vector<AsmToken>();
  Vec.resize(5);
  auto Buf = MutableArrayRef<AsmToken>(Vec);

  getLexer().peekTokens(Buf);

  if (Buf[0].isNot(AsmToken::Hash) ||
      Buf[2].isNot(AsmToken::Comma)  ||
      Buf[3].isNot(AsmToken::Hash)) {
    return true;
  }

  Offset = Buf[1].getIntVal();
  SubstValue = Buf[4].getIntVal();

  SMLoc SMem = Parser.getTok().getLoc();
  SMLoc EMem = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer()-1);

  const ICMCMCExpr* Res = ICMCMCExpr::create(getContext(), Sym, Offset, SubstValue);

  Operands.push_back(ICMCOperand::CreateImm(Res, SMem, EMem));

  return false;
}

bool ICMCAsmParser::parseOperand(OperandVector &Operands) {
  switch (getLexer().getKind()) {
  case AsmToken::Identifier:
    //register name or label
    if (!tryParseRegisterOperand(Operands)) {
      return false;
    }
    return tryParseSymbolref(Operands);

  case AsmToken::Hash:
    // memory address or imm
    switch (getLexer().peekTok(false).getKind()) {
    case AsmToken::Identifier:
      Parser.Lex();
      return tryParseSymbolref(Operands);
    case AsmToken::Integer:
      Parser.Lex();
      return tryParseExpression(Operands);
    default:
      return Error(Parser.getTok().getLoc(),
                   "expected memory address or immediate");
    }
  default:
    return Error(Parser.getTok().getLoc(), "unexpected operand token");
  }

  return true;
}

bool ICMCAsmParser::tryParseExpression(OperandVector &Operands) {
  MCExpr const *Expression;
  if (getParser().parseExpression(Expression))
    return true;

  SMLoc S = Parser.getTok().getLoc();
  SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
  Operands.push_back(ICMCOperand::CreateImm(Expression, S, E));
  return false;
}

bool ICMCAsmParser::tryParseSymbolref(OperandVector &Operands) {
  StringRef Identifier;
  if (Parser.parseIdentifier(Identifier))
    return true;

  MCSymbol *Sym = getContext().getOrCreateSymbol(Identifier);

  const MCExpr *Res = MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None,
                                              getContext());

  SMLoc S = Parser.getTok().getLoc();
  SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
  Operands.push_back(ICMCOperand::CreateImm(Res, S, E));
  return false;
}

bool ICMCAsmParser::tryParseRegisterOperand(OperandVector &Operands) {
  if(Parser.getTok().isNot(AsmToken::Identifier)){
      return true;
  }

  // try to match with a register name as defined in tablegen
  StringRef Name = Parser.getTok().getString();
  int RegNo = MatchRegisterName(Name.lower());

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

