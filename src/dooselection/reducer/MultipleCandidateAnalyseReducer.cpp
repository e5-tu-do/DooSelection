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

MultipleCandidateAnalyseReducer::MultipleCandidateAnalyseReducer():
do_multi_cand_analysis_(true),
check_sequential_identifiers_(true)
{}
  
MultipleCandidateAnalyseReducer::~MultipleCandidateAnalyseReducer() {
  sdebug << "MultipleCandidateAnalyseReducer::~MultipleCandidateAnalyseReducer()" << endmsg;
}

void MultipleCandidateAnalyseReducer::AddEventIdentifier(const std::string& name_leaf) {
  event_identifier_names_ += name_leaf;
}
  
void MultipleCandidateAnalyseReducer::ProcessInputTree() {
  
  if (do_multi_cand_analysis_){
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
    
    // precaching everything that should not be evaluated in the event loop
    ULong64_t num_entries = input_tree_->GetEntries();
    std::vector<ReducerLeaf<ULong64_t> >::const_iterator identfiers_begin = event_identifiers.begin();
    std::vector<ReducerLeaf<ULong64_t> >::const_iterator identfiers_end   = event_identifiers.end();
    std::vector<ReducerLeaf<ULong64_t> >::const_iterator it;
    double frac = 0.0;
    int n_print_stepping = 500;
    double frac_increment = static_cast<double>(n_print_stepping)/num_entries*100.0;
    bool tty = isatty(fileno(stdout));
    
    sinfo << "MultipleCandidateAnalyseReducer::ProcessInputTree(): Analysing events according to event identifiers." << endmsg;
    std::vector<ULong64_t> last_identifier;
    for (ULong64_t i=0; i<num_entries; ++i) {
      input_tree_->GetEntry(i);
      
      std::vector<ULong64_t> identifier;
      
      for (it = identfiers_begin; it != identfiers_end; ++it) {
        identifier.push_back(it->GetValue());
      }
      
      if (check_sequential_identifiers_ && mapping_id_tree_.count(identifier) > 0) {
        if (identifier != last_identifier) {
          swarn << "Event #" << i << " is a non-sequential multiple candidate." << endmsg;
          swarn << "  Identifier: " << identifier << endmsg;
          
          for (std::multimap<std::vector<ULong64_t>, ULong64_t>::iterator it = mapping_id_tree_.find(identifier); it != mapping_id_tree_.upper_bound(identifier); ++it) {
            swarn << "  Found this before in event #" << it->second << endmsg;
          }
        }
      }
      
      insert(mapping_id_tree_)(identifier, i);
      
      if (tty && (i%n_print_stepping) == 0) {
        // double frac = static_cast<double> (i)/num_entries*100.0;
        frac += frac_increment;
        printf("Progress %.2f %         \xd", frac);
        fflush(stdout);
      }
      
      last_identifier = identifier;
    }
    
    typedef std::multimap<std::vector<ULong64_t>, ULong64_t> MapType;
    std::map<int,int> multicand_histogram;
    
    sinfo << "MultipleCandidateAnalyseReducer::ProcessInputTree(): Analysing stored events for multiplicities." << endmsg;
    // iterating over unique keys with this trick:
    for(MapType::const_iterator it = mapping_id_tree_.begin(), end = mapping_id_tree_.end();
        it != end; it = mapping_id_tree_.upper_bound(it->first)) {
      
      // iterators inside a unique key
      MapType::const_iterator it_start = it;
      MapType::const_iterator it_end   = mapping_id_tree_.upper_bound(it->first);
      
      // count possible multiple occurrences of one unique key
      int i = 0;
      for (MapType::const_iterator it_same = it_start; it_same != it_end; ++it_same) {
        i++;
      }
      if (multicand_histogram.count(i) == 0) {
        multicand_histogram[i] = 1;
      } else {
        multicand_histogram[i]++;
      }
    }
    
    sinfo << "MultipleCandidateAnalyseReducer::ProcessInputTree(): Analysis finished. Printing number of multiple candidates (# mc) vs. number of occurrences (# evts)" << endmsg;
    sinfo << "# mc \t # evts" << endmsg;
    int num_multicands_total = 0;
    for (std::map<int,int>::const_iterator it = multicand_histogram.begin();
         it != multicand_histogram.end(); ++it) {
      sinfo << it->first << "\t " << it->second << endmsg;
      if (it->first > 1) {
        num_multicands_total += it->first * it->second;
      }
    }
    sinfo << "Total number of multiple candidates: " << num_multicands_total << " (" << static_cast<double>(num_multicands_total)/num_entries*100 << "%)" << endmsg;
  }
  
  input_tree_->SetBranchStatus("*", true);
}
} // namespace reducer
} // namespace dooselection