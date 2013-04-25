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
  
 protected:
  virtual void ProcessInputTree();
  
 private:
  /**
   *  @brief Vector of pointers to leaf branch addresses with unique event identifiers
   */
  std::vector<ReducerLeaf<ULong64_t> > event_identifiers_;
  
  /**
   *  @brief Mapping for unique event identifier and tree entry
   */
  std::multimap<std::vector<ULong64_t>, ULong64_t> mapping_id_tree_;
};
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_MULTIPLECANDIDATEANALYSEREDUCER_H
