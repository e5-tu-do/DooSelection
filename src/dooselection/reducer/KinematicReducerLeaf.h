#ifndef DOOSELECTION_REDUCER_KINEMATICREDUCERLEAF_H
#define DOOSELECTION_REDUCER_KINEMATICREDUCERLEAF_H

// from STL
#include <vector>

// from ROOT

// from DooCore
#include "doocore/physics/kinematic/kinematic.h"

// from project
#include "dooselection/reducer/ReducerLeaf.h"

// forward decalarations

/**
 * @class dooselection::reducer::KinematicReducerLeaf class
 *
 * This helper class represents a leaf in a tree. It provides functionality to
 * do kinematic calculations based on other leaves.
 */

/**
 * @struct dooselection::reducer::KinematicDaughterPropertiesFixedMass class
 *
 * This helper container holds links to momenta leaves for a particle and 
 * additionally the fixed mass of this particle.
 */

namespace dooselection {
namespace reducer {

template <typename T>
struct KinematicDaughterPropertiesFixedMass {
  KinematicDaughterPropertiesFixedMass(ReducerLeaf<T>* leaf_px,
                                       ReducerLeaf<T>* leaf_py,
                                       ReducerLeaf<T>* leaf_pz,
                                       double m)
  : leaf_px_(leaf_px), leaf_py_(leaf_py), leaf_pz_(leaf_pz), m_(m) {}
  
  ReducerLeaf<T>* leaf_px_;
  ReducerLeaf<T>* leaf_py_;
  ReducerLeaf<T>* leaf_pz_;
  double m_;
};
  
template <typename T>
class KinematicReducerLeaf : public ReducerLeaf<T> {
 public:
  KinematicReducerLeaf(TString name, TString title, TString type, TTree* tree, T default_value=T());
  
  virtual ~KinematicReducerLeaf() {}
  
  /** @name Leaf value determination
   *  These functions assure setting the leaves value correctly.
   */
  ///@{
  /**
   * @brief Update the leaf value according to the set dependant leaves and operation.
   *
   * Due to virtuality, higher level ReducerLeaves can implement other
   * operations as well.
   *
   * @return whether the any operation set the value
   */
  virtual bool UpdateValue();
  ///@}
  
  /** @name Kinematic operations
   *  These functions set kinematic operations
   */
  ///@{
  /**
   *  @brief Use two fixed mass daughters to generate a mother mass
   *
   *  Based on momenta leaves and fixed masses of two daughters, a new mass will
   *  be calculated for this leaf, representing a two body decay into these two
   *  particles. This function sets all necessary information.
   *
   *  @param d1_px leaf to daughter 1 px
   *  @param d1_py leaf to daughter 1 py
   *  @param d1_pz leaf to daughter 1 pz
   *  @param d1_m leaf to daughter 1 fixed mass
   *  @param d2_px leaf to daughter 2 px
   *  @param d2_py leaf to daughter 2 py
   *  @param d2_pz leaf to daughter 2 pz
   *  @param d2_m leaf to daughter 2 fixed mass
   */
  void FixedMassDaughtersTwoBodyDecayMotherMass(const ReducerLeaf<T>& d1_px,
                                                const ReducerLeaf<T>& d1_py,
                                                const ReducerLeaf<T>& d1_pz,
                                                double d1_m,
                                                const ReducerLeaf<T>& d2_px,
                                                const ReducerLeaf<T>& d2_py,
                                                const ReducerLeaf<T>& d2_pz,
                                                double d2_m);
  ///@{

 private:
  /**
   *  @brief Vector containing fixed mass daughters for mass hypothesis calculation
   */
  std::vector<KinematicDaughterPropertiesFixedMass<T> > daughters_fixed_mass_;
};

template <class T>
KinematicReducerLeaf<T>::KinematicReducerLeaf(TString name, TString title, TString type, TTree* tree, T default_value)
: ReducerLeaf<T>(name, title, type, tree, default_value)
{
}

template <class T>
bool KinematicReducerLeaf<T>::UpdateValue() {
  using namespace doocore::physics::kinematic;
  
  bool matched = false;
  if (!daughters_fixed_mass_.empty()) {
    daughters_fixed_mass_[0].leaf_px_.UpdateValue();
    daughters_fixed_mass_[0].leaf_py_.UpdateValue();
    daughters_fixed_mass_[0].leaf_pz_.UpdateValue();
    daughters_fixed_mass_[1].leaf_px_.UpdateValue();
    daughters_fixed_mass_[1].leaf_py_.UpdateValue();
    daughters_fixed_mass_[1].leaf_pz_.UpdateValue();
    
    *(this->branch_address_templ_) = MotherTwoBodyDecayMass(
          daughters_fixed_mass_[0].leaf_px_.GetValue(),
          daughters_fixed_mass_[0].leaf_py_.GetValue(),
          daughters_fixed_mass_[0].leaf_pz_.GetValue(),
          daughters_fixed_mass_[0].m_,
          daughters_fixed_mass_[1].leaf_px_.GetValue(),
          daughters_fixed_mass_[1].leaf_py_.GetValue(),
          daughters_fixed_mass_[1].leaf_pz_.GetValue(),
          daughters_fixed_mass_[1].m_);
    matched = true;
  } else {
    *(this->branch_address_templ_) = this->default_value_;
  }
  return matched;
}

template <class T>
void KinematicReducerLeaf<T>::FixedMassDaughtersTwoBodyDecayMotherMass(const ReducerLeaf<T>& d1_px,
                                                const ReducerLeaf<T>& d1_py,
                                                const ReducerLeaf<T>& d1_pz,
                                                double d1_m,
                                                const ReducerLeaf<T>& d2_px,
                                                const ReducerLeaf<T>& d2_py,
                                                const ReducerLeaf<T>& d2_pz,
                                                double d2_m) {
  daughters_fixed_mass_.clear();
  daughters_fixed_mass_.push_back(KinematicDaughterPropertiesFixedMass<T>(d1_px, d1_py, d1_pz, d1_m));
  daughters_fixed_mass_.push_back(KinematicDaughterPropertiesFixedMass<T>(d2_px, d2_py, d2_pz, d2_m));
}
  
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_KINEMATICREDUCERLEAF_H
