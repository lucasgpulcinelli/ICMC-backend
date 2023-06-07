#include "ICMCISelDAGToDAG.h"
#include "ICMCTargetMachine.h"

#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGISel.h"

using namespace llvm;

namespace {

/*
 * ICMCISelDAGToDAG: defines how the Instruction Selection DAG is initially
 * created. This, together with ICMCISelLowering, are the main classes for the
 * compilation process itself.
 */
class ICMCDAGToDAGISel : public SelectionDAGISel {
public:
  ICMCDAGToDAGISel(ICMCTargetMachine &TM, CodeGenOpt::Level OptLevel)
      : SelectionDAGISel(TM, OptLevel) {}

  // Select makes the instruction selection for a node N using the definitions
  // in ICMCInstrInfo.td, this method runs right after the ICMCISelLowering is
  // done.
  void Select(SDNode *N) override;

  StringRef getPassName() const override {
    return "ICMC DAG->DAG Pattern Instruction Selection";
  }

  // selectSPmem makes the instruction selection for a frame index, needed when
  // a pointer to a stack variable needs to be accessed directly, for instance
  // when a programmer does pointer arithmetic.
  bool selectSPmem(SDValue Addr, SDValue &Offset);

#include "ICMCGenDAGISel.inc"

private:
  bool selectFrameIndex(SDNode *N);
};

} // end anonymous namespace

void ICMCDAGToDAGISel::Select(SDNode *N) { SelectCode(N); }

bool ICMCDAGToDAGISel::selectSPmem(SDValue Addr, SDValue &Offset) {
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
