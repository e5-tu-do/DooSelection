#ifndef DOOSELECTION_REDUCER_MULTIPLECANDIDATEANALYSEREDUCER_H
#define DOOSELECTION_REDUCER_MULTIPLECANDIDATEANALYSEREDUCER_H

// from STL
#include <map>
#include <vector>
#include <string>

// from ROOT

// from RooFit

// from DooCore

// from project
#include "Reducer.h"

// forward declarations

/** @class dooselection::reducer::MultipleCandidateAnalyseReducer
 *  @brief Derived Reducer to analyse multiple candidates in an input tuple.
 *
 *  This is a Reducer derived from Reducer. It can analyse the amount of 
 *  multiple candidates in the input tuple. This is accomplished by counting
 *  the number of events with identical event identifiers. Event identifiers
 *  are a list of int-based leaves (e.g. event and run number) that uniquely
 *  identify the event.
 *
 **/
namespace dooselection {
namespace reducer {
class MultipleCandidateAnalyseReducer : virtual public Reducer {
 public:
  /**
   *  @brief Default constructor
   */
  MultipleCandidateAnalyseReducer();
  
  /**
   *  @brief Add a leaf to event identifiers
   *
   *  Add a leaf by name as event identifier. Internally, the leaf will be 
   *  stored as ReducerLeaf<Long64_t> so type conversions will be handled 
   *  automatically.
   *
   *  @param name_leaf name of the leaf in the input tree to use
   */
  void AddEventIdentifier(const std::string& name_leaf);
  
  /**
   *  @brief Set check for sequential identifiers
   *
   *  By activating this option (default: true), the 
   *  MultipleCandidateAnalyseReducer will check whether all multiple candidates
   *  identified by a unique identifier are sequential (i.e. all entries of the 
   *  same event follow after each other). If an event is found whose identifier 
   *  has also been found before (and another event lies in between), a warning 
   *  will be printed.
   *
   *  @param check_sequential_identifiers whether to check for sequential identifiers
   */
  void set_check_sequential_identifiers(bool check_sequential_identifiers) { check_sequential_identifiers_ = check_sequential_identifiers; }
  
 protected:
  virtual void ProcessInputTree();
  bool set_do_multi_cand_analysis(bool status){do_multi_cand_analysis_ = status;}

 private:
  /**
   *  @brief Bool to decide if the analysis runs or not
   *  Use case: You write an inherited reducer that also does other things
   *  and you want to specify if the analysis should run or not depending on whatever...
   */
  bool do_multi_cand_analysis_;
  
  /**
   *  @brief Check for sequential identifiers.
   */
  bool check_sequential_identifiers_;

  /**
   *  @brief Vector of names of leaves with unique event identifiers
   */
  std::vector<std::string> event_identifier_names_;
  
  /**
   *  @brief Mapping for unique event identifier and tree entry
   */
  std::multimap<std::vector<ULong64_t>, ULong64_t> mapping_id_tree_;  
};
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_MULTIPLECANDIDATEANALYSEREDUCER_H
