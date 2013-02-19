#include "LeafDoublerReducer.h"

// from STL
#include <vector>

// from ROOT
#include "TString.h"

// from RooFit

// from DooCore

// from DooFit

namespace dooselection {
namespace reducer {
 
void LeafDoublerReducer::CreateSpecialBranches() {
  for (std::vector<ReducerLeaf<Float_t>* >::const_iterator it = GetInterimLeavesBegin();
       it != GetInterimLeavesEnd(); ++it) {
    if ((*it)->type() != "Double_t" && (*it)->type() != "Float_t" && (*it)->type() != "Int_t" && (*it)->type() != "UInt_t") {
      CreateDoubleCopyLeaf((*it)->name()+"_D", **it);
    }
  }
}
} // namespace reducer
} // namespace dooselection
