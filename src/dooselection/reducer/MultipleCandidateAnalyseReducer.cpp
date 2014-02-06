#include "MultipleCandidateAnalyseReducer.h"

// from STL
#include <map>
#include <iomanip>

// from Boost
#include <boost/assign/std/vector.hpp> // for 'operator+=()'
#include <boost/assign/list_inserter.hpp> // for 'insert()'
using namespace boost::assign; // bring 'operator+=()' into scope

// from ROOT
#include "TLeaf.h"
#include "TStopwatch.h"

// from RooFit

// from DooCore
#include <doocore/io/MsgStream.h>
#include <doocore/io/Progress.h>

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
}

void MultipleCandidateAnalyseReducer::AddEventIdentifier(const std::string& name_leaf) {
  event_identifier_names_ += name_leaf;
}

void MultipleCandidateAnalyseReducer::AddEventCharacteristic(const std::string& name_leaf) {
  additional_event_characteristics_ += name_leaf;
}
  
void MultipleCandidateAnalyseReducer::ProcessInputTree() {
  using namespace doocore::io;
  if (do_multi_cand_analysis_){
    std::vector<ReducerLeaf<ULong64_t> > event_identifiers;
    std::vector<ReducerLeaf<ULong64_t> > event_characteristics;
    
    for (std::vector<std::string>::const_iterator it = event_identifier_names_.begin(), end=event_identifier_names_.end(); it != end; ++it) {
      TLeaf* leaf = input_tree_->GetLeaf(it->c_str());
      
      if (leaf == NULL) {
        serr << "MultipleCandidateAnalyseReducer::AddEventIdentifier(...): Cannot find indentifier leaf " << *it << " in input tree. Ignoring it." << endmsg;
      } else {
        sinfo << "MultipleCandidateAnalyseReducer::AddEventIdentifier(...): Adding " << *it << " as event identifier." << endmsg;
        event_identifiers.push_back(ReducerLeaf<ULong64_t>(leaf));
      }
    }
    for (auto name_leaf : additional_event_characteristics_) {
      TLeaf* leaf = input_tree_->GetLeaf(name_leaf.c_str());
      
      if (leaf == NULL) {
        serr << "MultipleCandidateAnalyseReducer::AddEventCharacteristics(...): Cannot find characteristics leaf " << name_leaf << " in input tree. Ignoring it." << endmsg;
      } else {
        sinfo << "MultipleCandidateAnalyseReducer::AddEventCharacteristics(...): Adding " << name_leaf << " as event characteristics." << endmsg;
        event_characteristics.push_back(ReducerLeaf<ULong64_t>(leaf));
      }
    }

    input_tree_->SetBranchStatus("*", false);
    
    
    for (std::vector<ReducerLeaf<ULong64_t> >::const_iterator it = event_identifiers.begin();
         it != event_identifiers.end(); ++it) {
      input_tree_->SetBranchStatus(it->name(), true);
    }
    for (auto name_leaf : additional_event_characteristics_) {
      input_tree_->SetBranchStatus(name_leaf.c_str(), true);
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
    Progress p("Analysing multiple candidates", num_entries);
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
          
          for (EventMap::iterator it = mapping_id_tree_.find(identifier); it != mapping_id_tree_.upper_bound(identifier); ++it) {
            swarn << "  Found this before in event #" << it->second << endmsg;
          }
        }
      }
      
      TreeIndexEventBucket b;
      b.first = i;
      
      for (auto leaf : event_characteristics) {
        b.second.push_back(leaf.GetValue());
      }
      
      insert(mapping_id_tree_)(identifier, b);
      
      ++p;
      
      last_identifier = identifier;
    }

    p.Finish();
    
    std::map<std::pair<int, std::vector<int>>,int> multicand_histogram;
    
    sinfo << "MultipleCandidateAnalyseReducer::ProcessInputTree(): Analysing stored events for multiplicities." << endmsg;
    // iterating over unique keys with this trick:
    for(EventMap::const_iterator it = mapping_id_tree_.begin(), end = mapping_id_tree_.end();
        it != end; it = mapping_id_tree_.upper_bound(it->first)) {
      
      // secondary characteristics
      std::map<UniqueEventIdentifier, int> mapping_second_characteristics;
      
      // iterators inside a unique key
      EventMap::const_iterator it_start = it;
      EventMap::const_iterator it_end   = mapping_id_tree_.upper_bound(it->first);
      
      // count possible multiple occurrences of one unique key
      int i = 0;
      std::pair<int, std::vector<int>> eb;
      for (EventMap::const_iterator it_same = it_start; it_same != it_end; ++it_same) {
        i++;
        
        if (mapping_second_characteristics.count((it_same->second).second) == 0) {
          mapping_second_characteristics[(it_same->second).second] = 1;
        } else {
          mapping_second_characteristics[(it_same->second).second]++;
        }
      }
      eb.first = i;
      for (auto secondary_characteristic : mapping_second_characteristics) {
        eb.second.push_back(secondary_characteristic.second);
      }
      
      if (multicand_histogram.count(eb) == 0) {
        multicand_histogram[eb] = 1;
      } else {
        multicand_histogram[eb]++;
      }
    }
    
    sinfo << "MultipleCandidateAnalyseReducer::ProcessInputTree(): Analysis finished. Printing number of multiple candidates (# mc) vs. number of occurrences (# evts)" << endmsg;
    sinfo << std::setw(10) << std::setfill(' ') << "# mc";
    sinfo << std::setw(10) << std::setfill(' ') << "# events";
    if (event_characteristics.size() > 0) {
      sinfo << " | # occurences per (unique characteristics: " <<  additional_event_characteristics_ << ")" << endmsg;
    } else {
      sinfo << endmsg;
    }
    int num_multicands_total = 0;
    int num_singlecands = 0;
    for (std::map<std::pair<int, std::vector<int>>,int>::const_iterator it = multicand_histogram.begin();
         it != multicand_histogram.end(); ++it) {
      sinfo << std::setw(10) << std::setfill(' ') << (it->first).first;
      sinfo << std::setw(10) << std::setfill(' ') << it->second;
      if ((it->first).second.size() > 0) {
        sinfo << " | " << (it->first).second << endmsg;
      } else {
        sinfo << endmsg;
      }
      if ((it->first).first > 1) {
        num_multicands_total += (it->first).first * it->second;
      }
      num_singlecands += it->second;
    }
    sinfo << "Total number of multiple candidates: " << num_multicands_total << " (" << static_cast<double>(num_multicands_total)/num_entries*100 << "%)" << endmsg;
    sinfo << "Total number of candidates after single candidate selection: " << num_singlecands << endmsg;
    sinfo << "Total number of entries in tree: " << input_tree_->GetEntries() << endmsg;
  }
  
  input_tree_->SetBranchStatus("*", true);
}
} // na mespace reducer
} // namespace dooselection