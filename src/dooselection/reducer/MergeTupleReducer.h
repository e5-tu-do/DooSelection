#ifndef DOOSELECTION_REDUCER_MERGETUPLEREDUCER_H
#define DOOSELECTION_REDUCER_MERGETUPLEREDUCER_H

// from STL
#include <list>
#include <utility>

// from ROOT

// from RooFit

// from DooCore

// from project
#include "Reducer.h"

// forward declarations

/** @class dooselection::reducer::MergeTupleReducer
 *  @brief Derived Reducer to merge tuples with different number of entries
 *
 *  This is Reducer will merge tuples with different number of entries into one
 *  tuple. Based on event identifiers only events available in all tuples will 
 *  be kept.
 *
 *  It is assumed that all added friends contain the identical event set and 
 *  that all events in the friends are contained in the input/interim tree.
 *
 **/
namespace dooselection {
namespace reducer {
class MergeTupleReducer : virtual public Reducer {
 public:
  /**
   *  @brief Default constructor
   */
  MergeTupleReducer() {}
  
  /**
   *  @brief Destructor
   */
  virtual ~MergeTupleReducer() {}
  
  /**
   *  @brief Add a leaves to event identifiers
   *
   *  Add a leaf by name as event identifier. Internally, the leaf will be
   *  stored as ReducerLeaf<Long64_t> so type conversions will be handled
   *  automatically.
   *
   *  @param name_tree name of the leaf in the input tree to use
   *  @param name_friend name of the leaf in the first friend tree to use
   */
  void AddEventIdentifier(const std::string& name_tree, const std::string& name_friend);
  
 protected:
  
  /**
   * @brief Process input tree after opening
   *
   * Virtual function for derived classes to process the input tree before it is
   * copied into the interim tree. This function will be called after
   * OpenInputFileAndTree() before leaves are deactivated.
   *
   **/
  virtual void ProcessInputTree();
  
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
  virtual void CreateSpecialBranches();
  
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
  
 private:
  /**
   *  @brief List of events to keep in input tree.
   */
  std::list<std::pair<long long, long long>> event_mapping_;
  
  /**
   *  @brief Map containing all created flat int leaves and according array-based leaves
   */
  ReducerLeaf<Int_t>* leaf_entries_matched_;
  
  /**
   *  @brief Vector of names of leaves with unique event identifiers
   */
  std::vector<std::pair<std::string,std::string>> event_identifier_names_;


};
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_MERGETUPLEREDUCER_H
