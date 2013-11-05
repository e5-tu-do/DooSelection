#include "MergeTupleReducer.h"

// from STL
#include <vector>

// from ROOT
#include "TTree.h"

// from DooCore
#include <doocore/io/MsgStream.h>

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
  
  for (std::vector<std::pair<ReducerLeaf<ULong64_t>,ReducerLeaf<ULong64_t>>>::const_iterator it = event_identifiers.begin();
       it != event_identifiers.end(); ++it) {
    input_tree_->SetBranchStatus(it->first.name(), true);
    (*it_friend)->SetBranchStatus(it->second.name(), true);
  }

  // precaching everything that should not be evaluated in the event loop
  ULong64_t num_entries = (*it_friend)->GetEntries();
  double frac = 0.0;
  int n_print_stepping = 500;
  double frac_increment = static_cast<double>(n_print_stepping)/num_entries*100.0;
  bool tty = isatty(fileno(stdout));
  
  sinfo << "MergeTupleReducer::ProcessInputTree(): Analysing events according to event identifiers." << endmsg;

  long long index_tree = 0;
  input_tree_->GetEvent(index_tree);
  for (long long index_friend=0; index_friend<(*it_friend)->GetEntries(); ++index_friend) {
    (*it_friend)->GetEvent(index_friend);
    
    bool entries_match = false;

    while (!entries_match) {
      entries_match = true;
      for (const std::pair<ReducerLeaf<ULong64_t>,ReducerLeaf<ULong64_t>>& identifier : event_identifiers) {
//        sdebug << "identifier.second.GetValue("<< identifier.second.name() <<") : " << identifier.second.GetValue() << endmsg;
        
        if (identifier.first.GetValue() != identifier.second.GetValue()) {
//          sdebug << "identifier.first.GetValue() != identifier.second.GetValue() : " << identifier.first.GetValue() << " != " << identifier.second.GetValue() << endmsg;
          entries_match = false;
        }
      }
      if (!entries_match) {
        ++index_tree;
        input_tree_->GetEvent(index_tree);
      }
    }
    
    if (entries_match) {
      event_mapping_.push_back(std::make_pair(index_tree, index_friend));
      ++index_tree;
      input_tree_->GetEvent(index_tree);
      
//      sdebug << "index_tree => index_friend : " << index_tree-1 << " => " << index_friend << endmsg;

    }
    
    if (tty && (index_friend%n_print_stepping) == 0) {
      // double frac = static_cast<double> (i)/num_entries*100.0;
      frac += frac_increment;
      printf("Progress %.2f %         \xd", frac);
      fflush(stdout);
    }

  }
  input_tree_->SetBranchStatus("*", true);
}

void dooselection::reducer::MergeTupleReducer::CreateSpecialBranches() {
  leaf_entries_matched_ = &CreateIntLeaf("merged_entries");
  leaf_entries_matched_->SetOperation(*leaf_entries_matched_, *leaf_entries_matched_, kDoNotUpdate);
}

void dooselection::reducer::MergeTupleReducer::LoadTreeFriendsEntryHook(long long entry) {
  const std::pair<long long, long long>& next_map = event_mapping_.front();
  
  if (next_map.first == entry) {
//    sinfo << entry << " corresponds to " << next_map.second << endmsg;
    
    for (std::vector<TTree*>::iterator it=additional_input_tree_friends_.begin(), end=additional_input_tree_friends_.end(); it!=end; ++it) {
      (*it)->GetEvent(next_map.second);
    }
    
    *leaf_entries_matched_ = 1;
    
    event_mapping_.pop_front();
  } else {
    *leaf_entries_matched_ = 0;
  }
}
