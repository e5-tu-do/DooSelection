#ifndef DOOSELECTION_REDUCER_REDUCER_H
#define DOOSELECTION_REDUCER_REDUCER_H

// from STL
#include <set>
#include <string>
#include <vector>
#include <map>
#include <iostream>

// from BOOST
#include <boost/bimap.hpp>

// from ROOT
#include "TString.h"
#include "TTree.h"
#include "TTreeFormula.h"

#include "ReducerLeaf.h"

// forward declarations
class TFile;
class TLeaf;
class TTreeFormula;
class RooArgSet;

/**
 * @namespace dooselection::reducer
 * @brief Reducer namespace
 *
 * This namespace contains all Reducer functionality. Reducer is a framework
 * to process and modify tuples. This includes applying selections, adding new
 * leaves and perform higher level modifications. The basic 
 * dooselection::reducer::Reducer provides standard functionality such as 
 * applying cut strings adding new leaves based on existing leaves and apply a
 * best candidate selection.
 *
 * Higher functionality is provided by derived Reducer classes which can be 
 * combined due to virtual inheritance. To develop own derived Reducers virtual
 * functions exist as hooks to interface the Reducer mechanisms.
 *
 * In principle the Reducer always works like this: An input tuple is opened and
 * an optional cut string is applied after which the input tree is copied into 
 * an interim tree. In the following step an event loop processes the interim 
 * tree, applies further cuts, calculates new leaves. Events passing the 
 * requirements are processed by an optional best candidate selection and if 
 * selected written into the output tree. 
 *
 * @section reducer_usage Usage
 *
 * Consider this example for basic usage info:
 *
 * @code
 * Reducer my_reducer;
 *
 * // set input/output files to process
 * my_reducer.set_input_file_path(input_file);
 * my_reducer.set_input_tree_path(input_tree);
 * my_reducer.set_output_file_path(output_file);
 * my_reducer.set_output_tree_path(output_tree);
 *
 * // set a cut string for the first step from input to interim tree
 * my_reducer.set_cut_string("piminus_TRACK_Type==3 && piplus_TRACK_Type==3");
 *
 * // step 1: Initialize. Open the input tree, set branch status, create interim 
 * //         tree by copying input tree with cut, open output tree and 
 * //         initialize higher level leaves.
 * my_reducer.Initialize();
 * 
 * // create custom branches via built-in functions here
 * my_reducer.CreateDoubleCopyLeaf(...);
 *
 * // step 2: Run. Prepare more higher level leaves, running event loop with 
 * //         best candidate selection and writing events into output tree.
 * my_reducer.Run();
 *
 * // step 3: Finalize. Reserved for future usage.
 * my_reducer.Finalize();
 * @endcode
 *
 * @section reducer_hooks Hooks for higher level Reducers
 *
 * Reducer supports a set of virtual functions for derived higher level Reducers
 * to implement further functionality. The workflow is as this:
 *
 * @code
 * Initialize();
 * \- open input tree
 *  - dooselection::reducer::Reducer::ProcessInputTree();
 *    \- virtual function to work on the input tree before any branches are 
 *     - deactivated or cuts applied.
 *  - copy input tree into interim tree with cut
 *  - create (empty) output tree
 *  - dooselection::reducer::Reducer::CreateSpecialBranches();
 *    \- virtual function to create/prepare leaves that are to be written in
 *     - the event loop. These can be leaves based on integrated support or 
 *     - higher level leaves that need to be evaluated for each event by special
 *     - functions.
 *
 * Run();
 * \- dooselection::reducer::Reducer::PrepareSpecialBranches();
 *    \- virtual function to create/prepare leaves that are to be written in
 *     - the event loop. Similar to CreateSpecialBranches() but this function 
 *     - is assured to run after the user has created their own leaves and 
 *     - directly before the event loop.
 *  - run event loop:
 *  \- select one event
 *   - update all leaf values, including new leaves to be created
 *   - dooselection::reducer::Reducer::UpdateSpecialLeaves();
 *   \- virtual function to calculate values of higher level leaves like 
 *    - complicated vetos and such.
 *   - dooselection::reducer::Reducer::LoadTreeFriendsEntryHook(long long);
 *   \- virtual function to load tree friend entries.
 *   - dooselection::reducer::Reducer::EntryPassesSpecialCuts();
 *   \- virtual function to check if leaves fulfil higher level cuts like 
 *    - complicated vetos or any other requirement.
 *   - if passing, get best candidate of passing events.
 *   - dooselection::reducer::Reducer::FillOutputTree();
 *   \- virtual function to write a passing and best candidate selected event 
 *    - into the output tree.
 *    - Must call dooselection::reducer::Reducer::FlushEvent() at least once to
 *    - actually write the event at least once. Last chance to modify any leaf
 *    - values.
 *   - close output tree and delete interim file
 * @endcode
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
  
  //void ReadConfigFile();
  //void WriteConfigFile() const;
  
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
  
  /** @name Input tree processing
   *  Functions to control processing of input tree
   */
  ///@{
  /**
   *  @brief Set number of events in input tree to process
   *
   *  Use this to specify how many events in the input tree are to be copied 
   *  into the interim and output tree. If set to -1 (default) all events will
   *  be processed.
   *
   *  @param num_events_process number of events to process
   */
  void set_num_events_process(int num_events_process) { num_events_process_ = num_events_process; }
  ///@}
  
  /** @name Branch keeping/omitting
   *  Functions to control which branches to keep/omit
   */
  ///@{
  void add_branch_keep(TString const&);
  void add_branch_omit(TString const&);
  void add_branches_keep(std::set<TString> const&);
  void add_branches_omit(std::set<TString> const&);
  
  /**
   *  @brief Add regular expression of branches to keep
   *
   *  @param keep_regex regular expression for branches to keep
   */
  void AddBranchesKeepRegex(std::string keep_regex) { branches_keep_regex_.push_back(keep_regex); }
  
  /**
   *  @brief Add branches to keep by RooArgSet
   *
   *  @param argset argset with all observables to keep
   */
  void AddBranchesKeep(const RooArgSet& argset);
  
  /**
   *  @brief Add regular expression of branches to omit
   *
   *  @param omit_regex regular expression for branches to omit
   */
  void AddBranchesOmitRegex(std::string omit_regex) { branches_omit_regex_.push_back(omit_regex); }
  ///@}
  
  /** @name Cut string functions
   *  Functions for setting/extending the cut string
   */
  ///@{
  void set_cut_string(TString const&);
  void add_cut_string(TString const&);
  TString const& cut_string() const;
  ///@}
  
  /** @name Rename branches functions
   *  Functions to rename branches either by passing old and new name or boost::bimap containing unique pairs of old and new names
   */
  ///@{
  void AddNameMapping(TString old_name, TString new_name) {
    name_mapping_.insert(boost::bimap<TString, TString>::value_type(old_name, new_name));
  }
  void AddNameMapping(boost::bimap<TString, TString> bm) {
    name_mapping_=bm;
  }
  ///@}
  
  
  /** @name Best candidate selection configuration
   *  Functions controlling best candidate selection
   */
  ///@{
  template<class T>
  void SetEventNumberLeaf(const ReducerLeaf<T>& leaf) {
    event_number_leaf_ptr_ = new ReducerLeaf<ULong64_t>(leaf.name(), leaf.title(), leaf.type(), leaf.tree());
    event_number_leaf_ptr_->set_branch_address(leaf.branch_address());
    input_tree_->SetBranchStatus(event_number_leaf_ptr_->name(),1);
  }
  template<class T>
  void SetRunNumberLeaf(const ReducerLeaf<T>& leaf) {
    run_number_leaf_ptr_ = new ReducerLeaf<ULong64_t>(leaf.name(), leaf.title(), leaf.type(), leaf.tree());
    run_number_leaf_ptr_->set_branch_address(leaf.branch_address());
    input_tree_->SetBranchStatus(run_number_leaf_ptr_->name(),1);
  }
  template<class T>
  void SetBestCandidateLeaf(const ReducerLeaf<T>& leaf) {
    best_candidate_leaf_ptr_ = new ReducerLeaf<Double_t>(leaf.name(), leaf.title(), leaf.type(), leaf.tree());
    best_candidate_leaf_ptr_->set_branch_address(leaf.branch_address());
    input_tree_->SetBranchStatus(best_candidate_leaf_ptr_->name(),1);
  }
  ///@}

  /** @name Accessing leaves
   *  Functions accessing existing or to-be-created leaves
   */
  ///@{
  /**
   *  @brief Access a leaf in the interim tree.
   *
   *  This fuction is not const as it allows to also access leaves in the input 
   *  tree that were previously deactivated. These will be reactivated in the 
   *  process.
   *
   *  @param name name of the leaf.
   *  @return the leaf as ReducerLeaf
   */
  const ReducerLeaf<Float_t>& GetInterimLeafByName(const TString& name);
  
  const ReducerLeaf<Float_t>& GetInterimLeafByName(const TString& name) const {
    return GetLeafByName<Float_t>(name, interim_leaves_);
  }
  
  const ReducerLeaf<Double_t>& GetDoubleLeafByName(const TString& name) const {
    return GetLeafByName<Double_t>(name, double_leaves_);
  }
  const ReducerLeaf<Float_t>& GetFloatLeafByName(const TString& name) const {
    return GetLeafByName<Float_t>(name, float_leaves_);
  }
  const ReducerLeaf<ULong64_t>& GetULongLeafByName(const TString& name) const {
    return GetLeafByName<ULong64_t>(name, ulong_leaves_);
  }
  const ReducerLeaf<Long64_t>& GetLongLeafByName(const TString& name) const {
    return GetLeafByName<Long64_t>(name, long_leaves_);
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
    for (std::vector<ReducerLeaf<ULong64_t>* >::const_iterator it = ulong_leaves_.begin(); it != ulong_leaves_.end(); ++it) {
      if ((*it)->name() == name) return true;
    }
    for (std::vector<ReducerLeaf<Long64_t>* >::const_iterator it = long_leaves_.begin(); it != long_leaves_.end(); ++it) {
      if ((*it)->name() == name) return true;
    }
    return false;
  }
  
  /**
   *  @brief Interim leaves begin iterator
   *
   *  To iterate over all interim leaves, this function will return the begin()
   *  iterator of the corresponding vector.
   *
   *  @return begin() of interim leaves vector
   */
  std::vector<ReducerLeaf<Float_t>* >::const_iterator GetInterimLeavesBegin() const {
    return interim_leaves_.begin();
  }
  
  /**
   *  @brief Interim leaves end iterator
   *
   *  To iterate over all interim leaves, this function will return the end()
   *  iterator of the corresponding vector.
   *
   *  @return end() of interim leaves vector
   */
  std::vector<ReducerLeaf<Float_t>* >::const_iterator GetInterimLeavesEnd() const {
    return interim_leaves_.end();
  }
  
  /**
   *  @brief Double leaves begin iterator
   *
   *  To iterate over all double leaves, this function will return the begin()
   *  iterator of the corresponding vector.
   *
   *  @return begin() of double leaves vector
   */
  std::vector<ReducerLeaf<Double_t>* >::const_iterator GetDoubleLeavesBegin() const {
    return double_leaves_.begin();
  }
  
  /**
   *  @brief Double leaves end iterator
   *
   *  To iterate over all double leaves, this function will return the end()
   *  iterator of the corresponding vector.
   *
   *  @return end() of double leaves vector
   */
  std::vector<ReducerLeaf<Double_t>* >::const_iterator GetDoubleLeavesEnd() const {
    return double_leaves_.end();
  }

  /**
   *  @brief Float leaves begin iterator
   *
   *  To iterate over all float leaves, this function will return the begin()
   *  iterator of the corresponding vector.
   *
   *  @return begin() of float leaves vector
   */
  std::vector<ReducerLeaf<Float_t>* >::const_iterator GetFloatLeavesBegin() const {
    return float_leaves_.begin();
  }
 
  /**
   *  @brief Float leaves end iterator
   *
   *  To iterate over all float leaves, this function will return the end()
   *  iterator of the corresponding vector.
   *
   *  @return end() of float leaves vector
   */
  std::vector<ReducerLeaf<Float_t>* >::const_iterator GetFloatLeavesEnd() const {
    return float_leaves_.end();
  }
  
  /**
   *  @brief Unsigned long leaves begin iterator
   *
   *  To iterate over all unsigned long leaves, this function will return the begin()
   *  iterator of the corresponding vector.
   *
   *  @return begin() of ulong leaves vector
   */
  std::vector<ReducerLeaf<ULong64_t>* >::const_iterator GetULongLeavesBegin() const {
    return ulong_leaves_.begin();
  }
  
  /**
   *  @brief Unsigned long leaves end iterator
   *
   *  To iterate over all unsigned long leaves, this function will return the end()
   *  iterator of the corresponding vector.
   *
   *  @return end() of ulong leaves vector
   */
  std::vector<ReducerLeaf<ULong64_t>* >::const_iterator GetULongLeavesEnd() const {
    return ulong_leaves_.end();
  }
  
  /**
   *  @brief Long leaves begin iterator
   *
   *  To iterate over all long leaves, this function will return the begin()
   *  iterator of the corresponding vector.
   *
   *  @return begin() of long leaves vector
   */
  std::vector<ReducerLeaf<Long64_t>* >::const_iterator GetLongLeavesBegin() const {
    return long_leaves_.begin();
  }
  
  /**
   *  @brief Long leaves end iterator
   *
   *  To iterate over all long leaves, this function will return the end()
   *  iterator of the corresponding vector.
   *
   *  @return end() of long leaves vector
   */
  std::vector<ReducerLeaf<Long64_t>* >::const_iterator GetLongLeavesEnd() const {
    return long_leaves_.end();
  }

  /**
   *  @brief Int leaves begin iterator
   *
   *  To iterate over all int leaves, this function will return the begin()
   *  iterator of the corresponding vector.
   *
   *  @return begin() of int leaves vector
   */
  std::vector<ReducerLeaf<Int_t>* >::const_iterator GetIntLeavesBegin() const {
    return int_leaves_.begin();
  }
  
  /**
   *  @brief Int leaves end iterator
   *
   *  To iterate over all int leaves, this function will return the end()
   *  iterator of the corresponding vector.
   *
   *  @return end() of int leaves vector
   */
  std::vector<ReducerLeaf<Int_t>* >::const_iterator GetIntLeavesEnd() const {
    return int_leaves_.end();
  }
  ///@}
  
  /**
   *  @brief Add additional tree friends to input tree
   *
   *  @param file_name File to open the tree friend from
   *  @param tree_name Tree to open in file
   */
  void AddTreeFriend(std::string file_name, std::string tree_name);
  
  /** @name Leaf creation
   *  Functions creating new leaves in the output tree
   */
  ///@{
  ReducerLeaf<Double_t>& CreateDoubleLeaf(TString name, TString title, TString type, Double_t default_value=0.0) {
    ReducerLeaf<Double_t>* new_leaf(new ReducerLeaf<Double_t>(name, title, type, interim_tree_, default_value));
    double_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  ReducerLeaf<Double_t>& CreateDoubleLeaf(TString name, Double_t default_value=0.0) {
    ReducerLeaf<Double_t>* new_leaf(new ReducerLeaf<Double_t>(name, name, "Double_t", interim_tree_, default_value));
    double_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  template<class T>
  ReducerLeaf<Double_t>& CreateDoubleCopyLeaf(TString name, const ReducerLeaf<T>& leaf_to_copy, double c=1.0) {
    ReducerLeaf<Double_t>& new_leaf(CreateDoubleLeaf(name, name, "Double_t"));
    new_leaf.Equal(leaf_to_copy, c);
    return new_leaf;
  }
  
  ReducerLeaf<Float_t>& CreateFloatLeaf(TString name, TString title, TString type, Float_t default_value=0.0) {
    ReducerLeaf<Float_t>* new_leaf(new ReducerLeaf<Float_t>(name, title, type, interim_tree_, default_value));
    float_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  ReducerLeaf<Float_t>& CreateFloatLeaf(TString name, Float_t default_value=0.0) {    
    ReducerLeaf<Float_t>* new_leaf(new ReducerLeaf<Float_t>(name, name, "Float_t", interim_tree_, default_value));
    float_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  template<class T>
  ReducerLeaf<Float_t>& CreateFloatCopyLeaf(TString name, const ReducerLeaf<T>& leaf_to_copy, double c=1.0) {
    ReducerLeaf<Float_t>& new_leaf(CreateFloatLeaf(name, name, "Float_t");
    new_leaf.Equal(leaf_to_copy, c));
    return new_leaf;
  }
  
  ReducerLeaf<ULong64_t>& CreateULongLeaf(TString name, TString title, TString type, ULong64_t default_value=0) {
    ReducerLeaf<ULong64_t>* new_leaf(new ReducerLeaf<ULong64_t>(name, title, type, interim_tree_, default_value));
    ulong_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  ReducerLeaf<ULong64_t>& CreateULongLeaf(TString name, Float_t default_value=0.0) {
    ReducerLeaf<ULong64_t>* new_leaf(new ReducerLeaf<ULong64_t>(name, name, "ULong64_t", interim_tree_, default_value));
    ulong_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  template<class T>
  ReducerLeaf<ULong64_t>& CreateULongCopyLeaf(TString name, const ReducerLeaf<T>& leaf_to_copy, double c=1.0) {
    ReducerLeaf<ULong64_t>& new_leaf(CreateULongLeaf(name, name, "ULong64_t");
    new_leaf.Equal(leaf_to_copy, c));
    return new_leaf;
  }
  
  ReducerLeaf<Long64_t>& CreateLongLeaf(TString name, TString title, TString type, ULong64_t default_value=0) {
    ReducerLeaf<Long64_t>* new_leaf(new ReducerLeaf<Long64_t>(name, title, type, interim_tree_, default_value));
    long_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  ReducerLeaf<Long64_t>& CreateLongLeaf(TString name, Float_t default_value=0.0) {
    ReducerLeaf<Long64_t>* new_leaf(new ReducerLeaf<Long64_t>(name, name, "Long64_t", interim_tree_, default_value));
    long_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  template<class T>
  ReducerLeaf<Long64_t>& CreateLongCopyLeaf(TString name, const ReducerLeaf<T>& leaf_to_copy, double c=1.0) {
    ReducerLeaf<Long64_t>& new_leaf(CreateLongLeaf(name, name, "Long64_t");
    new_leaf.Equal(leaf_to_copy, c));
    return new_leaf;
  }
  
  ReducerLeaf<Int_t>& CreateIntLeaf(TString name, TString title, TString type, Int_t default_value=0) {
    ReducerLeaf<Int_t>* new_leaf(new ReducerLeaf<Int_t>(name, title, type, interim_tree_, default_value));
    int_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  ReducerLeaf<Int_t>& CreateIntLeaf(TString name, Int_t default_value=0) {
    ReducerLeaf<Int_t>* new_leaf(new ReducerLeaf<Int_t>(name, name, "Int_t", interim_tree_, default_value));
    int_leaves_.push_back(new_leaf);
    return *new_leaf;
  }
  template<class T>
  ReducerLeaf<Int_t>& CreateIntCopyLeaf(TString name, const ReducerLeaf<T>& leaf_to_copy, double c=1.0) {
    ReducerLeaf<Int_t>& new_leaf(CreateIntLeaf(name, name, "Int_t");
    new_leaf.Equal(leaf_to_copy, c));
    return new_leaf;
  }
  
  /**
   *  @brief Register externally created double leaf
   *
   *  An externally created double leaf will be registered internally in this 
   *  Reducer for output writing. The Reducer assumes to take over ownership of
   *  this leaf.
   *
   *  @param leaf the leaf to be registered.
   */
  void RegisterDoubleLeaf(ReducerLeaf<Double_t>* leaf) {
    double_leaves_.push_back(leaf);
  }
  
  /**
   *  @brief Register externally created float leaf
   *
   *  An externally created float leaf will be registered internally in this
   *  Reducer for output writing. The Reducer assumes to take over ownership of
   *  this leaf.
   *
   *  @param leaf the leaf to be registered.
   */
  void RegisterFloatLeaf(ReducerLeaf<Float_t>* leaf) {
    float_leaves_.push_back(leaf);
  }
  
  /**
   *  @brief Register externally created unsigned long leaf
   *
   *  An externally created unsigned long leaf will be registered internally in this
   *  Reducer for output writing. The Reducer assumes to take over ownership of
   *  this leaf.
   *
   *  @param leaf the leaf to be registered.
   */
  void RegisterULongLeaf(ReducerLeaf<ULong64_t>* leaf) {
    ulong_leaves_.push_back(leaf);
  }
  
  /**
   *  @brief Register externally created long leaf
   *
   *  An externally created long leaf will be registered internally in this
   *  Reducer for output writing. The Reducer assumes to take over ownership of
   *  this leaf.
   *
   *  @param leaf the leaf to be registered.
   */
  void RegisterLongLeaf(ReducerLeaf<Long64_t>* leaf) {
    long_leaves_.push_back(leaf);
  }
  
  /**
   *  @brief Register externally created int leaf
   *
   *  An externally created int leaf will be registered internally in this
   *  Reducer for output writing. The Reducer assumes to take over ownership of
   *  this leaf.
   *
   *  @param leaf the leaf to be registered.
   */
  void RegisterIntLeaf(ReducerLeaf<Int_t>* leaf) {
    int_leaves_.push_back(leaf);
  }
  ///@}
  
 protected:
  /**
   * @brief Process input tree after opening
   *
   * Virtual function for derived classes to process the input tree before it is
   * copied into the interim tree. This function will be called after 
   * OpenInputFileAndTree() before leaves are deactivated.
   *
   **/
  virtual void ProcessInputTree() {}
  
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
 
  /**
   * Virtual function for derived classes with higher level leaves. This will be 
   * called to compute values of these leaves upon loading of an event from the 
   * interim tree.
   **/
  virtual void UpdateSpecialLeaves() {}
  
  /**
   * Virtual function for derived classes with higher level cuts. This will be 
   * called to check if an event/candidate passes certain requirements. Events
   * failing this will not be considered at all. Events passing will be 
   * considered for the best candidate selection (if this is used).
   **/
  virtual bool EntryPassesSpecialCuts() { return true; }
  
  /**
   *  @brief Fill the output tree
   *
   *  Fill events into the output tree via this virtual function. It is called 
   *  after best candidate selection (if applicable). In the default 
   *  implementation it will just call FlushEvent() to write the current event 
   *  into the output tree. Derived classes can use this to do last minute event
   *  manipulation or to not write events at all. Also it can be used to write 
   *  one event multiple times (modifying specific leaves in each write, e.g. to
   *  flatten array-based leaves).
   *
   *  For every event flush/fill, FlushEvent() must be called.
   */
  virtual void FillOutputTree();
  
  /**
   *  @brief Hook for loading of friend entries
   *
   *  For each new interim tree entry that is loaded, this function is called 
   *  with the interim tree entry number. In Reducer this will simply load the
   *  appropriate entries in all tree friends as well. 
   *
   *  Derived Reducers like TupleMergeReducer can overwrite this function.
   *
   *  @warning In case the tree friend does not contain enough entries, the last 
   *           event in the tree friends will be loaded instead. This will 
   *           possibly lead to undesired behaviour.
   *
   *  @param entry The entry of the interim tree that is loaded.
   */
  virtual void LoadTreeFriendsEntryHook(long long entry);
  
  /**
   *  @brief Fill/flush the current event into the output tree
   *
   *  This function is responsible for filling the current event (i.e. all 
   *  leaves) into the output tree.
   */
  void FlushEvent();

  /**
   *  @brief Setting correct branch status (keep/omit) for input tree branches
   *
   *  This function is responsible for setting the branch status in all leaves
   *  that are either to keep or omit.
   */
  void InitializeBranches();
  
  /**
   *  @brief Force usage of old-style interim tree
   *
   *  By using this switch, derived Reducers can force this Reducer to use the 
   *  old way of creating an interim tree by copying the input tree with a cut, 
   *  rather than just using the input tree directly and applying cuts in the
   *  event loop.
   *
   *  @param old_style_interim_tree whether to use old-style copied interim tree
   */
  void set_old_style_interim_tree(bool old_style_interim_tree) {old_style_interim_tree_ = old_style_interim_tree;}
  
	/**
	 * Interim tree protected to give derived classed possibility to work with it.
	 */
  TTree* interim_tree_;
  
  /**
	 * Input tree protected to give derived classed possibility to work with it.
	 */
  TTree* input_tree_;
  
  /**
   *  @brief Additional tree friends to add to the input tree
   */
  std::vector<TTree*> additional_input_tree_friends_;
  
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
  
  unsigned int GetBestCandidate(TTree* tree, unsigned int pos_event_start, unsigned int num_entries);
  
  /**
   * Get all leaves from a tree and store them into the vector of ReducerLeafs
   *
   * (unfortunately tree cannot be a const TTree*)
   *
   */
  void InitializeInterimLeafMap(TTree* tree, std::vector<ReducerLeaf<Float_t>* >* leaves);  
  
  /**
   *  @brief Check if specific interim tree creation is necessary
   *
   *  This function will return whether a unique interim tree needs to be 
   *  created by copying input tree into a new tree. By default, Reducer will 
   *  try to avoid this time-consuming step and use the input tree directly.
   *  The copying will only be performed if set_old_style_interim_tree() is 
   *  used.
   *
   *  @return whether the interim tree is to be created or not
   */
  bool CreateUniqueInterimTree() const {
    return !(!old_style_interim_tree_ || (old_style_interim_tree_ && num_events_process_ == -1 && cut_string_.Length() == 0));
  }
  
  /**
   * Rename branches based on a given name mapping vector
   *
   */
  void RenameBranches(std::vector<ReducerLeaf<Float_t>* >* leaves, const boost::bimap<TString, TString>& mapping); 
  
  /**
   * Initialize branches of a given output tree based on vector of ReducerLeaves
   *
   */
  template<class T>
  void InitializeOutputBranches(TTree* tree, const std::vector<ReducerLeaf<T>* >& leaves);
  
  /**
   *  @brief Purge new leaves already existing in interim tree
   *
   *  All leaves that already exist in the interim tree, but are also to be 
   *  created will be purged by this function from the list of leaves 
   *  supplied.
   *
   *  @param leaves vector of leaves to purge
   *  @param interim_leaves vector of leaves to compare against
   *  @return new purged vector
   */
  template<class T1,class T2>
  std::vector<ReducerLeaf<T1>*> PurgeOutputBranches(const std::vector<ReducerLeaf<T1>* >& leaves, const std::vector<ReducerLeaf<T2>* >& interim_leaves) const;
  
  /**
   *  @brief Activate all dependent leaves of given leaf list
   *
   *  @param leaves list of leaves to check
   */
  template<class T>
  void ActivateDependentLeaves(const std::vector<ReducerLeaf<T>* >& leaves) {
    using namespace doocore::io;
    for (auto leaf : leaves) {
//      sdebug << "leaf: " << leaf << endmsg;
//      sdebug << "leaf->leaf_pointer_one(): " << leaf->leaf_pointer_one() << endmsg;
//      sdebug << "leaf->leaf_pointer_one()->name(): " << leaf->leaf_pointer_one()->name() << endmsg;
      if (leaf->leaf_pointer_one() != NULL) {
        if (input_tree_->GetLeaf(leaf->leaf_pointer_one()->name()) != NULL) {
          //sdebug << "Reactivating " << leaf->leaf_pointer_one()->name() << " (needed for operation in " << leaf->name() << ")" << endmsg;
          input_tree_->SetBranchStatus(leaf->leaf_pointer_one()->name(), 1);
        }
      }
      if (leaf->leaf_pointer_two() != NULL) {
        if (input_tree_->GetLeaf(leaf->leaf_pointer_two()->name()) != NULL) {
          //sdebug << "Reactivating " << leaf->leaf_pointer_two()->name() << " (needed for operation in " << leaf->name() << ")" << endmsg;
          input_tree_->SetBranchStatus(leaf->leaf_pointer_two()->name(), 1);
        }
      }
      
      leaf->ActivateDependentConditionLeaves(input_tree_);
    }
  }
  
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
    
    LoadTreeFriendsEntryHook(i);
    
    UpdateSpecialLeaves();
    UpdateAllValues<Float_t>(float_leaves_);
    UpdateAllValues<Double_t>(double_leaves_);
    UpdateAllValues<Int_t>(int_leaves_);
    UpdateAllValues<ULong64_t>(ulong_leaves_);
    UpdateAllValues<Long64_t>(long_leaves_);
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
  
  /**
   *  @brief Regular expressions with branches to keep
   */
  std::vector<std::string> branches_keep_regex_;
  
  /**
   *  @brief Regular expressions with branches to omit
   */
  std::vector<std::string> branches_omit_regex_;
  
  boost::bimap<TString, TString> name_mapping_;
  
  std::vector<ReducerLeaf<Float_t>* >  interim_leaves_; ///< vector of all leaves in the interim tree
  std::vector<ReducerLeaf<Float_t>* >  float_leaves_;   ///< new float leaves for output tree

  /**
   *  @brief Container of newly created unsigned long leaves.
   */
  std::vector<ReducerLeaf<ULong64_t>* > ulong_leaves_;

  /**
   *  @brief Container of newly created signed long leaves.
   */
  std::vector<ReducerLeaf<Long64_t>* > long_leaves_;
  
  /**
   *  @brief Container of newly created double leaves.
   */
  std::vector<ReducerLeaf<Double_t>* > double_leaves_;
  
  std::vector<ReducerLeaf<Int_t>* >    int_leaves_;     ///< new int leaves for output tree
  
  TFile* input_file_;
    
  TFile* output_file_;
  TTree* output_tree_;
  
  TFile* interim_file_;
  
  /**
   *  @brief Formula for applying the tree cut
   */
  TTreeFormula* formula_input_tree_;
  
  /**
   * members needed for best candidate selection
   *
   */
  ReducerLeaf<Double_t> * best_candidate_leaf_ptr_;
  
  static bool abort_loop_;
  
  ///< SIGINT handler (i.e. CTRL-C)
  static void HandleSigInt(int param);
  
  /**
   *  @brief maximum number of events to process
   */
  int num_events_process_;

  /**
   *  @brief number of written events in output tree
   */
  unsigned int num_written_;
  
  /**
   *  @brief Status bit to request old-style interim tree processing
   */
  bool old_style_interim_tree_;
};

template<class T>
const ReducerLeaf<T>& Reducer::GetLeafByName(const TString& name, const std::vector<ReducerLeaf<T>* >& leaves) const {
  for (typename std::vector<ReducerLeaf<T>* >::const_iterator it = leaves.begin(); it != leaves.end(); ++it) {
    if ((*it)->name() == name) return *(*it);
  }
  
  //doocore::io::serr << "ERROR in Reducer::GetLeafByName(const TString&, const std::vector<ReducerLeaf<T> >&): Leaf " << name << " not found." << doocore::io::endmsg;
  throw 10;
}
  
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_REDUCER_H
