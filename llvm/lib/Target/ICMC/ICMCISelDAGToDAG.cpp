#include "ICMCISelDAGToDAG.h"
#include "ICMCTargetMachine.h"

#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGISel.h"

using namespace llvm;

namespace {

class ICMCDAGToDAGISel : public SelectionDAGISel {
public:
  ICMCDAGToDAGISel(ICMCTargetMachine &TM, CodeGenOpt::Level OptLevel)
      : SelectionDAGISel(TM, OptLevel) {}

  void Select(SDNode *N) override;

  StringRef getPassName() const override {
    return "ICMC DAG->DAG Pattern Instruction Selection";
  }

  bool SelectSPmem(SDValue Addr, SDValue &Offset);

  #include "ICMCGenDAGISel.inc"
};

} // end anonymous namespace

void ICMCDAGToDAGISel::Select(SDNode *N) {
  SelectCode(N);
}

bool ICMCDAGToDAGISel::SelectSPmem(SDValue Addr, SDValue &Offset){
  FrameIndexSDNode *FIN = nullptr;
  if ((FIN = dyn_cast<FrameIndexSDNode>(Addr))) {
    Offset = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i16);
    return true;
  }
  return false;
}

FunctionPass *llvm::createICMCISelDag(ICMCTargetMachine &TM,
                                     CodeGenOpt::Level OptLevel) {
  return new ICMCDAGToDAGISel(TM, OptLevel);
}

