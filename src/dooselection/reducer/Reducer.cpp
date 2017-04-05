#include "Reducer.h"

// from STL
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <cassert>

// POSIX/UNIX
#include <unistd.h>

// from BOOST
//#ifdef __GNUG__
//#define BOOST_NO_CXX11_SCOPED_ENUMS
//#endif
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>
#include <boost/bimap.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

// from ROOT
#include "TROOT.h"
#include "TFile.h"
#include "TString.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TLeaf.h"
#include "TTreeFormula.h"
#include "TRandom.h"
#include "TStopwatch.h"
#include "TTreeFormula.h"

// from RooFit
#include "RooArgSet.h"

// from DooCore
#include <doocore/io/MsgStream.h>
#include <doocore/io/Progress.h>

using namespace doocore::io;
using namespace std;

namespace dooselection {
namespace reducer {

typedef boost::bimap<TString, TString> bimap;

bool Reducer::abort_loop_ = false;

Reducer::Reducer() : 
event_number_leaf_ptr_(NULL),
run_number_leaf_ptr_(NULL),
interim_file_(NULL),
formula_input_tree_(NULL),
best_candidate_leaf_ptr_(NULL),
num_events_process_(-1),
old_style_interim_tree_(false),
overwrite_existing_leaves_(false)
{
  GenerateInterimFileName();
}

Reducer::~Reducer(){
//  sdebug << "Reducer::~Reducer()" << endmsg;

  for (std::vector<ReducerLeaf<Float_t>* >::const_iterator it = interim_leaves_.begin(); it != interim_leaves_.end(); ++it) {
    delete *it;
  }
  for (std::vector<ReducerLeaf<Double_t>* >::const_iterator it = double_leaves_.begin(); it != double_leaves_.end(); ++it) {
    delete *it;
  }
  for (std::vector<ReducerLeaf<Float_t>* >::const_iterator it = float_leaves_.begin(); it != float_leaves_.end(); ++it) {
    delete *it;
  }
  for (std::vector<ReducerLeaf<Int_t>* >::const_iterator it = int_leaves_.begin(); it != int_leaves_.end(); ++it) {
    delete *it;
  }
  
  if (formula_input_tree_ != NULL) {
    delete formula_input_tree_;
  }
  
  if (interim_file_ != NULL) {
    interim_file_->Close();
    delete interim_file_;
  }
  if (input_file_ != NULL) {
    input_file_->Close();
    delete input_file_;
  }
  
  // TODO: delete file and tree pointers
}

void Reducer::Initialize(){
  TTree::SetMaxTreeSize(1000000000000);
  
  PrepareIntitialTree();
  PrepareFinalTree();
}

void Reducer::PrepareIntitialTree() {
  OpenInputFileAndTree();
  ProcessInputTree();
  InitializeBranches();
}

void Reducer::PrepareFinalTree() {
  CreateInterimFileAndTree();
  CreateOutputFileAndTree();
  CreateSpecialBranches();
}

void Reducer::Run(){
  signal(SIGINT, Reducer::HandleSigInt);

  PrepareSpecialBranches();
  
  sinfo << "All branches that new leaves depend on are kept. " << endmsg;
  ActivateDependentLeaves(float_leaves_);
  ActivateDependentLeaves(double_leaves_);
  ActivateDependentLeaves(int_leaves_);
  ActivateDependentLeaves(ulong_leaves_);
  ActivateDependentLeaves(long_leaves_);

  float_leaves_  = PurgeOutputBranches<Float_t,Float_t>(float_leaves_, interim_leaves_);
  double_leaves_ = PurgeOutputBranches<Double_t,Float_t>(double_leaves_, interim_leaves_);
  int_leaves_    = PurgeOutputBranches<Int_t,Float_t>(int_leaves_, interim_leaves_);
  
  std::cout << "Initializing new branches of output tree" << std::endl;
  InitializeOutputBranches<Float_t>(output_tree_, interim_leaves_);
  InitializeOutputBranches<Float_t>(output_tree_, float_leaves_);
  InitializeOutputBranches<Double_t>(output_tree_, double_leaves_);
  InitializeOutputBranches<Int_t>(output_tree_, int_leaves_);
  
  int num_entries         = interim_tree_->GetEntries();
  
  if (num_events_process_ != -1) {
    num_entries = num_events_process_;
  }
  
  num_written_ = 0;
  //unsigned int num_written         = 0;
  //unsigned int num_best_candidates = 0;
  
  std::cout << "Writing OutputTree for " << num_entries << " entries in interim tree." << std::endl;

  if (event_number_leaf_ptr_ == NULL || run_number_leaf_ptr_ == NULL || best_candidate_leaf_ptr_ == NULL) {
    std::cout << "Using no best candidate selection (due to not set event number, run number or best candidate leaf)." << std::endl;
  } else {
    std::cout << "Using best candidate selection for leaf " << best_candidate_leaf_ptr_->name() << std::endl;
  }
  
  int i = 0;
//  int status_stepping       = num_entries>100000 ? num_entries/10000 : 10;
//  int status_stepping_redir = num_entries>1000 ? num_entries/100 : 10;
  
  TStopwatch sw;
  sw.Start();
  
  Progress p("Writing output tree", num_entries);
  int last_i = 0;
  while (i<num_entries) {
  //for (int i=0; i<num_entries; ++i) {
    // best candidate selection: 
    i = GetBestCandidate(interim_tree_, i, num_entries);
    //  ^^ will jump to best candidate in event and return beginning of next event
    
    //interim_tree_->GetEntry(i);
    
    // in the case that GetBestCandidate could not find any more events passing 
    // the cuts, we will not write the loaded event.
    if (i != -1) {
      FillOutputTree();
      
      //++num_best_candidates;
    } else {
      break;
    }
    
    p += (i-last_i);
    last_i = i;
    
//    if (isatty(fileno(stdout))) {
//      //std::cout << (i+1) << std::endl;
//      //std::cout << (i+1)%1000 << std::endl;
////      sdebug << "num_written_%status_stepping = " << num_written_ << "%" << status_stepping << " = " << num_written_%status_stepping << endmsg;
//      if ((num_written_%status_stepping) == 0) {
//        double frac = static_cast<double>(i)/num_entries;
//        double time = sw.RealTime();
//        double ete  = time/frac-time;
//        sw.Start(false);
//        if (ete > 3600) {
//          printf("Progress %.2f %   (ETE: %.1f h, spent: %.0f s, t/evt: %.2f ms)      \xd", frac*100.0, ete/3600., time, time/num_written_*1000);
//        } else if (ete > 60) {
//          printf("Progress %.2f %   (ETE: %.0f min, spent: %.0f s, t/evt: %.2f ms)      \xd", frac*100.0, ete/60., time, time/num_written_*1000);
//        } else {
//          printf("Progress %.2f %   (ETE: %.0f s, spent: %.0f s, t/evt: %.2f ms)      \xd", frac*100.0, ete, time, time/num_written_*1000);
//        }
//        fflush(stdout);
//      }
//    } else {
//      if ((num_written_%status_stepping_redir) == 0) {
//        double frac = static_cast<double>(i)/num_entries;
//        double time = sw.RealTime();
//        double ete  = time/frac-time;
//        sw.Start(false);
//        if (ete > 3600) {
//          printf("Progress %.2f %   (ETE: %.1f h, spent: %.0f s, t/evt: %.2f ms)\n", frac*100.0, ete/3600., time, time/num_written_*1000);
//        } else if (ete > 60) {
//          printf("Progress %.2f %   (ETE: %.0f min, spent: %.0f s, t/evt: %.2f ms)\n", frac*100.0, ete/60., time, time/num_written_*1000);
//        } else {
//          printf("Progress %.2f %   (ETE: %.0f s, spent: %.0f s, t/evt: %.2f ms)\n", frac*100.0, ete, time, time/num_written_*1000);
//        }
//        fflush(stdout);
//      }
//    }
    
    if (abort_loop_) {
      std::cout << "Aborting loop..." << std::endl;
      abort_loop_ = false;
      num_entries = i+1;
      break;
    }
  }
  p.Finish();
  double time = sw.RealTime();
  sinfo << "Processing event loop took " << time << " s (" << time/num_written_*1000 << " ms/event).                                 " << endmsg;
  
  output_tree_->Write();
  sinfo << "OutputTree " << output_tree_path_ << " written to file " << output_file_path_ << " with " << num_written_ << " candidates." << endmsg; // "(" << num_best_candidates << " were best candidates without special cuts)." << endl;
  
  output_file_->Close();
  delete output_file_;
  output_file_ = NULL;
  
  sinfo << "Removing interim file " << interim_file_path_ << endmsg;
  using namespace boost::filesystem;
  remove(path(interim_file_path_));
}
  
void Reducer::FillOutputTree() {
//  int num_pvs = GetInterimLeafByName("B0_FitDaughtersPVConst_nPV").GetValue();
//  ReducerLeaf<Int_t>&    flat_leaf_index = CreateIntLeaf("index_pv");
//  ReducerLeaf<Double_t>& flat_leaf_value = CreateDoubleLeaf("value_pv");
//  
//  for (int i=0; i<num_pvs; ++i) {
//    flat_leaf_index = i;
//    flat_leaf_value = GetInterimLeafByName("B0_FitDaughtersPVConst_M").GetValue(i);
//    FlushEvent();
//  }
  FlushEvent();
}
  
void Reducer::LoadTreeFriendsEntryHook(long long entry) {
  for (std::vector<TTree*>::iterator it=additional_input_tree_friends_.begin(), end=additional_input_tree_friends_.end(); it!=end; ++it) {
    (*it)->GetEntry( entry<=(*it)->GetEntries() ? entry : (*it)->GetEntries() );
  }
}

void Reducer::FlushEvent() {
  output_tree_->Fill();
  ++num_written_;
}

void Reducer::Finalize(){
  
}


// ==========================================================================
// = Functions for creation and readout of config file                      =
// ==========================================================================
//void Reducer::ReadConfigFile(){
//  
//}
//
//void Reducer::WriteConfigFile() const{
//  
//}


// ==========================================================================
// = Setters for filename and path                                          =
// ==========================================================================
void Reducer::set_input_file_path(TString const& input_file_path ){
  input_file_path_ = TString(input_file_path);
}

void Reducer::set_input_tree_path(TString const& input_tree_path ){
  input_tree_path_ = TString(input_tree_path);
}

void Reducer::set_interim_file_path(TString const& interim_file_path ){
  swarn << "NOTE: Setting of interim file name is deprecated. Reducer can automatically generate an interim file name." << endmsg;
  interim_file_path_ = TString(interim_file_path);
}

void Reducer::set_output_file_path(TString const& output_file_path ){
  output_file_path_ = TString(output_file_path);
}

void Reducer::set_output_tree_path(TString const& output_tree_path ){
  output_tree_path_ = TString(output_tree_path);
}

void Reducer::OpenInputFileAndTree(){
  cout << "Opening InputFile " << input_file_path_ << endl;
  input_file_ = new TFile(input_file_path_,"READ");
  
  cout << "Opening InputTree " << input_tree_path_ << endl;
  input_tree_ = (TTree*)input_file_->Get(input_tree_path_);
  
  for (std::vector<TTree*>::iterator it=additional_input_tree_friends_.begin(), end=additional_input_tree_friends_.end(); it!=end; ++it) {
    
    if (input_tree_->GetEntries() != (*it)->GetEntries()) {
      swarn << "Error in Reducer::OpenInputFileAndTree(): Input tree " << input_tree_->GetName() << " and friend " << (*it)->GetName() << " do not have equal number of entries (" << input_tree_->GetEntries() << " vs. " << (*it)->GetEntries() << "). " << endmsg;
    }
  }
  
  if (input_tree_ == NULL || TString(input_tree_->IsA()->GetName()).CompareTo("TTree") != 0) {
    serr << "Could not open input tree! Giving up." << endmsg;
    input_file_->ls();
    throw 1;
  }
}
  
void Reducer::AddTreeFriend(std::string file_name, std::string tree_name) {
  TFile* input_file = new TFile(file_name.c_str());
  TTree* input_tree = (TTree*)input_file->Get(tree_name.c_str());
  
  sinfo << "Adding tree " << file_name << ":" << tree_name << " as friend of input tree." << endmsg;
  additional_input_tree_friends_.push_back(input_tree);
}

void Reducer::CreateInterimFileAndTree(){
  gROOT->cd();
  
  if (!CreateUniqueInterimTree()) {
    sinfo << "Using input tree as interim tree." << endmsg;
    
    interim_tree_ = input_tree_;
    
    if (cut_string_.Length() > 0) {
      sinfo << "Initializing tree formula with cut " << cut_string_ << endmsg;
      
      // split cut string into tokens to iterate through these and reactivate all necessary leaves
      std::string cut_string(cut_string_.Data());
      std::vector<std::string> vector_split;
      boost::split( vector_split, cut_string, boost::is_any_of("(())&|<>="), boost::token_compress_on );
      for (auto token : vector_split) {
        TLeaf* leaf = input_tree_->GetLeaf(token.c_str());
        if (leaf != NULL) {
          //sdebug << "Reactivating " << token << " (needed for cut string)" << endmsg;
          input_tree_->SetBranchStatus(token.c_str(), 1);
        }
      }
      
      formula_input_tree_ = new TTreeFormula("formula_input_tree_", cut_string_, interim_tree_);
      
      if (formula_input_tree_->GetNdim() == 0) {
        serr << "Error in Reducer::CreateInterimFileAndTree(): Cut string cannot be evaluated. " << endmsg;
        throw 32;
      }
    } else {
      sinfo << "Copying tree without specific cut (cuts may apply through higher level Reducers)." << endmsg;
    }
    
    if (num_events_process_ != -1) {
      swarn << "Warning: Copying only " << num_events_process_ << " events." << endmsg;
    }
  } else {
  
    cout << "Creating InterimFile " << interim_file_path_ << endl;
    interim_file_ = new TFile(interim_file_path_,"RECREATE");
    
    if (num_events_process_ == -1 && cut_string_.Length() == 0) {
      serr << "Error in Reducer::CreateInterimFileAndTree(): This should never happen. " << endmsg;
      assert(false);
    } else if (num_events_process_ == -1) {
      cout << "Creating InterimTree with cut " << cut_string_ << endl;
      interim_tree_ = input_tree_->CopyTree(cut_string_/*, "", 2000*/);
    } else {
      cout << "Creating InterimTree with cut " << cut_string_ << ", copying only "
      << num_events_process_ << " events." << endl;
      interim_tree_ = input_tree_->CopyTree(cut_string_, "", num_events_process_);
    }
    interim_tree_->Write();
    input_tree_ = NULL;
    cout << "Closing InputFile." << endl;
    input_file_->Close();
  }
}

void Reducer::CreateOutputFileAndTree(){
  cout << "Creating OutputFile " << output_file_path_ << endl;
  output_file_ = new TFile(output_file_path_,"RECREATE");
  
  if (!output_file_->IsOpen()) {
    serr << "Cannot open output file " << output_file_path_ << endmsg;
    throw 12;
  }
  
  cout << "Creating OutputTree " << output_tree_path_ << endl;
  output_tree_ = new TTree(output_tree_path_, "GrimReaperTree");
  
  InitializeInterimLeafMap(interim_tree_, &interim_leaves_);
  RenameBranches(&interim_leaves_, name_mapping_);
}


// ==========================================================================
// = Setters and functions for omitting and keeping branches                =
// ==========================================================================
void Reducer::add_branch_keep( TString const& branch_keep){
  branches_keep_.insert(branch_keep);
}

void Reducer::add_branch_omit( TString const& branch_omit){
  branches_omit_.insert(branch_omit);
}

void Reducer::add_branches_keep(std::set<TString> const& branches_keep){
  for(std::set<TString>::const_iterator it = branches_keep.begin(); it != branches_keep.end(); it++){
    branches_keep_.insert(*it);
  }
}

void Reducer::add_branches_omit(std::set<TString> const& branches_omit){
  for(std::set<TString>::const_iterator it = branches_omit.begin(); it != branches_omit.end(); it++){
    branches_omit_.insert(*it);
  }
}
  
void Reducer::AddBranchesKeep(const RooArgSet& argset) {
  TIterator* args_it = argset.createIterator();
  RooAbsArg* arg = NULL;
  
  while ((arg = dynamic_cast<RooAbsArg*>(args_it->Next()))) {
    add_branch_keep(arg->GetName());
  }
}

void Reducer::InitializeBranches(){
  // iterate over regex containers for keeping/omitting branches and fill
  // branches_keep_/branches_omit_ accordingly
  TObjArray* leaf_list = input_tree_->GetListOfLeaves();
  int num_leaves       = leaf_list->GetEntries();
  
  // iterate over all leaves and check regex matching
  for (int i=0; i<num_leaves; ++i) {
    for (std::vector<std::string>::const_iterator it_regex = branches_keep_regex_.begin(), end = branches_keep_regex_.end();
         it_regex != end; ++it_regex) {
      boost::regex regex(*it_regex);
      if (regex_match((*leaf_list)[i]->GetName(),regex)) {
        branches_keep_.insert((*leaf_list)[i]->GetName());
      }
    }
    for (std::vector<std::string>::const_iterator it_regex = branches_omit_regex_.begin(), end = branches_omit_regex_.end();
         it_regex != end; ++it_regex) {
      boost::regex regex(*it_regex);
      if (regex_match((*leaf_list)[i]->GetName(),regex)) {
        branches_omit_.insert((*leaf_list)[i]->GetName());
      }
    }
  }
  
  // Keep/omit branches according to filled containers. Keep supercedes omit.
  if ( !branches_keep_.empty() ){
    // Deactivate all branches
    input_tree_->SetBranchStatus("*",0);
    
    // Loop over all branches that should be kept
    cout << "Keeping following branches:" << endl;
    for ( set<TString>::iterator it = branches_keep_.begin(); 
          it != branches_keep_.end(); ++it){
      cout << "  " << *it << endl;
      // Check if tree exists
      if ( input_tree_->FindBranch(*it) == 0){
        cout << "    " << "Branch does not exist in tree! Skipping." << endl;
        continue;
      }
      else{
        input_tree_->SetBranchStatus(*it,1);
      }
    }
    // now loop over all branches that are used in name mapping
    cout << "Additionally keep all branches that are used in renaming process:" << endl;
    for (bimap::left_const_iterator it_map = name_mapping_.left.begin(); it_map != name_mapping_.left.end(); ++it_map) {
      cout << "  " << (*it_map).first << endl;
      // Check if tree exists
      if ( input_tree_->FindBranch((*it_map).first) == 0){
        cout << "    " << "Branch does not exist in tree! Skipping." << endl;
        continue;
      }
      else{
        input_tree_->SetBranchStatus((*it_map).first,1);
      }
    }
    
    if (event_number_leaf_ptr_ != NULL) {
      input_tree_->SetBranchStatus(event_number_leaf_ptr_->name(),1);
    }
    if (run_number_leaf_ptr_ != NULL) {
      input_tree_->SetBranchStatus(run_number_leaf_ptr_->name(),1);
    }
    if (best_candidate_leaf_ptr_ != NULL) {
      input_tree_->SetBranchStatus(best_candidate_leaf_ptr_->name(),1);
    }
  }
  // If no branches to keep: Loop over all branches that should be omitted
  else if ( !branches_omit_.empty()){
    cout << "Omitting following branches:" << endl;
    for ( set<TString>::iterator it = branches_omit_.begin(); 
          it != branches_omit_.end(); ++it){
      cout << "  " << *it << endl;
      // Check if tree exists
      if ( input_tree_->FindBranch(*it) == 0){
        cout << "    " << "Branch does not exist in tree! Skipping." << endl;
        continue;
      }
      else{
        // check if branch is used in name mapping
        if (name_mapping_.left.count(*it)==0){
          input_tree_->SetBranchStatus(*it,0);
        }
        else{
          cout << "    " << "Branch will be used later in renaming process! Skipping!" << endl;
          continue;
        }
      }
    }
  }
  // No branches to keep, no branches to omit. Exit.
  else{
    cout << "No branches specified to keep or to omit. Keeping all." << endl;
    input_tree_->SetBranchStatus("*",1);
  }
}

unsigned int Reducer::GetBestCandidate(TTree* tree, unsigned int pos_event_start, unsigned int num_entries) {
  unsigned int i = pos_event_start;
  
  GetTreeEntryUpdateLeaves(tree, i);
  
  if (event_number_leaf_ptr_ == NULL || run_number_leaf_ptr_ == NULL || best_candidate_leaf_ptr_ == NULL) {
    // without best candidate selection
    while (i<num_entries) {
      if ((formula_input_tree_ == NULL || formula_input_tree_->EvalInstance() != 0) && EntryPassesSpecialCuts()) {
        //std::cout << i << " " << runNumber << " " << eventNumber << std::endl;
        return i+1;
      }
      ++i;
      GetTreeEntryUpdateLeaves(tree, i);
    }
    
    /* 
     * return some -1 if we're past the tuple's end (this will let ::Run() know
     * that we're finished)
     * if not, the loop will just go round and round and round and round and...
     */
    return -1;
  } else {
    // with best candidate selection
    
    /*
     * Everything in here is a bit complicated and weird since it involves
     * some tricky conditions and also recursion.
     * As I tested it with special testing tuples to test certain cases I am
     * quite confident it does what it is supposed to do.
     */
    
    ULong64_t run_number_before          = run_number_leaf_ptr_->GetValue();
    ULong64_t event_number_before        = event_number_leaf_ptr_->GetValue();
    //Float_t best_candidate_value_before = *best_candidate_value_ptr_;
    // Double_t best_candidate_value_before = best_candidate_leaf_ptr_->GetValue();
    Double_t best_candidate_value_min    = -1.0;
    Int_t best_candidate                 = -1;
    
    // while we're still in the tuple and inside the current event...
    while (i<num_entries && run_number_before == run_number_leaf_ptr_->GetValue() && event_number_before == event_number_leaf_ptr_->GetValue()) {
      run_number_before           = run_number_leaf_ptr_->GetValue();
      event_number_before         = event_number_leaf_ptr_->GetValue();
      // best_candidate_value_before = best_candidate_leaf_ptr_->GetValue();
      // check if event passes special cuts and if event is better than current
      // best candidate or if there is no best candidate in this event yet
      if ((formula_input_tree_ == NULL || formula_input_tree_->EvalInstance() != 0) && EntryPassesSpecialCuts() && (best_candidate_leaf_ptr_->GetValue() < best_candidate_value_min || best_candidate_value_min == -1)) {
        best_candidate_value_min = best_candidate_leaf_ptr_->GetValue();
        best_candidate           = i;
      }
      ++i;
      // load next event
      GetTreeEntryUpdateLeaves(tree, i);
      
      //std::cout << "min = " << best_candidate_value_min << ", this = " << best_candidate_leaf_ptr_->GetValue() << ", best = " << best_candidate << ", this = " << i << ", start = " << pos_event_start << std::endl;
    }
    if (best_candidate != -1) {
      // quick and dirty intervention: do not really use best candidate but a
      // random one in this event
//      int random_best_candidate = gRandom->Rndm()*(i-pos_event_start)+static_cast<double>(pos_event_start)+0.5;
//      if ((i-1)-pos_event_start > 1) {
//        std::cout << "event range from " << pos_event_start << " to " << i-1 << ", best is " << best_candidate << ", random best is: " << random_best_candidate << ", next event start " << i << std::endl;
//        best_candidate = random_best_candidate;
//      }
      
      GetTreeEntryUpdateLeaves(tree, best_candidate);
    } else if (i<num_entries) {
      // This case will be met if no entries in this event pass the special cuts. 
      // i is still < num_entries (to prevent infinite loops in case last entry
      // does not satisfy special cuts). We'll just start the next iteration
      i = GetBestCandidate(tree,i,num_entries);
    } else {
      // i == num_entries && best_candidate == -1
      // this means we are past the end of the tree and we could not find any more 
      // good events passing the special cuts. We'll return -1.
      return -1;
    }
    
    //std::cout << "event range from " << pos_event_start << " to " << i-1 << ", best is " << best_candidate << ", next event start " << i << std::endl;
    
    return i;
  }
}

void Reducer::InitializeInterimLeafMap(TTree* tree, std::vector<ReducerLeaf<Float_t>* >* leaves) {
  std::cout << "Initializing leaves of interim tree" << std::endl;
  
  TObjArray* leaf_list = tree->GetListOfLeaves();
  int num_leaves       = leaf_list->GetEntries();
  
  // iterate over all leaves and sort them into map
  for (int i=0; i<num_leaves; ++i) {
    TLeaf* leaf_tree = dynamic_cast<TLeaf*>((*leaf_list)[i]);
    
    //sdebug << " leaf: " << leaf_tree->GetName() << " - " << leaf_tree->GetBranch()->TestBit(1024) << endmsg;
    if (!leaf_tree->GetBranch()->TestBit(1024)) {
      ReducerLeaf<Float_t>* leaf = new ReducerLeaf<Float_t>(leaf_tree);
      leaves->push_back(leaf);
    }
  }
  
  for (std::vector<TTree*>::iterator it=additional_input_tree_friends_.begin(), end=additional_input_tree_friends_.end(); it!=end; ++it) {
    TObjArray* leaf_list = (*it)->GetListOfLeaves();
    int num_leaves       = leaf_list->GetEntries();
    
    // iterate over all leaves and sort them into map
    for (int i=0; i<num_leaves; ++i) {
      TLeaf* leaf_tree = dynamic_cast<TLeaf*>((*leaf_list)[i]);
      
      //sdebug << " leaf: " << leaf_tree->GetName() << " - " << leaf_tree->GetBranch()->TestBit(1024) << endmsg;
      if (!leaf_tree->GetBranch()->TestBit(1024) && !LeafExists(leaf_tree->GetName())) {
        ReducerLeaf<Float_t>* leaf = new ReducerLeaf<Float_t>(leaf_tree);


        // for (auto )
        // leaf->set_name(leaf->name() + "_bla");

        leaves->push_back(leaf);
      } else if (LeafExists(leaf_tree->GetName())) {
        swarn << "Warning in Reducer::InitializeInterimLeafMap(...): Leaf " << leaf_tree->GetName() << " in friend tree " << (*it)->GetName() << " already existing. Will rename to keep it unique." << endmsg;

        ReducerLeaf<Float_t>* leaf = new ReducerLeaf<Float_t>(leaf_tree);
        leaf->set_name(leaf->name() + "_1");

        leaves->push_back(leaf);
      }
    }

  }
  
  std::cout << num_leaves << " total leaves in interim tree" << std::endl;
  std::cout << leaves->size() << " leaves to be copied" << std::endl;
}

void Reducer::RenameBranches(std::vector<ReducerLeaf<Float_t>* >* leaves, const boost::bimap<TString, TString>& mapping) {
  for (bimap::left_const_iterator it_map = mapping.left.begin(); it_map != mapping.left.end(); ++it_map) {
    // (*it_map).first  : old_name : TString
    // (*it_map).second : new_name : TString
    for (std::vector<ReducerLeaf<Float_t>* >::iterator it_leaf = leaves->begin(); it_leaf != leaves->end(); ++it_leaf) {
      if ((*it_map).first == (*it_leaf)->name()) {
        std::cout << "Leaf " << (*it_leaf)->name() << " shall be known as " << (*it_map).second << std::endl;
        (*it_leaf)->set_name((*it_map).second);
      }
    }
  }
}

template<class T>
void Reducer::InitializeOutputBranches(TTree* tree, const std::vector<ReducerLeaf<T>* >& leaves) {
  for (typename std::vector<ReducerLeaf<T>* >::const_iterator it = leaves.begin(); it != leaves.end(); ++it) {
    (*it)->CreateBranch(tree);
  }
}
  
template<class T1,class T2>
std::vector<ReducerLeaf<T1>*> Reducer::PurgeOutputBranches(const std::vector<ReducerLeaf<T1>* >& leaves, const std::vector<ReducerLeaf<T2>* >& interim_leaves) const {
  std::vector<ReducerLeaf<T1>* > purged_leaves;
  for (typename std::vector<ReducerLeaf<T1>* >::const_iterator it = leaves.begin(); it != leaves.end(); ++it) {
    bool found = false;
    if (overwrite_existing_leaves_) {
      purged_leaves.push_back(*it);
    }
    else {
      for (typename std::vector<ReducerLeaf<T2>* >::const_iterator it_ex = interim_leaves.begin(); it_ex != interim_leaves.end(); ++it_ex) {
        if ((*it_ex)->name() == (*it)->name()) {
          found = true;
          break;
        }
      }
      if (found) {
        swarn << "New leaf " << (*it)->name() << " already existing. Will ignore." << endmsg;
      } else {
        purged_leaves.push_back(*it);
      }
    }
  }
  return purged_leaves;
}

template<class T>
void Reducer::UpdateAllValues(std::vector<ReducerLeaf<T>* >& leaves) {
  for (typename std::vector<ReducerLeaf<T>* >::iterator it = leaves.begin(); it != leaves.end(); ++it) {
    (*it)->UpdateValue();
  }
}

void Reducer::GenerateInterimFileName() {
  using namespace boost::filesystem;
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  std::string s_uuid = boost::lexical_cast<std::string>(uuid);

//  using namespace doocore::io;
//  sdebug << "The uuid is: " << s_uuid << endmsg;
  
  path tempfile = temp_directory_path() / s_uuid;
  tempfile.replace_extension(".root");
  
  interim_file_path_ = tempfile.string();
}

// ==========================================================================
// = Setters for cut string manipulation                                    =
// ==========================================================================
void Reducer::set_cut_string( TString const& cut_string ){
  cut_string_ = cut_string;
}

void Reducer::add_cut_string( TString const& partial_cut_string ){
  cut_string_ += partial_cut_string;
}

TString const& Reducer::cut_string() const{
  return cut_string_;
}

void Reducer::HandleSigInt(int) {
  std::cout << "Caught Ctrl-C. Will abort loop." << std::endl << std::endl;
  
  // in case of second sigint exit the program
  if (abort_loop_) exit(1);
  
  abort_loop_ = true;
}
  
const ReducerLeaf<Float_t>& Reducer::GetInterimLeafByName(const TString& name) {
  const ReducerLeaf<Float_t>* leaf = NULL;
  try {
    leaf = &GetLeafByName<Float_t>(name, interim_leaves_);
  } catch (int e) {
    if (e==10 && !CreateUniqueInterimTree()) {
      //swarn << "Reducer::GetInterimLeafByName(" << name << "): Leaf could not be found in interim tree. Will check if it is available in the input tree but deactivated." << endmsg;
      
      TLeaf* leaf_input_tree = input_tree_->GetLeaf(name);
      if (leaf_input_tree != NULL) {
        input_tree_->SetBranchStatus(name, 1);
        ReducerLeaf<Float_t>* leaf_new = new ReducerLeaf<Float_t>(leaf_input_tree);
        interim_leaves_.push_back(leaf_new);
        swarn << "Reducer::GetInterimLeafByName(" << name << "): Re-enabling deactiavted leaf as it was requested." << endmsg;
        leaf = &GetLeafByName<Float_t>(name, interim_leaves_);
      } else {
        serr << "Reducer::GetInterimLeafByName(" << name << "): Leaf could not be found in interim tree." << endmsg;
        throw e;
      }
    } else {
      serr << "Reducer::GetInterimLeafByName(" << name << "): Leaf could not be found in interim tree. Maybe it is deactivated and was not copied." << endmsg;
      throw e;
    }
  }
  
  return *leaf;
}
  


} // namespace reducer
} // namespace dooselection

      
