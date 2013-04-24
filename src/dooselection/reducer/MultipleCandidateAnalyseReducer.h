#ifndef DOOSELECTION_REDUCER_MULTIPLECANDIDATEANALYSEREDUCER_H
#define DOOSELECTION_REDUCER_MULTIPLECANDIDATEANALYSEREDUCER_H

// from STL
#include <map>
#include <vector>

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
   *  @brief Brief description of function.
   *
   *  Detailed description.
   *
   *  @param parameter description
   *  @return description of the return value
   */
  void AddEventIdentifier();
  
 protected:
  virtual void ProcessInputTree();
  
 private:
  /**
   *  @brief Vector of pointers to leaf branch addresses with unique event identifiers
   */
  std::vector<Int_t*> event_identifiers_;
  
  /**
   *  @brief Mapping for unique event identifier and tree entry
   */
  std::multimap<std::vector<Int_t*>, Long64_t> mapping_id_tree_;
};
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_MULTIPLECANDIDATEANALYSEREDUCER_H
