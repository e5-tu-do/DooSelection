#ifndef DOOSELECTION_REDUCER_REDUCER_H
#define DOOSELECTION_REDUCER_REDUCER_H

// from STL
#include <set>
#include <string>
#include <vector>
#include <map>
#include <iostream>

// from ROOT
#include "TString.h"
#include "TTree.h"
#include "TTreeFormula.h"

#include "ReducerLeaf.h"

// forward decalarations
class TFile;
class TLeaf;

/**
 * @namespace dooselection::reducer
 * @brief Reducer namespace
 *
 * This namespace contains all Reducer functionality.
 */

/**
 * @class dooselection::reducer::Reducer
 * @brief Tool to modify, extend and reduce tuples
 *
 * The Reducer can be used to apply selections to tuples, transform variables, 
 * add new variables and higher level information into ROOT tuples. It features
 * a best candidate selection.
 */

namespace dooselection {
namespace reducer {
class Reducer {
 /** \publicsection */
 public:
  Reducer();
  Reducer( std::string const& );
  virtual ~Reducer();

  /** @name Main running functions for Reducer
   *  These functions to let the Reducer run
   */
  ///@{
  void Initialize();
  void Run();
  void Finalize();
  ///@}

  void PrepareIntitialTree();
  void PrepareFinalTree();
  
  void ReadConfigFile();
  void WriteConfigFile() const;
  
  /** @name I/O configuration
   *  These functions control input/output files and trees
   */
  ///@{
  void set_input_file_path(TString const&);
  void set_input_tree_path(TString const&);
  
  void set_interim_file_path(TString const&);
  
  void set_output_file_path(TString const&);
  void set_output_tree_path(TString const&);
  ///@}

  /** @name Branch keeping/omitting
   *  Functions to control which branches to keep/omit
   */
  ///@{
  void add_branch_keep(TString const&);
  void add_branch_omit(TString const&);
  ///@}
  
  /** @name Cut string functions
   *  Functions for setting/extending the cut string
   */
  ///@{
  void set_cut_string(TString const&);
  void add_cut_string(TString const&);
  TString const& cut_string() const;
  ///@}
  
  void AddNameMapping(TString old_name, TString new_name) {
    name_mapping_.push_back(std::pair<TString,TString>(old_name, new_name));
  }
  
  
  /** @name Best candidate selection configuration
   *  Functions controlling best candidate selection
   */
  ///@{
  template<class T>
  void SetEventNumberLeaf(const ReducerLeaf<T>& leaf) {
    event_number_leaf_ptr_ = new ReducerLeaf<ULong64_t>(leaf.name(), leaf.title(), leaf.type(), leaf.tree());
    event_number_leaf_ptr_->set_branch_address(leaf.branch_address());
  }
  template<class T>
  void SetRunNumberLeaf(const ReducerLeaf<T>& leaf) {
    run_number_leaf_ptr_ = new ReducerLeaf<ULong64_t>(leaf.name(), leaf.title(), leaf.type(), leaf.tree());
    run_number_leaf_ptr_->set_branch_address(leaf.branch_address());
  }
  template<class T>
  void SetBestCandidateLeaf(const ReducerLeaf<T>& leaf) {
    best_candidate_leaf_ptr_ = new ReducerLeaf<Double_t>(leaf.name(), leaf.title(), leaf.type(), leaf.tree());
    best_candidate_leaf_ptr_->set_branch_address(leaf.branch_address());
  }
  ///@}

  /** @name Accessing leaves
   *  Functions accessing existing or to-be-created leaves
   */
  ///@{
  const ReducerLeaf<Float_t>& GetInterimLeafByName(const TString& name) const {
    return GetLeafByName<Float_t>(name, interim_leaves_);
  }
  const ReducerLeaf<Double_t>& GetDoubleLeafByName(const TString& name) const {
    return GetLeafByName<Double_t>(name, double_leaves_);
  }
  const ReducerLeaf<Float_t>& GetFloatLeafByName(const TString& name) const {
    return GetLeafByName<Float_t>(name, float_leaves_);
  }
  const ReducerLeaf<Int_t>& GetIntLeafByName(const TString& name) const {
    return GetLeafByName<Int_t>(name, int_leaves_);
  }
  bool LeafExists(std::string name) const {
    // if interim tree not yet created, check for leaf in
    if (interim_leaves_.size() == 0) {
      if (input_tree_->FindBranch(name.c_str()) != NULL) {
        return true;
      } else {
        return false;
      }
    }
    
    for (std::vector<ReducerLeaf<Float_t>* >::const_iterator it = interim_leaves_.begin(); it != interim_leaves_.end(); ++it) {
      if ((*it)->name() == name) return true;
    }
    for (std::vector<ReducerLeaf<Double_t>* >::const_iterator it = double_leaves_.begin(); it != double_leaves_.end(); ++it) {
      if ((*it)->name() == name) return true;
    }
    for (std::vector<ReducerLeaf<Float_t>* >::const_iterator it = float_leaves_.begin(); it != float_leaves_.end(); ++it) {
      if ((*it)->name() == name) return true;
    }
    for (std::vector<ReducerLeaf<Int_t>* >::const_iterator it = int_leaves_.begin(); it != int_leaves_.end(); ++it) {
      if ((*it)->name() == name) return true;
    }
    return false;
  }

  ///@}
  
  /** @name Leaf creation
   *  Functions creating new leaves in the output tree
   */
  ///@{
  ReducerLeaf<Double_t>& CreateDoubleLeaf(TString name, TString title, TString type, Double_t default_value=0.0) {
    ReducerLeaf<Double_t>* new_leaf = new ReducerLeaf<Double_t>(name, title, type, interim_tree_, default_value);
    double_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  ReducerLeaf<Double_t>& CreateDoubleLeaf(TString name, Double_t default_value=0.0) {
    ReducerLeaf<Double_t>* new_leaf = new ReducerLeaf<Double_t>(name, name, "Double_t", interim_tree_, default_value);
    double_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  template<class T>
  ReducerLeaf<Double_t>& CreateDoubleCopyLeaf(TString name, const ReducerLeaf<T>& leaf_to_copy, double c=1.0) {
    ReducerLeaf<Double_t>& new_leaf = CreateDoubleLeaf(name, name, "Double_t");
    new_leaf.Equal(leaf_to_copy, c);
    return new_leaf;
  }
  ReducerLeaf<Float_t>& CreateFloatLeaf(TString name, TString title, TString type, Float_t default_value=0.0) {
    ReducerLeaf<Float_t>* new_leaf = new ReducerLeaf<Float_t>(name, title, type, interim_tree_, default_value);
    float_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  ReducerLeaf<Float_t>& CreateFloatLeaf(TString name, Float_t default_value=0.0) {    
    ReducerLeaf<Float_t>* new_leaf = new ReducerLeaf<Float_t>(name, name, "Float_t", interim_tree_, default_value);
    float_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  template<class T>
  ReducerLeaf<Float_t>& CreateFloatCopyLeaf(TString name, const ReducerLeaf<T>& leaf_to_copy, double c=1.0) {
    ReducerLeaf<Float_t>& new_leaf = CreateFloatLeaf(name, name, "Float_t");
    new_leaf.Equal(leaf_to_copy, c);
    return new_leaf;
  }
  ReducerLeaf<Int_t>& CreateIntLeaf(TString name, TString title, TString type, Int_t default_value=0) {
    ReducerLeaf<Int_t>* new_leaf = new ReducerLeaf<Int_t>(name, title, type, interim_tree_, default_value);
    int_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  ReducerLeaf<Int_t>& CreateIntLeaf(TString name, Int_t default_value=0) {
    ReducerLeaf<Int_t>* new_leaf = new ReducerLeaf<Int_t>(name, name, "Int_t", interim_tree_, default_value);
    int_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  template<class T>
  ReducerLeaf<Int_t>& CreateIntCopyLeaf(TString name, const ReducerLeaf<T>& leaf_to_copy, double c=1.0) {
    ReducerLeaf<Int_t>& new_leaf = CreateIntLeaf(name, name, "Int_t");
    new_leaf.Equal(leaf_to_copy, c);
    return new_leaf;
  }
  ///@}
  
 protected:
  /*
   * Virtual function for derived classes with higher level leaves. This will be 
   * called to compute values of these leaves.
   */
  virtual void UpdateSpecialLeaves() {}
  
  /*
   * Virtual function for derived classes with higher level cuts. This will be 
   * called to check if an event/candidate passes certain requirements.
   */
  virtual bool EntryPassesSpecialCuts() { return true; }
  
  /**
   * @brief Create specialized higher level branches
   *
   * Virtual function for derived classes with higher level leaves. This will 
   * be called to initialize higher level branches/leaves at the end of
   * Reducer::Initialize().
   * 
   * At the stage of calling this function, the interim tree is already created
   * but in general no user-created leaves will be accessible.
   **/
  virtual void CreateSpecialBranches() {}
  
  /**
   * @brief Prepare specialized higher level branches just before final run
   *
   * Virtual function for derived classes with higher level leaves. This will 
   * be called to initialize higher level branches/leaves at the beginning of 
   * Reducer::Run().
   *
   * At the stage of calling this function, the interim tree is already created
   * and all user-created leaves will be accessible.
   **/
  virtual void PrepareSpecialBranches() {}
 
	/*
	 * Interim tree protected to give derived classed possibility to work with it.
	 */
  TTree* interim_tree_;
  
  /**
   * members needed for best candidate selection
   *
   */
  ReducerLeaf<ULong64_t> * event_number_leaf_ptr_;
  ReducerLeaf<ULong64_t> * run_number_leaf_ptr_;
  
  /**
   *  @brief Currently selected entry of interim tree during best candidate selection
   */
  unsigned int selected_entry_;

 /** \privatesection */
 private:
  void OpenInputFileAndTree();
  void CreateInterimFileAndTree();
  void CreateOutputFileAndTree();
  
  void InitializeBranches();
  
  unsigned int GetBestCandidate(TTree* tree, unsigned int pos_event_start, unsigned int num_entries);
  
  /**
   * Get all leaves from a tree and store them into the vector of ReducerLeafs
   *
   * (unfortunately tree cannot be a const TTree*)
   *
   */
  void InitializeInterimLeafMap(TTree* tree, std::vector<ReducerLeaf<Float_t>* >* leaves);  
  
  /**
   * Rename branches based on a given name mapping vector
   *
   */
  void RenameBranches(std::vector<ReducerLeaf<Float_t>* >* leaves, const std::vector< std::pair<TString,TString> >& mapping); 
  
  /**
   * Initialize branches of a given output tree based on vector of ReducerLeaves
   *
   */
  template<class T>
  void InitializeOutputBranches(TTree* tree, const std::vector<ReducerLeaf<T>* >& leaves);
  
  /**
   * Get a ReducerLeaf by name from a ReducerLeaf vector
   *
   */
  template<class T>
  const ReducerLeaf<T>& GetLeafByName(const TString& name, const std::vector<ReducerLeaf<T>* >& leaves) const;
  
  /**
   * Update values of all leaves in ReducerLeaf vector
   *
   */
  template<class T>
  void UpdateAllValues(std::vector<ReducerLeaf<T>* >& leaves);
  
  /*
   * Get tree entry for tree and update all leaves
   */
  void GetTreeEntryUpdateLeaves(TTree* tree, unsigned int i) {
    selected_entry_ = i;
    tree->GetEntry(i);
    UpdateAllValues<Float_t>(float_leaves_);
    UpdateAllValues<Double_t>(double_leaves_);
    UpdateAllValues<Int_t>(int_leaves_);
    UpdateSpecialLeaves();
  }
  
  /**
   *  @brief Generate interim file if not set
   *
   *  This function will set a unique interim file path if none is set.
   */
  void GenerateInterimFileName();
  
  std::string config_file_;
  
  TString input_file_path_;
  TString input_tree_path_;
  
  TString interim_file_path_;
  
  TString output_file_path_;  
  TString output_tree_path_;
  
  TString cut_string_;
  
  std::set<TString> branches_keep_;
  std::set<TString> branches_omit_;
  
  std::vector< std::pair<TString,TString> > name_mapping_;
  
  std::vector<ReducerLeaf<Float_t>* >  interim_leaves_; ///< vector of all leaves in the interim tree
  std::vector<ReducerLeaf<Float_t>* >  float_leaves_;   ///< new float leaves for output tree

  /**
   *  @brief Container of newly created double leaves.
   */
  std::vector<ReducerLeaf<Double_t>* > double_leaves_;
  
  std::vector<ReducerLeaf<Int_t>* >    int_leaves_;     ///< new int leaves for output tree
  
  TFile* input_file_;
  TTree* input_tree_;
  
  TFile* output_file_;
  TTree* output_tree_;
  
  TFile* interim_file_;
  
  /**
   * members needed for best candidate selection
   *
   */
  ReducerLeaf<Double_t> * best_candidate_leaf_ptr_;
  
  static bool abort_loop_;
  
  ///< SIGINT handler (i.e. CTRL-C)
  static void HandleSigInt(int param);
  
};

template<class T>
const ReducerLeaf<T>& Reducer::GetLeafByName(const TString& name, const std::vector<ReducerLeaf<T>* >& leaves) const {
  for (typename std::vector<ReducerLeaf<T>* >::const_iterator it = leaves.begin(); it != leaves.end(); ++it) {
    if ((*it)->name() == name) return *(*it);
  }
  
  std::cerr << "ERROR in Reducer::GetLeafByName(const TString&, const std::vector<ReducerLeaf<T> >&): Leaf " << name << " not found." << std::endl;
  throw 10;
}
  
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_REDUCER_H
