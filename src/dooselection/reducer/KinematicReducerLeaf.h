#ifndef DOOSELECTION_REDUCER_KINEMATICREDUCERLEAF_H
#define DOOSELECTION_REDUCER_KINEMATICREDUCERLEAF_H

// from STL
#include <vector>

// from ROOT

// from DooCore
#include "doocore/physics/kinematic/kinematic.h"
#include "doocore/io/MsgStream.h"

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
  
  virtual ~KinematicReducerLeaf() {
    EmptyDependantVectors();
  }
  
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
  template<class T1, class T2, class T3, class T4, class T5, class T6>
  void FixedMassDaughtersTwoBodyDecayMotherMass(const ReducerLeaf<T1>& d1_px,
                                                const ReducerLeaf<T2>& d1_py,
                                                const ReducerLeaf<T3>& d1_pz,
                                                double d1_m,
                                                const ReducerLeaf<T4>& d2_px,
                                                const ReducerLeaf<T5>& d2_py,
                                                const ReducerLeaf<T6>& d2_pz,
                                                double d2_m);
  ///@{

 private:
  /**
   *  @brief Empty vectors conatining links to dependant leaves
   */
  void EmptyDependantVectors();
  
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
    daughters_fixed_mass_[0].leaf_px_->UpdateValue();
    daughters_fixed_mass_[0].leaf_py_->UpdateValue();
    daughters_fixed_mass_[0].leaf_pz_->UpdateValue();
    daughters_fixed_mass_[1].leaf_px_->UpdateValue();
    daughters_fixed_mass_[1].leaf_py_->UpdateValue();
    daughters_fixed_mass_[1].leaf_pz_->UpdateValue();
    
    *(this->branch_address_templ_) = MotherTwoBodyDecayMass(
          daughters_fixed_mass_[0].leaf_px_->GetValue(),
          daughters_fixed_mass_[0].leaf_py_->GetValue(),
          daughters_fixed_mass_[0].leaf_pz_->GetValue(),
          daughters_fixed_mass_[0].m_,
          daughters_fixed_mass_[1].leaf_px_->GetValue(),
          daughters_fixed_mass_[1].leaf_py_->GetValue(),
          daughters_fixed_mass_[1].leaf_pz_->GetValue(),
          daughters_fixed_mass_[1].m_);
    matched = true;
  } else {
    *(this->branch_address_templ_) = this->default_value_;
  }
  return matched;
}

template <class T> template<class T1, class T2, class T3, class T4, class T5, class T6>
void KinematicReducerLeaf<T>::FixedMassDaughtersTwoBodyDecayMotherMass(
                                                const ReducerLeaf<T1>& d1_px,
                                                const ReducerLeaf<T2>& d1_py,
                                                const ReducerLeaf<T3>& d1_pz,
                                                double d1_m,
                                                const ReducerLeaf<T4>& d2_px,
                                                const ReducerLeaf<T5>& d2_py,
                                                const ReducerLeaf<T6>& d2_pz,
                                                double d2_m) {
  using namespace doocore::io;

  sinfo << "Leaf " << this->name() << " is kinematic combination of ("
        << d1_px.name() << ", " << d1_py.name() << ", " << d1_pz.name() << ", mass " << d1_m << ") and ("
        << d2_px.name() << ", " << d2_py.name() << ", " << d2_pz.name() << ", mass " << d2_m << ")." << endmsg;
    
  EmptyDependantVectors();
  daughters_fixed_mass_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(d1_px.name(), d1_px.title(), d1_px.type(), d1_px.tree()),
      new ReducerLeaf<T>(d1_py.name(), d1_py.title(), d1_py.type(), d1_py.tree()),
      new ReducerLeaf<T>(d1_pz.name(), d1_pz.title(), d1_pz.type(), d1_pz.tree()),
      d1_m));
  
  daughters_fixed_mass_[0].leaf_px_->branch_address_ = d1_px.branch_address();
  daughters_fixed_mass_[0].leaf_py_->branch_address_ = d1_py.branch_address();
  daughters_fixed_mass_[0].leaf_pz_->branch_address_ = d1_pz.branch_address();
  
   daughters_fixed_mass_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(d2_px.name(), d2_px.title(), d2_px.type(), d2_px.tree()),
      new ReducerLeaf<T>(d2_py.name(), d2_py.title(), d2_py.type(), d2_py.tree()),
      new ReducerLeaf<T>(d2_pz.name(), d2_pz.title(), d2_pz.type(), d2_pz.tree()),
      d2_m));
  
  daughters_fixed_mass_[1].leaf_px_->branch_address_ = d2_px.branch_address();
  daughters_fixed_mass_[1].leaf_py_->branch_address_ = d2_py.branch_address();
  daughters_fixed_mass_[1].leaf_pz_->branch_address_ = d2_pz.branch_address();
}
  
template <class T>
void KinematicReducerLeaf<T>::EmptyDependantVectors() {
  using namespace doocore::io;
  for (typename std::vector<KinematicDaughterPropertiesFixedMass<T> >::iterator
       it = daughters_fixed_mass_.begin(), end = daughters_fixed_mass_.end();
       it != end; ++it) {
    sdebug << "deleting " << it->leaf_px_->name() << " copy." << endmsg;
    delete it->leaf_px_;
    sdebug << "deleting " << it->leaf_py_->name() << " copy." << endmsg;
    delete it->leaf_py_;
    sdebug << "deleting " << it->leaf_pz_->name() << " copy." << endmsg;
    delete it->leaf_pz_;
  }
  daughters_fixed_mass_.clear();
}
  
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_KINEMATICREDUCERLEAF_H
