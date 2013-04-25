#include "MultipleCandidateAnalyseReducer.h"

// from STL
#include <map>

// from Boost
#include <boost/assign/std/vector.hpp> // for 'operator+=()'
#include <boost/assign/list_inserter.hpp> // for 'insert()'
using namespace boost::assign; // bring 'operator+=()' into scope

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
    event_identifiers_.push_back(ReducerLeaf<ULong64_t>(leaf));
  }
}
  
void MultipleCandidateAnalyseReducer::ProcessInputTree() {
  input_tree_->SetBranchStatus("*", false);
  
  for (std::vector<ReducerLeaf<ULong64_t> >::const_iterator it = event_identifiers_.begin();
       it != event_identifiers_.end(); ++it) {
    input_tree_->SetBranchStatus(it->name(), true);
  }
  
  ULong64_t num_entries = input_tree_->GetEntries();
  for (ULong64_t i=0; i<num_entries; ++i) {
    input_tree_->GetEntry(i);
    
    std::vector<ULong64_t> identifier;
    
    for (std::vector<ReducerLeaf<ULong64_t> >::const_iterator it = event_identifiers_.begin();
         it != event_identifiers_.end(); ++it) {
      sdebug << "i = " << i << ", " << it->name() << " = " << it->GetValue() << endmsg;
      
      identifier += it->GetValue();
    }
    
    insert(mapping_id_tree_)(identifier, i);
  }
  
  for(std::multimap<std::vector<ULong64_t>, ULong64_t>::const_iterator it = mapping_id_tree_.begin(), end = mapping_id_tree_.end();
      it != end; it = mapping_id_tree_.upper_bound(it->first)) {
    sdebug << it->first << ' -> ' << it->second << endmsg;
//    for (std::vector<ULong64_t>::const_iterator it_id = it->first.begin();
//         it_id != it->first.end(); ++it) {
//      sdebug << *it_id << " " << endmsg;
//    }
//    sdebug << "-> " << it->second << endmsg;
  }
  
  input_tree_->SetBranchStatus("*", true);
}
} // namespace reducer
} // namespace dooselection