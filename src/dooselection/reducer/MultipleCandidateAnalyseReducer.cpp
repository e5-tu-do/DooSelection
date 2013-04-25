#include "MultipleCandidateAnalyseReducer.h"

// from STL
#include <map>

// from Boost
#include <boost/assign/std/vector.hpp> // for 'operator+=()'
#include <boost/assign/list_inserter.hpp> // for 'insert()'
using namespace boost::assign; // bring 'operator+=()' into scope

// from ROOT
#include "TLeaf.h"
#include "TStopwatch.h"

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
  event_identifier_names_ += name_leaf;
}
  
void MultipleCandidateAnalyseReducer::ProcessInputTree() {
  std::vector<ReducerLeaf<ULong64_t> > event_identifiers;

  for (std::vector<std::string>::const_iterator it = event_identifier_names_.begin(), end=event_identifier_names_.end();
       it != end; ++it) {
    TLeaf* leaf = input_tree_->GetLeaf(it->c_str());
    
    if (leaf == NULL) {
      serr << "MultipleCandidateAnalyseReducer::AddEventIdentifier(...): Cannot find indentifier leaf " << *it << " in input tree. Ignoring it." << endmsg;
    } else {
      sinfo << "MultipleCandidateAnalyseReducer::AddEventIdentifier(...): Adding " << *it << " as event identifier." << endmsg;
      event_identifiers.push_back(ReducerLeaf<ULong64_t>(leaf));
    }
  }

  
  input_tree_->SetBranchStatus("*", false);
  
  for (std::vector<ReducerLeaf<ULong64_t> >::const_iterator it = event_identifiers.begin();
       it != event_identifiers.end(); ++it) {
    input_tree_->SetBranchStatus(it->name(), true);
  }
  
  ULong64_t num_entries = input_tree_->GetEntries();
  std::vector<ReducerLeaf<ULong64_t> >::const_iterator identfiers_begin = event_identifiers.begin();
  std::vector<ReducerLeaf<ULong64_t> >::const_iterator identfiers_end   = event_identifiers.end();

  sinfo << "MultipleCandidateAnalyseReducer::ProcessInputTree(): Analysing events according to event identifiers." << endmsg;
  TStopwatch sw;
  sw.Start();
  for (ULong64_t i=0; i<num_entries; ++i) {
    input_tree_->GetEntry(i);
    
    std::vector<ULong64_t> identifier;
    
    for (std::vector<ReducerLeaf<ULong64_t> >::const_iterator it = identfiers_begin;
         it != identfiers_end; ++it) {
//      sdebug << "i = " << i << ", " << it->name() << " = " << it->GetValue() << endmsg;
      identifier.push_back(it->GetValue());
    }
    
//    insert(mapping_id_tree_)(identifier, i);
    mapping_id_tree_.insert(std::pair<std::vector<ULong64_t>,ULong64_t>(identifier, i));
    
    if (isatty(fileno(stdout))) {
      if ((i%200) == 0) {
        double frac = static_cast<double> (i)/num_entries*100.0;
        printf("Progress %.2f %         \xd", frac);
        fflush(stdout);
      }
    }
  }
  sdebug << sw << endmsg;
  
  for(std::multimap<std::vector<ULong64_t>, ULong64_t>::const_iterator it = mapping_id_tree_.begin(), end = mapping_id_tree_.end();
      it != end; it = mapping_id_tree_.upper_bound(it->first)) {
    sdebug << it->first << " -> " << it->second << endmsg;
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