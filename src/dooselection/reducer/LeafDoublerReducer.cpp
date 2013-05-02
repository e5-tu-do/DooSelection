#include "LeafDoublerReducer.h"

// from STL
#include <vector>

// from ROOT
#include "TString.h"

// from RooFit

// from DooCore
#include <doocore/io/MsgStream.h>

// from DooFit

using namespace doocore::io;

namespace dooselection {
namespace reducer {
 
void LeafDoublerReducer::CreateSpecialBranches() {
  for (std::vector<ReducerLeaf<Float_t>* >::const_iterator it = GetInterimLeavesBegin();
       it != GetInterimLeavesEnd(); ++it) {
    if ((*it)->type() != "Double_t" && (*it)->type() != "Float_t" && (*it)->type() != "Int_t" && (*it)->type() != "UInt_t") {
      if (LeafExists(std::string((*it)->name())+"_D")) {
        sinfo << "Leaf " << (*it)->name() << " (not RooFit-compatible), but seems to be already copied to double version " << (*it)->name()+"_D" << "." << endmsg;
      } else {
        CreateDoubleCopyLeaf((*it)->name()+"_D", **it);
      }
    }
  }
}
} // namespace reducer
} // namespace dooselection
