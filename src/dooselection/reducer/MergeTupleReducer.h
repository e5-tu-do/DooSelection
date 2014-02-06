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
  
  /**
   *  @brief Add a leaves to equalise for non-matching entries
   *
   *  Add leaves by name to equalise. For all events in the input tree that are
   *  not matched between input tree and friends the content of these friend's
   *  leaves is set to the content of the respective input tree leaves.
   *
   *  @param name_tree name of the leaf in the input tree to use
   *  @param name_friend name of the leaf in the first friend tree to use
   */
  void AddEqualiseLeaves(const std::string& name_tree, const std::string& name_friend) {
    names_friend_leaves_equalise_.push_back(std::make_pair(name_tree,name_friend));
  }
  
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
  
  /**
   *  @brief Vector of names of leaves that are to be equalised for non-matching events
   */
  std::vector<std::pair<std::string,std::string>> names_friend_leaves_equalise_;

  /**
   *  @brief Int_t branch addresses to equalise
   */
  std::vector<std::pair<Int_t*,Int_t*>> branch_addresses_equalise_int_;
  
  /**
   *  @brief Float_t branch addresses to equalise
   */
  std::vector<std::pair<Float_t*,Float_t*>> branch_addresses_equalise_float_;
  
  /**
   *  @brief Double_t branch addresses to equalise
   */
  std::vector<std::pair<Double_t*,Double_t*>> branch_addresses_equalise_double_;
  
  /**
   *  @brief UInt_t branch addresses to equalise
   */
  std::vector<std::pair<UInt_t*,UInt_t*>> branch_addresses_equalise_uint_;

  /**
   *  @brief Bool_t branch addresses to equalise
   */
  std::vector<std::pair<Bool_t*,Bool_t*>> branch_addresses_equalise_bool_;

  /**
   *  @brief Long64_t branch addresses to equalise
   */
  std::vector<std::pair<Long64_t*,Long64_t*>> branch_addresses_equalise_long_;

  /**
   *  @brief ULong64_t branch addresses to equalise
   */
  std::vector<std::pair<ULong64_t*,ULong64_t*>> branch_addresses_equalise_ulong_;

  /**
   *  @brief Short_t branch addresses to equalise
   */
  std::vector<std::pair<Short_t*,Short_t*>> branch_addresses_equalise_short_;

  /**
   *  @brief UShort_t branch addresses to equalise
   */
  std::vector<std::pair<UShort_t*,UShort_t*>> branch_addresses_equalise_ushort_;

  /**
   *  @brief Char_t branch addresses to equalise
   */
  std::vector<std::pair<Char_t*,Char_t*>> branch_addresses_equalise_char_;
  
  /**
   *  @brief UChar_t branch addresses to equalise
   */
  std::vector<std::pair<UChar_t*,UChar_t*>> branch_addresses_equalise_uchar_;
};
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_MERGETUPLEREDUCER_H
