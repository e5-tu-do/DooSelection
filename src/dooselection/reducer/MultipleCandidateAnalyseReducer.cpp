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
    
    sinfo << "MultipleCandidateAnalyseReducer::ProcessInputTree(): Analysis finished." << endmsg; 
    sinfo.Ruler();
    sinfo << "Total number of multiple candidates (# mc) vs. number of occurrences (# evts)" << endmsg;
    sinfo << std::setw(10) << std::setfill(' ') << "# mc";
    sinfo << std::setw(10) << std::setfill(' ') << "# evts";
    if (event_characteristics.size() > 0) {
      sinfo << " | # occurences per unique characteristics: " <<  additional_event_characteristics_ << endmsg;
    } else {
      sinfo << endmsg;
    }

    std::map<int, int> map_multiple_b_candidates;
    std::map<int, int> map_events_with_multiple_b_candidates;
    std::map<int, int> map_multiple_pvs;
    std::map<int, int> map_events_with_multiple_pvs;

    map_multiple_b_candidates[1] = 0;
    map_events_with_multiple_b_candidates[1] = 0;
    map_multiple_pvs[1] = 0;
    map_events_with_multiple_pvs[1] = 0;

    int num_multiple_b_candidates = 0;
    int num_events_with_multiple_b_candidates = 0;
    int num_multiple_pvs = 0;
    int num_events_with_multiple_pvs = 0;
    int num_multicands_total = 0;
    int num_events_with_multicands_total = 0;
    int num_singlecands = 0;
    
    for (std::map<std::pair<int, std::vector<int>>,int>::const_iterator it = multicand_histogram.begin();
         it != multicand_histogram.end(); ++it) {
      sinfo << std::setw(10) << std::setfill(' ') << (it->first).first;         // number of multiple candidates in event
      sinfo << std::setw(10) << std::setfill(' ') << it->second;                // number of events of this type
      if ((it->first).second.size() > 0) {
        sinfo << " | " << (it->first).second << endmsg;                         // occurences per unique characteristics

        // now count the multiple B candidate occurences per event
        int max_entry_of_characteristics = 1;
        for (auto entry : (it->first).second){
          if (max_entry_of_characteristics < entry){
            max_entry_of_characteristics = entry;
          }
          // std::cout << entry << " " << max_entry_of_characteristics << std::endl;
        }
        
        if (max_entry_of_characteristics > 1){
          if (map_multiple_b_candidates.count(max_entry_of_characteristics) == 0) {
            map_multiple_b_candidates[max_entry_of_characteristics] =  (it->second) * max_entry_of_characteristics;
            map_events_with_multiple_b_candidates[max_entry_of_characteristics] =  (it->second);
          } else {
            map_multiple_b_candidates[max_entry_of_characteristics] += (it->second) * max_entry_of_characteristics;
            map_events_with_multiple_b_candidates[max_entry_of_characteristics] += (it->second);
          }
          num_multiple_b_candidates += (it->second) * max_entry_of_characteristics;
          num_events_with_multiple_b_candidates += (it->second);
        }
        else{
          map_multiple_b_candidates[1] +=  (it->second);
          map_events_with_multiple_b_candidates[1] += (it->second);
          num_multiple_b_candidates += (it->second);
        }

        // number of PVs
        if (map_multiple_pvs.count((it->first).second.size()) == 0) {
          map_multiple_pvs[(it->first).second.size()] =  (it->second) * (it->first).second.size();
          map_events_with_multiple_pvs[(it->first).second.size()] =  (it->second);
        } else {
          map_multiple_pvs[(it->first).second.size()] += (it->second) * (it->first).second.size();
          map_events_with_multiple_pvs[(it->first).second.size()] += (it->second);
        }
        if ((it->first).second.size() > 1){
          num_multiple_pvs += (it->second) * (it->first).second.size();
          num_events_with_multiple_pvs += (it->second);
        }

      } else {
        sinfo << endmsg;
      }
      if ((it->first).first > 1) {
        num_multicands_total += it->second * (it->first).first;
        num_events_with_multicands_total += it->second;
      }
      num_singlecands += it->second;
    }
    sinfo << "Overall number of multiple candidates: " << num_multicands_total << " (" << static_cast<double>(num_multicands_total)/num_entries*100 << "%)" << endmsg;
    sinfo << "Overall number of events with multiple candidates: " << num_events_with_multicands_total << " (" << static_cast<double>(num_events_with_multicands_total)/num_entries*100 << "%)" << endmsg;
    sinfo << "Overall number of multiple candidates to discard: " << num_multicands_total - num_events_with_multicands_total << " (" << static_cast<double>(num_multicands_total - num_events_with_multicands_total)/num_entries*100 << "%)" << endmsg;
    sinfo << endmsg;
    sinfo << "Number of multiple B candidates per event (# mBc) vs. number of B candidates matching this criteria (# Bc)" << endmsg;
    sinfo << std::setw(10) << std::setfill(' ') << "# mBc"; 
    sinfo << std::setw(10) << std::setfill(' ') << "# Bc" << endmsg;
    for (auto entry: map_multiple_b_candidates){
      sinfo << std::setw(10) << std::setfill(' ') << entry.first;
      sinfo << std::setw(10) << std::setfill(' ') << entry.second << endmsg;
    }
    sinfo << "Total number of B candidates: " << num_multiple_b_candidates << endmsg;
    sinfo << endmsg;
    sinfo << "Number of multiple B candidates per event (# mBc) vs. number of occurrences (# evts): " << num_events_with_multiple_b_candidates << " (" << static_cast<double>(num_events_with_multiple_b_candidates)/num_entries*100 << "%)" << endmsg;
    sinfo << std::setw(10) << std::setfill(' ') << "# mBc";
    sinfo << std::setw(10) << std::setfill(' ') << "# evts" << endmsg;
    for (auto entry: map_events_with_multiple_b_candidates){
      sinfo << std::setw(10) << std::setfill(' ') << entry.first;
      sinfo << std::setw(10) << std::setfill(' ') << entry.second << endmsg;
    }
    sinfo << endmsg;
    sinfo << "Number of multiple occurences of additional characteristics (# moac) vs. number of occurences of additional characteristics matching this criteria (# oac)" << endmsg;
    sinfo << std::setw(10) << std::setfill(' ') << "# moac"; 
    sinfo << std::setw(10) << std::setfill(' ') << "# oac" << endmsg;
    for (auto entry: map_multiple_pvs){
      sinfo << std::setw(10) << std::setfill(' ') << entry.first;
      sinfo << std::setw(10) << std::setfill(' ') << entry.second << endmsg;
    }
    sinfo << "Total number of multiple occurences (>1) of additional characteristics: " << num_multiple_pvs << endmsg;
    sinfo << endmsg;
    sinfo << "Number of multiple occurences of additional characteristics (# moac) vs. number of occurrences (# evts): " << num_events_with_multiple_pvs << " (" << static_cast<double>(num_events_with_multiple_pvs)/num_entries*100 << "%)" << endmsg;
    sinfo << std::setw(10) << std::setfill(' ') << "# moac"; 
    sinfo << std::setw(10) << std::setfill(' ') << "# evts" << endmsg;
    for (auto entry: map_events_with_multiple_pvs){
      sinfo << std::setw(10) << std::setfill(' ') << entry.first;
      sinfo << std::setw(10) << std::setfill(' ') << entry.second << endmsg;
    }
    sinfo << endmsg;
    sinfo << "Number of candidates after single candidate selection: " << num_singlecands << endmsg;
    sinfo << "Number of entries in tree: " << num_entries << endmsg;
    sinfo.Ruler();
  }
  
  input_tree_->SetBranchStatus("*", true);
}
} // na mespace reducer
} // namespace dooselection