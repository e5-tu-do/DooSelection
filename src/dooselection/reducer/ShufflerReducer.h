#ifndef SHUFFLERREDUCER_H
#define SHUFFLERREDUCER_H

// from STL
#include <utility>
#include <vector>

// from ROOT
#include "TRandom3.h"

// from project
#include "Reducer.h"

/** @class dooselection::reducer::ShufflerReducer
 *  @brief Derived Reducer to shuffle given leaves into new leaves
 *
 *  This is a Reducer derived from Reducer. It is used if a set of given 
 *  variables needs to be shuffled into another set of variables. For each event
 *  the order of the base variables is shuffled randomly and then stored into
 *  the new target leaves. This can be used to effectively destroy a given order
 *  in the base variables (e.g. having two pion candidates ordered by increasing
 *  PT, these can be saved unordered).
 *
 *  More than one so called shuffler can be defined each carrying multiple sets 
 *  of base and target variables. In each shuffler the same order per event is 
 *  guaranteed. I.e. all base element sets will be shuffled identically.
 *
 *  Example:
 *  You define two shufflers. The first is to shuffle the two variables 
 *  (pi1_PT,pi2_PT) into (out1_PT,out2_PT) and (pi1_IPCHI2,pi2_IPCHI2) into 
 *  (out1_IPCHI2,out2_IPCHI2). The second shuffles (B_s0_v1,B_s0_v2,B_s0_v3) 
 *  into (out1_v1,out1_v2,out1_v3). In each event the first shuffler will 
 *  generate a new random order for both base variable sets so that the 
 *  shuffling of pi1 and pi2 will be the same for both sets. The second shuffler
 *  is independent and will generate an own sequence.
 **/

namespace dooselection {
namespace reducer {
class ShufflerReducer : virtual public Reducer {
  typedef std::vector<ReducerLeaf<Float_t>*> LeafPtrVec;
  typedef std::vector<const ReducerLeaf<Float_t>*> ConstLeafPtrVec;
  typedef std::vector<std::pair<LeafPtrVec,ConstLeafPtrVec> > ShuffleLeafVec;
  
public:
  /**
   *  @brief Register a new shuffler
   *
   *  @return index of this shuffler
   */
  int RegisterShuffler();
  
  /**
   *  @brief Set leaves to shuffle two elements
   *
   *  Add another set of base variables and target variables. The order of the
   *  base variables is shuffled for each event and then written to the target
   *  variables.
   *
   *  @param shuffler_idx index of shuffler to use
   *  @param new_leaf1 target variable 1
   *  @param new_leaf2 target variable 2
   *  @param base_leaf1 base variable 1
   *  @param base_leaf2 base variable 2
   */
  void SetShuffleLeaves(int shuffler_idx, ReducerLeaf<Float_t>* new_leaf1,
                        ReducerLeaf<Float_t>* new_leaf2,
                        const ReducerLeaf<Float_t>* base_leaf1,
                        const ReducerLeaf<Float_t>* base_leaf2);
  /**
   *  @brief Set leaves to shuffle three elements
   *
   *  Add another set of base variables and target variables. The order of the
   *  base variables is shuffled for each event and then written to the target
   *  variables.
   *
   *  @param shuffler_idx index of shuffler to use
   *  @param new_leaf1 target variable 1
   *  @param new_leaf2 target variable 2
   *  @param new_leaf3 target variable 3
   *  @param base_leaf1 base variable 1
   *  @param base_leaf2 base variable 2
   *  @param base_leaf3 base variable 3
   */
  void SetShuffleLeaves(int shuffler_idx, ReducerLeaf<Float_t>* new_leaf1,
                        ReducerLeaf<Float_t>* new_leaf2,
                        ReducerLeaf<Float_t>* new_leaf3,
                        const ReducerLeaf<Float_t>* base_leaf1,
                        const ReducerLeaf<Float_t>* base_leaf2,
                        const ReducerLeaf<Float_t>* base_leaf3);
  
protected:
  virtual void UpdateSpecialLeaves();
  
private:
  /**
   * @brief All shufflers and their appropriate leaf sets.
   */
  std::vector<std::pair<int, ShuffleLeafVec> > shufflers_;
  /**
   * @brief Internal random number generator.
   */
  TRandom3 r_;
};

} // namespace reducer
} // namespace dooselection

#endif // SHUFFLERREDUCER_H

