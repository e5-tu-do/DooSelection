#ifndef DOOSELECTION_REDUCER_MOMENTUMARRAYFLATTENERREDUCER_H
#define DOOSELECTION_REDUCER_MOMENTUMARRAYFLATTENERREDUCER_H

// from STL
#include <map>
#include <string>

// from ROOT

// from RooFit

// from DooCore

// from project
#include "Reducer.h"

// forward declarations

/** @class dooselection::reducer::MomentumArrayFlattenerReducer
 *  @brief Derived Reducer to write out individual leaves for array-based momentum leaves.
 *
 *  This is a Reducer derived from Reducer. It can be used to add individual
 *  leaves for all dimensions originally comprised in an array-based momentum
 *  leaf. Arrays of length three and four can be flattened. The branch will be
 *  appended with (_E,) _x, _y and _z.
 *
 **/

namespace dooselection {
namespace reducer {
class MomentumArrayFlattenerReducer : virtual public Reducer {
 public:
  /**
   *  @brief Default constructor
   */
  MomentumArrayFlattenerReducer();
  
  /**
   *  @brief Add a momentum leaf of length three having array structure
   *   *
   *  @param threemomentum_array_leaf momentum leaf with array structure of length three
   */
  void AddThreeMomentumArrayLeaf(const ReducerLeaf<Float_t>& threemomentum_array_leaf) { threemomentum_array_leaves_.push_back(&threemomentum_array_leaf); }

  /**
   *  @brief Add a momentum leaf of length four having array structure
   *   *
   *  @param fourmomentum_array_leaf momentum leaf with array structure of length three
   */
  void AddFourMomentumArrayLeaf(const ReducerLeaf<Float_t>& fourmomentum_array_leaf) { fourmomentum_array_leaves_.push_back(&fourmomentum_array_leaf); }
  
 protected:
  virtual bool FlatLeavesPassSpecialCuts();
  virtual void PrepareSpecialBranches();
  virtual void FillOutputTree();
  
 private:  
  /**
   *  @brief Pointers to momentum leaves holding array length of three.
   */
  std::vector<const ReducerLeaf<Float_t>*> threemomentum_array_leaves_;

  /**
   *  @brief Pointers to momentum leaves holding array length of four.
   */
  std::vector<const ReducerLeaf<Float_t>*> fourmomentum_array_leaves_;
  
  /**
   *  @brief Map containing all created flat float leaves and according three-momentum array-based leaves
   */
  std::map<ReducerLeaf<Float_t>*, const ReducerLeaf<Float_t>*> three_leaves_map_float_;
  
  /**
   *  @brief Map containing all created flat double leaves and according three-momentum array-based leaves
   */
  std::map<ReducerLeaf<Double_t>*, const ReducerLeaf<Float_t>*> three_leaves_map_double_;
  
  /**
   *  @brief Map containing all created flat int leaves and according three-momentum array-based leaves
   */
  std::map<ReducerLeaf<Int_t>*, const ReducerLeaf<Float_t>*> three_leaves_map_int_;

  /**
   *  @brief Map containing all created flat float leaves and according four-momentum array-based leaves
   */
  std::map<ReducerLeaf<Float_t>*, const ReducerLeaf<Float_t>*> four_leaves_map_float_;
  
  /**
   *  @brief Map containing all created flat double leaves and according four-momentum array-based leaves
   */
  std::map<ReducerLeaf<Double_t>*, const ReducerLeaf<Float_t>*> four_leaves_map_double_;
  
  /**
   *  @brief Map containing all created flat int leaves and according four-momentum array-based leaves
   */
  std::map<ReducerLeaf<Int_t>*, const ReducerLeaf<Float_t>*> four_leaves_map_int_;

};

} // namespace reducer
} // namespace dooselection


#endif // DOOSELECTION_REDUCER_MOMENTUMARRAYFLATTENERREDUCER_H
