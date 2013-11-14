#include "MergeTupleReducer.h"

// from STL
#include <vector>
#include <chrono>

// from ROOT
#include "TTree.h"

// from DooCore
#include <doocore/io/MsgStream.h>
#include <doocore/io/Progress.h>

using namespace doocore::io;

//dooselection::reducer::MergeTupleReducer::MergeTupleReducer() {}

void dooselection::reducer::MergeTupleReducer::AddEventIdentifier(const std::string& name_tree, const std::string& name_friend) {
  event_identifier_names_.push_back(std::make_pair(name_tree,name_friend));
}

void dooselection::reducer::MergeTupleReducer::ProcessInputTree() {
  std::vector<std::pair<ReducerLeaf<ULong64_t>,ReducerLeaf<ULong64_t>>> event_identifiers;
  std::vector<TTree*>::iterator it_friend=additional_input_tree_friends_.begin();

  for (std::vector<std::pair<std::string,std::string>>::const_iterator it = event_identifier_names_.begin(), end=event_identifier_names_.end(); it != end; ++it) {
    TLeaf* leaf_tree   = input_tree_->GetLeaf(it->first.c_str());
    TLeaf* leaf_friend = (*it_friend)->GetLeaf(it->second.c_str());
    
    if (leaf_tree == NULL) {
      serr << "MultipleCandidateAnalyseReducer::AddEventIdentifier(...): Cannot find indentifier leaf " << it->first << " in input tree. Ignoring it." << endmsg;
    } else if (leaf_friend == NULL) {
        serr << "MultipleCandidateAnalyseReducer::AddEventIdentifier(...): Cannot find indentifier leaf " << it->second << " in input tree friend. Ignoring it." << endmsg;
    } else {
      sinfo << "MultipleCandidateAnalyseReducer::AddEventIdentifier(...): Adding " << it->first << "/" << it->second << " as event identifier." << endmsg;
      event_identifiers.push_back(std::make_pair(ReducerLeaf<ULong64_t>(leaf_tree), ReducerLeaf<ULong64_t>(leaf_friend)));
    }
  }
  
  input_tree_->SetBranchStatus("*", false);
  (*it_friend)->SetBranchStatus("*", false);
  
  for (std::vector<std::pair<ReducerLeaf<ULong64_t>,ReducerLeaf<ULong64_t>>>::const_iterator it = event_identifiers.begin();
       it != event_identifiers.end(); ++it) {
    input_tree_->SetBranchStatus(it->first.name(), true);
    (*it_friend)->SetBranchStatus(it->second.name(), true);
  }

  // precaching everything that should not be evaluated in the event loop
  ULong64_t num_entries = (*it_friend)->GetEntries();
  ULong64_t num_entries_tree = input_tree_->GetEntries();
  double frac_matched = 0.0;

  sinfo << "MergeTupleReducer::ProcessInputTree(): Analysing events according to event identifiers." << endmsg;

  long long index_tree = 0;
  input_tree_->GetEvent(index_tree);
  long long index_friend=0;
  doocore::io::Progress p("Event matching",num_entries);
  for (index_friend=0; index_friend<(*it_friend)->GetEntries(); ++index_friend) {
    (*it_friend)->GetEvent(index_friend);
    
    bool entries_match = false;
    
    while (!entries_match) {
      entries_match = true;
      for (const std::pair<ReducerLeaf<ULong64_t>,ReducerLeaf<ULong64_t>>& identifier : event_identifiers) {
        
        if (identifier.first.GetValue() != identifier.second.GetValue()) {
          entries_match = false;
        }
      }
      if (!entries_match) {
        ++index_tree;
        if (index_tree >= num_entries_tree) break;
        input_tree_->GetEvent(index_tree);
      }
    }
    if (index_tree >= input_tree_->GetEntries()) break;
    
    if (entries_match) {
      event_mapping_.push_back(std::make_pair(index_tree, index_friend));
      ++index_tree;
      input_tree_->GetEvent(index_tree);
      
      ++p;
    }
  }
  p.Finish();
  frac_matched = static_cast<double>(event_mapping_.size())/num_entries*100.0;
  sinfo << "MergeTupleReducer::ProcessInputTree(): Finished analysing events. A total of " << frac_matched << "% (" << event_mapping_.size() << " events) have been matched." << endmsg;
  
  input_tree_->SetBranchStatus("*", true);
  (*it_friend)->SetBranchStatus("*", true);
}

void dooselection::reducer::MergeTupleReducer::CreateSpecialBranches() {
  leaf_entries_matched_ = &CreateIntLeaf("merged_entries");
  leaf_entries_matched_->SetOperation(*leaf_entries_matched_, *leaf_entries_matched_, kDoNotUpdate);
  
  for (auto pair : names_friend_leaves_equalise_) {
    const ReducerLeaf<Float_t>& leaf_tree   = GetInterimLeafByName(pair.first.c_str());
    const ReducerLeaf<Float_t>& leaf_friend = GetInterimLeafByName(pair.second.c_str());
    
    if (leaf_tree.type() != leaf_friend.type()) {
      serr << "Error in MergeTupleReducer::CreateSpecialBranches(): Leaves to be equalised " << leaf_tree.name() << "(" << leaf_tree.type() << ") -> " << leaf_friend.name() << "(" << leaf_friend.type() << ") to not have equal types. This will most certainly go wrong" << endmsg;
    }
    
    sinfo << "MergeTupleReducer::CreateSpecialBranches(): Will equalise " << leaf_tree.name() << "(" << leaf_tree.type() << ") -> " << leaf_friend.name() << "(" << leaf_friend.type() << ") for all events of the input tree." << endmsg;
    if (leaf_tree.type() == "Int_t") {
      branch_addresses_equalise_int_.push_back(std::make_pair((Int_t*)leaf_tree.branch_address(), (Int_t*)leaf_friend.branch_address()));
    } else if (leaf_tree.type() == "Float_t") {
      branch_addresses_equalise_float_.push_back(std::make_pair((Float_t*)leaf_tree.branch_address(), (Float_t*)leaf_friend.branch_address()));
    } else if (leaf_tree.type() == "Double_t") {
      branch_addresses_equalise_double_.push_back(std::make_pair((Double_t*)leaf_tree.branch_address(), (Double_t*)leaf_friend.branch_address()));
    } else if (leaf_tree.type() == "UInt_t") {
      branch_addresses_equalise_uint_.push_back(std::make_pair((UInt_t*)leaf_tree.branch_address(), (UInt_t*)leaf_friend.branch_address()));
    } else if (leaf_tree.type() == "Bool_t") {
      branch_addresses_equalise_bool_.push_back(std::make_pair((Bool_t*)leaf_tree.branch_address(), (Bool_t*)leaf_friend.branch_address()));
    } else if (leaf_tree.type() == "Long64_t") {
      branch_addresses_equalise_long_.push_back(std::make_pair((Long64_t*)leaf_tree.branch_address(), (Long64_t*)leaf_friend.branch_address()));
    } else if (leaf_tree.type() == "Long64_t") {
      branch_addresses_equalise_long_.push_back(std::make_pair((Long64_t*)leaf_tree.branch_address(), (Long64_t*)leaf_friend.branch_address()));
    } else if (leaf_tree.type() == "ULong64_t") {
      branch_addresses_equalise_ulong_.push_back(std::make_pair((ULong64_t*)leaf_tree.branch_address(), (ULong64_t*)leaf_friend.branch_address()));
    } else if (leaf_tree.type() == "Short_t") {
      branch_addresses_equalise_short_.push_back(std::make_pair((Short_t*)leaf_tree.branch_address(), (Short_t*)leaf_friend.branch_address()));
    } else if (leaf_tree.type() == "UShort_t") {
      branch_addresses_equalise_ushort_.push_back(std::make_pair((UShort_t*)leaf_tree.branch_address(), (UShort_t*)leaf_friend.branch_address()));
    } else if (leaf_tree.type() == "Char_t") {
      branch_addresses_equalise_char_.push_back(std::make_pair((Char_t*)leaf_tree.branch_address(), (Char_t*)leaf_friend.branch_address()));
    } else if (leaf_tree.type() == "UChar_t") {
      branch_addresses_equalise_uchar_.push_back(std::make_pair((UChar_t*)leaf_tree.branch_address(), (UChar_t*)leaf_friend.branch_address()));
    }
  }
}

void dooselection::reducer::MergeTupleReducer::LoadTreeFriendsEntryHook(long long entry) {
  const std::pair<long long, long long>& next_map = event_mapping_.front();
  
  if (next_map.first == entry) {
//    sinfo << entry << " corresponds to " << next_map.second << endmsg;
    
    for (std::vector<TTree*>::iterator it=additional_input_tree_friends_.begin(), end=additional_input_tree_friends_.end(); it!=end; ++it) {
      (*it)->GetEvent(next_map.second);
    }
    
    *leaf_entries_matched_ = 1;
    
//    sdebug << "i: " << additional_input_tree_friends_.back()->GetLeaf("netOutput")->GetValue() << endmsg;
//    sdebug << "i: " << additional_input_tree_friends_.back()->GetLeaf("netOutput")->GetValuePointer() << endmsg;
//    sdebug << "o: " << GetInterimLeafByName("netOutput").GetValue() << endmsg;
//    sdebug << "o: " << GetInterimLeafByName("netOutput").branch_address() << endmsg;
//
//    sdebug << "i: " << interim_tree_->GetLeaf("runNumber")->GetValue() << endmsg;
//    sdebug << "i: " << interim_tree_->GetLeaf("runNumber")->GetValuePointer() << endmsg;
//    sdebug << "o: " << GetInterimLeafByName("runNumber").GetValue() << endmsg;
//    sdebug << "o: " << GetInterimLeafByName("runNumber").branch_address() << endmsg;
    
    event_mapping_.pop_front();
  } else {
    *leaf_entries_matched_ = 0;
    
    for (auto pair : branch_addresses_equalise_int_) {
      *pair.second = *pair.first;
    }
    for (auto pair : branch_addresses_equalise_float_) {
      *pair.second = *pair.first;
    }
    for (auto pair : branch_addresses_equalise_double_) {
      *pair.second = *pair.first;
    }
    for (auto pair : branch_addresses_equalise_uint_) {
      *pair.second = *pair.first;
    }
    for (auto pair : branch_addresses_equalise_bool_) {
      *pair.second = *pair.first;
    }
    for (auto pair : branch_addresses_equalise_long_) {
      *pair.second = *pair.first;
    }
    for (auto pair : branch_addresses_equalise_ulong_) {
      *pair.second = *pair.first;
    }
    for (auto pair : branch_addresses_equalise_short_) {
      *pair.second = *pair.first;
    }
    for (auto pair : branch_addresses_equalise_ushort_) {
      *pair.second = *pair.first;
    }
    for (auto pair : branch_addresses_equalise_char_) {
      *pair.second = *pair.first;
    }
    for (auto pair : branch_addresses_equalise_uchar_) {
      *pair.second = *pair.first;
    }
  }
}
