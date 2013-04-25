#include "MultipleCandidateAnalyseReducer.h"

// from STL
#include <map>

// from ROOT
#include "TLeaf.h"

// from RooFit

// from DooCore
#include "doocore/io/MsgStream.h"

// from DooFit

// from project

namespace dooselection {
namespace reducer {

using namespace doocore::io;

MultipleCandidateAnalyseReducer::MultipleCandidateAnalyseReducer()
//  : 
{}

void MultipleCandidateAnalyseReducer::AddEventIdentifier(const std::string& name_leaf) {
  TLeaf* leaf = input_tree_->GetLeaf(name_leaf.c_str());
  
  if (leaf == NULL) {
    serr << "MultipleCandidateAnalyseReducer::AddEventIdentifier(...): Cannot find indentifier leaf " << name_leaf << " in input tree. Ignoring it." << endmsg;
  } else {
    event_identifiers_.push_back(ReducerLeaf<Long64_t>(leaf));
  }
}
  
void MultipleCandidateAnalyseReducer::ProcessInputTree() {
  
}
} // namespace reducer
} // namespace dooselection