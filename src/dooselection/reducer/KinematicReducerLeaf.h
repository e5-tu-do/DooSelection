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
 * @class dooselection::reducer::KinematicReducerLeaf
 *
 * @brief Reducer leaf for kinematic calculations
 *
 * This helper class represents a leaf in a tree. It provides functionality to
 * do kinematic calculations based on other leaves.
 */

/**
 * @struct dooselection::reducer::KinematicDaughterPropertiesFixedMass
 *
 * @brief Helper container for dooselection::reducer::KinematicReducerLeaf
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
struct KinematicDaughterPropertiesVariableMass {
  KinematicDaughterPropertiesVariableMass(ReducerLeaf<T>* leaf_px,
                                          ReducerLeaf<T>* leaf_py,
                                          ReducerLeaf<T>* leaf_pz,
                                          ReducerLeaf<T>* leaf_m)
  : leaf_px_(leaf_px), leaf_py_(leaf_py), leaf_pz_(leaf_pz), leaf_m_(leaf_m) {}
  
  ReducerLeaf<T>* leaf_px_;
  ReducerLeaf<T>* leaf_py_;
  ReducerLeaf<T>* leaf_pz_;
  ReducerLeaf<T>* leaf_m_;
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
   *  @param d1_m daughter 1 fixed mass
   *  @param d2_px leaf to daughter 2 px
   *  @param d2_py leaf to daughter 2 py
   *  @param d2_pz leaf to daughter 2 pz
   *  @param d2_m daughter 2 fixed mass
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
  
  /**
   *  @brief Use two fixed mass and one variable mass daughters to generate a mother mass
   *
   *  Based on momenta leaves and fixed/variable masses of three daughters, a 
   *  new mass will be calculated for this leaf, representing a three body decay
   *  into these three particles. This function sets all necessary information.
   *
   *  @param d1_px leaf to daughter 1 px
   *  @param d1_py leaf to daughter 1 py
   *  @param d1_pz leaf to daughter 1 pz
   *  @param d1_m daughter 1 fixed mass
   *  @param d2_px leaf to daughter 2 px
   *  @param d2_py leaf to daughter 2 py
   *  @param d2_pz leaf to daughter 2 pz
   *  @param d2_m daughter 2 fixed mass
   *  @param d3_px leaf to daughter 3 px
   *  @param d3_py leaf to daughter 3 py
   *  @param d3_pz leaf to daughter 3 pz
   *  @param d3_m leaf to daughter 3 variable mass
   */
  template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
  void FixedMassDaughtersThreeBodyDecayMotherMass(const ReducerLeaf<T1>& d1_px,
                                                const ReducerLeaf<T2>& d1_py,
                                                const ReducerLeaf<T3>& d1_pz,
                                                double d1_m,
                                                const ReducerLeaf<T4>& d2_px,
                                                const ReducerLeaf<T5>& d2_py,
                                                const ReducerLeaf<T6>& d2_pz,
                                                double d2_m,
                                                const ReducerLeaf<T7>& d3_px,
                                                const ReducerLeaf<T8>& d3_py,
                                                const ReducerLeaf<T9>& d3_pz,
                                                const ReducerLeaf<T10>& d3_m);
  
  /**
   *  @brief Use three fixed mass daughters to generate a mother mass
   *
   *  Based on momenta leaves and fixed masses of three daughters, a
   *  new mass will be calculated for this leaf, representing a three body decay
   *  into these three particles. This function sets all necessary information.
   *
   *  @param d1_px leaf to daughter 1 px
   *  @param d1_py leaf to daughter 1 py
   *  @param d1_pz leaf to daughter 1 pz
   *  @param d1_m daughter 1 fixed mass
   *  @param d2_px leaf to daughter 2 px
   *  @param d2_py leaf to daughter 2 py
   *  @param d2_pz leaf to daughter 2 pz
   *  @param d2_m daughter 2 fixed mass
   *  @param d3_px leaf to daughter 3 px
   *  @param d3_py leaf to daughter 3 py
   *  @param d3_pz leaf to daughter 3 pz
   *  @param d3_m daughter 3 fixed mass
   */
  template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
  void FixedMassDaughtersThreeBodyDecayMotherMass(const ReducerLeaf<T1>& d1_px,
                                                const ReducerLeaf<T2>& d1_py,
                                                const ReducerLeaf<T3>& d1_pz,
                                                double d1_m,
                                                const ReducerLeaf<T4>& d2_px,
                                                const ReducerLeaf<T5>& d2_py,
                                                const ReducerLeaf<T6>& d2_pz,
                                                double d2_m,
                                                const ReducerLeaf<T7>& d3_px,
                                                const ReducerLeaf<T8>& d3_py,
                                                const ReducerLeaf<T9>& d3_pz,
                                                double d3_m);

  /**
   *  @brief Use four fixed mass daughters to generate a mother mass
   *
   *  Based on momenta leaves and fixed masses of four daughters, a new mass 
   *  will be calculated for this leaf, representing a four body decay into 
   *  these four particles. This function sets all necessary information.
   *
   *  @param d1_px leaf to daughter 1 px
   *  @param d1_py leaf to daughter 1 py
   *  @param d1_pz leaf to daughter 1 pz
   *  @param d1_m daughter 1 fixed mass
   *  @param d2_px leaf to daughter 2 px
   *  @param d2_py leaf to daughter 2 py
   *  @param d2_pz leaf to daughter 2 pz
   *  @param d2_m daughter 2 fixed mass
   *  @param d3_px leaf to daughter 3 px
   *  @param d3_py leaf to daughter 3 py
   *  @param d3_pz leaf to daughter 3 pz
   *  @param d3_m daughter 3 fixed mass
   *  @param d4_px leaf to daughter 4 px
   *  @param d4_py leaf to daughter 4 py
   *  @param d4_pz leaf to daughter 4 pz
   *  @param d4_m daughter 4 fixed mass
   */
  template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
  void FixedMassDaughtersFourBodyDecayMotherMass(const ReducerLeaf<T1>& d1_px,
                                                 const ReducerLeaf<T2>& d1_py,
                                                 const ReducerLeaf<T3>& d1_pz,
                                                 double d1_m,
                                                 const ReducerLeaf<T4>& d2_px,
                                                 const ReducerLeaf<T5>& d2_py,
                                                 const ReducerLeaf<T6>& d2_pz,
                                                 double d2_m,
                                                 const ReducerLeaf<T7>& d3_px,
                                                 const ReducerLeaf<T8>& d3_py,
                                                 const ReducerLeaf<T9>& d3_pz,
                                                 double d3_m,
                                                 const ReducerLeaf<T10>& d4_px,
                                                 const ReducerLeaf<T11>& d4_py,
                                                 const ReducerLeaf<T12>& d4_pz,
                                                 double d4_m);

  template<class T1, class T2, class T3, class T4, class T5, class T6>
  void DecayAngleTwoBodyDecay(double m_px,
                              double m_py,
                              double m_pz,
                              double m_m,
                              const ReducerLeaf<T1> d_px,
                              const ReducerLeaf<T2> d_py,
                              const ReducerLeaf<T3> d_pz,
                              double d_m,
                              const ReducerLeaf<T4> gd_px,
                              const ReducerLeaf<T5> gd_py,
                              const ReducerLeaf<T6> gd_pz,
                              double gd_m);

  template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
  void DecayAngleTwoBodyDecay(const ReducerLeaf<T1> m_px,
                              const ReducerLeaf<T2> m_py,
                              const ReducerLeaf<T3> m_pz,
                              double m_m,
                              const ReducerLeaf<T4> d_px,
                              const ReducerLeaf<T5> d_py,
                              const ReducerLeaf<T6> d_pz,
                              double d_m,
                              const ReducerLeaf<T7> gd_px,
                              const ReducerLeaf<T8> gd_py,
                              const ReducerLeaf<T9> gd_pz,
                              double gd_m);

  template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
  void DecayAngleThreeBodyDecay(const ReducerLeaf<T1> m_px,
                                const ReducerLeaf<T2> m_py,
                                const ReducerLeaf<T3> m_pz,
                                double m_m,
                                const ReducerLeaf<T4> d_px,
                                const ReducerLeaf<T5> d_py,
                                const ReducerLeaf<T6> d_pz,
                                double d_m,
                                const ReducerLeaf<T7> gd1_px,
                                const ReducerLeaf<T8> gd1_py,
                                const ReducerLeaf<T9> gd1_pz,
                                double gd1_m,
                                const ReducerLeaf<T10> gd2_px,
                                const ReducerLeaf<T11> gd2_py,
                                const ReducerLeaf<T12> gd2_pz,
                                double gd2_m);
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
  
  /**
   *  @brief Vector containing variable mass daughters for mass hypothesis calculation
   */
  std::vector<KinematicDaughterPropertiesVariableMass<T> > daughters_variable_mass_;

  /**
   *  @brief Vector containing fixed momenta (of mother) for decay angle calculation
   */
  std::vector<double> fixed_momenta_decay_angle_;

  /**
   *  @brief Vector containing momenta for decay angle calculation
   */
  std::vector<KinematicDaughterPropertiesFixedMass<T> > momenta_decay_angle_;
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
  if (daughters_fixed_mass_.size() == 2 && daughters_variable_mass_.empty()) {
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
  } else if (daughters_fixed_mass_.size() == 2 && daughters_variable_mass_.size() == 1) {
    daughters_fixed_mass_[0].leaf_px_->UpdateValue();
    daughters_fixed_mass_[0].leaf_py_->UpdateValue();
    daughters_fixed_mass_[0].leaf_pz_->UpdateValue();
    daughters_fixed_mass_[1].leaf_px_->UpdateValue();
    daughters_fixed_mass_[1].leaf_py_->UpdateValue();
    daughters_fixed_mass_[1].leaf_pz_->UpdateValue();

    daughters_variable_mass_[0].leaf_px_->UpdateValue();
    daughters_variable_mass_[0].leaf_py_->UpdateValue();
    daughters_variable_mass_[0].leaf_pz_->UpdateValue();
    daughters_variable_mass_[0].leaf_m_->UpdateValue();
    
    *(this->branch_address_templ_) = MotherThreeBodyDecayMass(
          daughters_fixed_mass_[0].leaf_px_->GetValue(),
          daughters_fixed_mass_[0].leaf_py_->GetValue(),
          daughters_fixed_mass_[0].leaf_pz_->GetValue(),
          daughters_fixed_mass_[0].m_,
          daughters_fixed_mass_[1].leaf_px_->GetValue(),
          daughters_fixed_mass_[1].leaf_py_->GetValue(),
          daughters_fixed_mass_[1].leaf_pz_->GetValue(),
          daughters_fixed_mass_[1].m_,
          daughters_variable_mass_[0].leaf_px_->GetValue(),
          daughters_variable_mass_[0].leaf_py_->GetValue(),
          daughters_variable_mass_[0].leaf_pz_->GetValue(),
          daughters_variable_mass_[0].leaf_m_->GetValue());
    matched = true;
  } else if (daughters_fixed_mass_.size() == 3 && daughters_variable_mass_.empty()) {
    daughters_fixed_mass_[0].leaf_px_->UpdateValue();
    daughters_fixed_mass_[0].leaf_py_->UpdateValue();
    daughters_fixed_mass_[0].leaf_pz_->UpdateValue();
    daughters_fixed_mass_[1].leaf_px_->UpdateValue();
    daughters_fixed_mass_[1].leaf_py_->UpdateValue();
    daughters_fixed_mass_[1].leaf_pz_->UpdateValue();
    daughters_fixed_mass_[2].leaf_px_->UpdateValue();
    daughters_fixed_mass_[2].leaf_py_->UpdateValue();
    daughters_fixed_mass_[2].leaf_pz_->UpdateValue();

    *(this->branch_address_templ_) = MotherThreeBodyDecayMass(
          daughters_fixed_mass_[0].leaf_px_->GetValue(),
          daughters_fixed_mass_[0].leaf_py_->GetValue(),
          daughters_fixed_mass_[0].leaf_pz_->GetValue(),
          daughters_fixed_mass_[0].m_,
          daughters_fixed_mass_[1].leaf_px_->GetValue(),
          daughters_fixed_mass_[1].leaf_py_->GetValue(),
          daughters_fixed_mass_[1].leaf_pz_->GetValue(),
          daughters_fixed_mass_[1].m_,
          daughters_fixed_mass_[2].leaf_px_->GetValue(),
          daughters_fixed_mass_[2].leaf_py_->GetValue(),
          daughters_fixed_mass_[2].leaf_pz_->GetValue(),
          daughters_fixed_mass_[2].m_);
    matched = true;
  } else if (daughters_fixed_mass_.size() == 4 && daughters_variable_mass_.size() == 0) {
    daughters_fixed_mass_[0].leaf_px_->UpdateValue();
    daughters_fixed_mass_[0].leaf_py_->UpdateValue();
    daughters_fixed_mass_[0].leaf_pz_->UpdateValue();
    daughters_fixed_mass_[1].leaf_px_->UpdateValue();
    daughters_fixed_mass_[1].leaf_py_->UpdateValue();
    daughters_fixed_mass_[1].leaf_pz_->UpdateValue();
    daughters_fixed_mass_[2].leaf_px_->UpdateValue();
    daughters_fixed_mass_[2].leaf_py_->UpdateValue();
    daughters_fixed_mass_[2].leaf_pz_->UpdateValue();
    daughters_fixed_mass_[3].leaf_px_->UpdateValue();
    daughters_fixed_mass_[3].leaf_py_->UpdateValue();
    daughters_fixed_mass_[3].leaf_pz_->UpdateValue();
    
    *(this->branch_address_templ_) = MotherFourBodyDecayMass(
          daughters_fixed_mass_[0].leaf_px_->GetValue(),
          daughters_fixed_mass_[0].leaf_py_->GetValue(),
          daughters_fixed_mass_[0].leaf_pz_->GetValue(),
          daughters_fixed_mass_[0].m_,
          daughters_fixed_mass_[1].leaf_px_->GetValue(),
          daughters_fixed_mass_[1].leaf_py_->GetValue(),
          daughters_fixed_mass_[1].leaf_pz_->GetValue(),
          daughters_fixed_mass_[1].m_,
          daughters_fixed_mass_[2].leaf_px_->GetValue(),
          daughters_fixed_mass_[2].leaf_py_->GetValue(),
          daughters_fixed_mass_[2].leaf_pz_->GetValue(),
          daughters_fixed_mass_[2].m_,
          daughters_fixed_mass_[3].leaf_px_->GetValue(),
          daughters_fixed_mass_[3].leaf_py_->GetValue(),
          daughters_fixed_mass_[3].leaf_pz_->GetValue(),
          daughters_fixed_mass_[3].m_);
    matched = true;
  } else if (fixed_momenta_decay_angle_.size() == 1 && momenta_decay_angle_.size() == 2) {
    momenta_decay_angle_[0].leaf_px_->UpdateValue();
    momenta_decay_angle_[0].leaf_py_->UpdateValue();
    momenta_decay_angle_[0].leaf_pz_->UpdateValue();
    momenta_decay_angle_[1].leaf_px_->UpdateValue();
    momenta_decay_angle_[1].leaf_py_->UpdateValue();
    momenta_decay_angle_[1].leaf_pz_->UpdateValue();

    *(this->branch_address_templ_) = TwoBodyDecayAngle(
          fixed_momenta_decay_angle_[0],
          fixed_momenta_decay_angle_[1],
          fixed_momenta_decay_angle_[2],
          fixed_momenta_decay_angle_[3],
          momenta_decay_angle_[0].leaf_px_->GetValue(),
          momenta_decay_angle_[0].leaf_py_->GetValue(),
          momenta_decay_angle_[0].leaf_pz_->GetValue(),
          momenta_decay_angle_[0].m_,
          momenta_decay_angle_[1].leaf_px_->GetValue(),
          momenta_decay_angle_[1].leaf_py_->GetValue(),
          momenta_decay_angle_[1].leaf_pz_->GetValue(),
          momenta_decay_angle_[1].m_);
    matched = true;
  } else if (momenta_decay_angle_.size() == 3) {
    momenta_decay_angle_[0].leaf_px_->UpdateValue();
    momenta_decay_angle_[0].leaf_py_->UpdateValue();
    momenta_decay_angle_[0].leaf_pz_->UpdateValue();
    momenta_decay_angle_[1].leaf_px_->UpdateValue();
    momenta_decay_angle_[1].leaf_py_->UpdateValue();
    momenta_decay_angle_[1].leaf_pz_->UpdateValue();
    momenta_decay_angle_[2].leaf_px_->UpdateValue();
    momenta_decay_angle_[2].leaf_py_->UpdateValue();
    momenta_decay_angle_[2].leaf_pz_->UpdateValue();

    *(this->branch_address_templ_) = TwoBodyDecayAngle(
          momenta_decay_angle_[0].leaf_px_->GetValue(),
          momenta_decay_angle_[0].leaf_py_->GetValue(),
          momenta_decay_angle_[0].leaf_pz_->GetValue(),
          momenta_decay_angle_[0].m_,
          momenta_decay_angle_[1].leaf_px_->GetValue(),
          momenta_decay_angle_[1].leaf_py_->GetValue(),
          momenta_decay_angle_[1].leaf_pz_->GetValue(),
          momenta_decay_angle_[1].m_,
          momenta_decay_angle_[2].leaf_px_->GetValue(),
          momenta_decay_angle_[2].leaf_py_->GetValue(),
          momenta_decay_angle_[2].leaf_pz_->GetValue(),
          momenta_decay_angle_[2].m_);
    matched = true;
  } else if (momenta_decay_angle_.size() == 4) {
    momenta_decay_angle_[0].leaf_px_->UpdateValue();
    momenta_decay_angle_[0].leaf_py_->UpdateValue();
    momenta_decay_angle_[0].leaf_pz_->UpdateValue();
    momenta_decay_angle_[1].leaf_px_->UpdateValue();
    momenta_decay_angle_[1].leaf_py_->UpdateValue();
    momenta_decay_angle_[1].leaf_pz_->UpdateValue();
    momenta_decay_angle_[2].leaf_px_->UpdateValue();
    momenta_decay_angle_[2].leaf_py_->UpdateValue();
    momenta_decay_angle_[2].leaf_pz_->UpdateValue();
    momenta_decay_angle_[3].leaf_px_->UpdateValue();
    momenta_decay_angle_[3].leaf_py_->UpdateValue();
    momenta_decay_angle_[3].leaf_pz_->UpdateValue();

    *(this->branch_address_templ_) = ThreeBodyDecayAngle(
          momenta_decay_angle_[0].leaf_px_->GetValue(),
          momenta_decay_angle_[0].leaf_py_->GetValue(),
          momenta_decay_angle_[0].leaf_pz_->GetValue(),
          momenta_decay_angle_[0].m_,
          momenta_decay_angle_[1].leaf_px_->GetValue(),
          momenta_decay_angle_[1].leaf_py_->GetValue(),
          momenta_decay_angle_[1].leaf_pz_->GetValue(),
          momenta_decay_angle_[1].m_,
          momenta_decay_angle_[2].leaf_px_->GetValue(),
          momenta_decay_angle_[2].leaf_py_->GetValue(),
          momenta_decay_angle_[2].leaf_pz_->GetValue(),
          momenta_decay_angle_[2].m_,
          momenta_decay_angle_[3].leaf_px_->GetValue(),
          momenta_decay_angle_[3].leaf_py_->GetValue(),
          momenta_decay_angle_[3].leaf_pz_->GetValue(),
          momenta_decay_angle_[3].m_);
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

  sout  << "Leaf " << this->name() << ": kinematic combination of ("
        << d1_px.name() << ", " << d1_py.name() << ", " << d1_pz.name() << ", mass: " << d1_m << ") and ("
        << d2_px.name() << ", " << d2_py.name() << ", " << d2_pz.name() << ", mass: " << d2_m << ")." << endmsg;
    
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
  
template <class T> template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
void KinematicReducerLeaf<T>::FixedMassDaughtersThreeBodyDecayMotherMass(const ReducerLeaf<T1>& d1_px,
                                                const ReducerLeaf<T2>& d1_py,
                                                const ReducerLeaf<T3>& d1_pz,
                                                double d1_m,
                                                const ReducerLeaf<T4>& d2_px,
                                                const ReducerLeaf<T5>& d2_py,
                                                const ReducerLeaf<T6>& d2_pz,
                                                double d2_m,
                                                const ReducerLeaf<T7>& d3_px,
                                                const ReducerLeaf<T8>& d3_py,
                                                const ReducerLeaf<T9>& d3_pz,
                                                const ReducerLeaf<T10>& d3_m) {
  using namespace doocore::io;

  sout  << "Leaf " << this->name() << ": kinematic combination of ("
        << d1_px.name() << ", " << d1_py.name() << ", " << d1_pz.name() << ", mass: " << d1_m << ") and ("
        << d2_px.name() << ", " << d2_py.name() << ", " << d2_pz.name() << ", mass: " << d2_m << ") and ("
        << d3_px.name() << ", " << d3_py.name() << ", " << d3_pz.name() << ", mass: " << d3_m.name() << ")." << endmsg;
    
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
  
  daughters_variable_mass_.push_back(KinematicDaughterPropertiesVariableMass<T>(
      new ReducerLeaf<T>(d3_px.name(), d3_px.title(), d3_px.type(), d3_px.tree()),
      new ReducerLeaf<T>(d3_py.name(), d3_py.title(), d3_py.type(), d3_py.tree()),
      new ReducerLeaf<T>(d3_pz.name(), d3_pz.title(), d3_pz.type(), d3_pz.tree()),
      new ReducerLeaf<T>(d3_m.name(), d3_m.title(), d3_m.type(), d3_m.tree())));
  
  daughters_variable_mass_[0].leaf_px_->branch_address_ = d3_px.branch_address();
  daughters_variable_mass_[0].leaf_py_->branch_address_ = d3_py.branch_address();
  daughters_variable_mass_[0].leaf_pz_->branch_address_ = d3_pz.branch_address();
  daughters_variable_mass_[0].leaf_m_->branch_address_  = d3_m.branch_address();
}

template <class T> template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
void KinematicReducerLeaf<T>::FixedMassDaughtersThreeBodyDecayMotherMass(const ReducerLeaf<T1>& d1_px,
                                                const ReducerLeaf<T2>& d1_py,
                                                const ReducerLeaf<T3>& d1_pz,
                                                double d1_m,
                                                const ReducerLeaf<T4>& d2_px,
                                                const ReducerLeaf<T5>& d2_py,
                                                const ReducerLeaf<T6>& d2_pz,
                                                double d2_m,
                                                const ReducerLeaf<T7>& d3_px,
                                                const ReducerLeaf<T8>& d3_py,
                                                const ReducerLeaf<T9>& d3_pz,
                                                double d3_m) {
  using namespace doocore::io;

  sout  << "Leaf " << this->name() << ": kinematic combination of ("
        << d1_px.name() << ", " << d1_py.name() << ", " << d1_pz.name() << ", mass: " << d1_m << ") and ("
        << d2_px.name() << ", " << d2_py.name() << ", " << d2_pz.name() << ", mass: " << d2_m << ") and ("
        << d3_px.name() << ", " << d3_py.name() << ", " << d3_pz.name() << ", mass: " << d3_m << ")." << endmsg;

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

  daughters_fixed_mass_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(d3_px.name(), d3_px.title(), d3_px.type(), d3_px.tree()),
      new ReducerLeaf<T>(d3_py.name(), d3_py.title(), d3_py.type(), d3_py.tree()),
      new ReducerLeaf<T>(d3_pz.name(), d3_pz.title(), d3_pz.type(), d3_pz.tree()),
      d3_m));

  daughters_fixed_mass_[2].leaf_px_->branch_address_ = d3_px.branch_address();
  daughters_fixed_mass_[2].leaf_py_->branch_address_ = d3_py.branch_address();
  daughters_fixed_mass_[2].leaf_pz_->branch_address_ = d3_pz.branch_address();
}

template <class T> template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
void KinematicReducerLeaf<T>::FixedMassDaughtersFourBodyDecayMotherMass(const ReducerLeaf<T1>& d1_px,
                                              const ReducerLeaf<T2>& d1_py,
                                              const ReducerLeaf<T3>& d1_pz,
                                              double d1_m,
                                              const ReducerLeaf<T4>& d2_px,
                                              const ReducerLeaf<T5>& d2_py,
                                              const ReducerLeaf<T6>& d2_pz,
                                              double d2_m,
                                              const ReducerLeaf<T7>& d3_px,
                                              const ReducerLeaf<T8>& d3_py,
                                              const ReducerLeaf<T9>& d3_pz,
                                              double d3_m,
                                              const ReducerLeaf<T10>& d4_px,
                                              const ReducerLeaf<T11>& d4_py,
                                              const ReducerLeaf<T12>& d4_pz,
                                              double d4_m) {
  using namespace doocore::io;

  sout  << "Leaf " << this->name() << ": kinematic combination of ("
        << d1_px.name() << ", " << d1_py.name() << ", " << d1_pz.name() << ", mass: " << d1_m << ") and ("
        << d2_px.name() << ", " << d2_py.name() << ", " << d2_pz.name() << ", mass: " << d2_m << ") and ("
        << d3_px.name() << ", " << d3_py.name() << ", " << d3_pz.name() << ", mass: " << d3_m << ") and ("
        << d4_px.name() << ", " << d4_py.name() << ", " << d4_pz.name() << ", mass: " << d4_m << ")." << endmsg;
    
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
  
  daughters_fixed_mass_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(d3_px.name(), d3_px.title(), d3_px.type(), d3_px.tree()),
      new ReducerLeaf<T>(d3_py.name(), d3_py.title(), d3_py.type(), d3_py.tree()),
      new ReducerLeaf<T>(d3_pz.name(), d3_pz.title(), d3_pz.type(), d3_pz.tree()),
      d3_m));
  
  daughters_fixed_mass_[2].leaf_px_->branch_address_ = d3_px.branch_address();
  daughters_fixed_mass_[2].leaf_py_->branch_address_ = d3_py.branch_address();
  daughters_fixed_mass_[2].leaf_pz_->branch_address_ = d3_pz.branch_address();
  
  daughters_fixed_mass_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(d4_px.name(), d4_px.title(), d4_px.type(), d4_px.tree()),
      new ReducerLeaf<T>(d4_py.name(), d4_py.title(), d4_py.type(), d4_py.tree()),
      new ReducerLeaf<T>(d4_pz.name(), d4_pz.title(), d4_pz.type(), d4_pz.tree()),
      d4_m));
  
  daughters_fixed_mass_[3].leaf_px_->branch_address_ = d4_px.branch_address();
  daughters_fixed_mass_[3].leaf_py_->branch_address_ = d4_py.branch_address();
  daughters_fixed_mass_[3].leaf_pz_->branch_address_ = d4_pz.branch_address();
}

template <class T> template<class T1, class T2, class T3, class T4, class T5, class T6>
void KinematicReducerLeaf<T>::DecayAngleTwoBodyDecay(double m_px,
                                                     double m_py,
                                                     double m_pz,
                                                     double m_m,
                                                     const ReducerLeaf<T1> d_px,
                                                     const ReducerLeaf<T2> d_py,
                                                     const ReducerLeaf<T3> d_pz,
                                                     double d_m,
                                                     const ReducerLeaf<T4> gd_px,
                                                     const ReducerLeaf<T5> gd_py,
                                                     const ReducerLeaf<T6> gd_pz,
                                                     double gd_m) {
  using namespace doocore::io;

  EmptyDependantVectors();
  fixed_momenta_decay_angle_.push_back(m_px);
  fixed_momenta_decay_angle_.push_back(m_py);
  fixed_momenta_decay_angle_.push_back(m_pz);
  fixed_momenta_decay_angle_.push_back(m_m);
  momenta_decay_angle_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(d_px.name(), d_px.title(), d_px.type(), d_px.tree()),
      new ReducerLeaf<T>(d_py.name(), d_py.title(), d_py.type(), d_py.tree()),
      new ReducerLeaf<T>(d_pz.name(), d_pz.title(), d_pz.type(), d_pz.tree()),
      d_m));

  momenta_decay_angle_[0].leaf_px_->branch_address_ = d_px.branch_address();
  momenta_decay_angle_[0].leaf_py_->branch_address_ = d_py.branch_address();
  momenta_decay_angle_[0].leaf_pz_->branch_address_ = d_pz.branch_address();

  momenta_decay_angle_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(gd_px.name(), gd_px.title(), gd_px.type(), gd_px.tree()),
      new ReducerLeaf<T>(gd_py.name(), gd_py.title(), gd_py.type(), gd_py.tree()),
      new ReducerLeaf<T>(gd_pz.name(), gd_pz.title(), gd_pz.type(), gd_pz.tree()),
      gd_m));

  momenta_decay_angle_[1].leaf_px_->branch_address_ = gd_px.branch_address();
  momenta_decay_angle_[1].leaf_py_->branch_address_ = gd_py.branch_address();
  momenta_decay_angle_[1].leaf_pz_->branch_address_ = gd_pz.branch_address();
}

template <class T> template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
void KinematicReducerLeaf<T>::DecayAngleTwoBodyDecay(const ReducerLeaf<T1> m_px,
                                                     const ReducerLeaf<T2> m_py,
                                                     const ReducerLeaf<T3> m_pz,
                                                     double m_m,
                                                     const ReducerLeaf<T4> d_px,
                                                     const ReducerLeaf<T5> d_py,
                                                     const ReducerLeaf<T6> d_pz,
                                                     double d_m,
                                                     const ReducerLeaf<T7> gd_px,
                                                     const ReducerLeaf<T8> gd_py,
                                                     const ReducerLeaf<T9> gd_pz,
                                                     double gd_m) {
  using namespace doocore::io;

  EmptyDependantVectors();
  momenta_decay_angle_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(m_px.name(), m_px.title(), m_px.type(), m_px.tree()),
      new ReducerLeaf<T>(m_py.name(), m_py.title(), m_py.type(), m_py.tree()),
      new ReducerLeaf<T>(m_pz.name(), m_pz.title(), m_pz.type(), m_pz.tree()),
      m_m));

  momenta_decay_angle_[0].leaf_px_->branch_address_ = m_px.branch_address();
  momenta_decay_angle_[0].leaf_py_->branch_address_ = m_py.branch_address();
  momenta_decay_angle_[0].leaf_pz_->branch_address_ = m_pz.branch_address();

  momenta_decay_angle_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(d_px.name(), d_px.title(), d_px.type(), d_px.tree()),
      new ReducerLeaf<T>(d_py.name(), d_py.title(), d_py.type(), d_py.tree()),
      new ReducerLeaf<T>(d_pz.name(), d_pz.title(), d_pz.type(), d_pz.tree()),
      d_m));

  momenta_decay_angle_[1].leaf_px_->branch_address_ = d_px.branch_address();
  momenta_decay_angle_[1].leaf_py_->branch_address_ = d_py.branch_address();
  momenta_decay_angle_[1].leaf_pz_->branch_address_ = d_pz.branch_address();
  
  momenta_decay_angle_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(gd_px.name(), gd_px.title(), gd_px.type(), gd_px.tree()),
      new ReducerLeaf<T>(gd_py.name(), gd_py.title(), gd_py.type(), gd_py.tree()),
      new ReducerLeaf<T>(gd_pz.name(), gd_pz.title(), gd_pz.type(), gd_pz.tree()),
      gd_m));

  momenta_decay_angle_[2].leaf_px_->branch_address_ = gd_px.branch_address();
  momenta_decay_angle_[2].leaf_py_->branch_address_ = gd_py.branch_address();
  momenta_decay_angle_[2].leaf_pz_->branch_address_ = gd_pz.branch_address();
}

template <class T> template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
void KinematicReducerLeaf<T>::DecayAngleThreeBodyDecay(const ReducerLeaf<T1> m_px,
                                                       const ReducerLeaf<T2> m_py,
                                                       const ReducerLeaf<T3> m_pz,
                                                       double m_m,
                                                       const ReducerLeaf<T4> d_px,
                                                       const ReducerLeaf<T5> d_py,
                                                       const ReducerLeaf<T6> d_pz,
                                                       double d_m,
                                                       const ReducerLeaf<T7> gd1_px,
                                                       const ReducerLeaf<T8> gd1_py,
                                                       const ReducerLeaf<T9> gd1_pz,
                                                       double gd1_m,
                                                       const ReducerLeaf<T10> gd2_px,
                                                       const ReducerLeaf<T11> gd2_py,
                                                       const ReducerLeaf<T12> gd2_pz,
                                                       double gd2_m) {
  using namespace doocore::io;

  EmptyDependantVectors();
  momenta_decay_angle_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(m_px.name(), m_px.title(), m_px.type(), m_px.tree()),
      new ReducerLeaf<T>(m_py.name(), m_py.title(), m_py.type(), m_py.tree()),
      new ReducerLeaf<T>(m_pz.name(), m_pz.title(), m_pz.type(), m_pz.tree()),
      m_m));

  momenta_decay_angle_[0].leaf_px_->branch_address_ = m_px.branch_address();
  momenta_decay_angle_[0].leaf_py_->branch_address_ = m_py.branch_address();
  momenta_decay_angle_[0].leaf_pz_->branch_address_ = m_pz.branch_address();

  momenta_decay_angle_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(d_px.name(), d_px.title(), d_px.type(), d_px.tree()),
      new ReducerLeaf<T>(d_py.name(), d_py.title(), d_py.type(), d_py.tree()),
      new ReducerLeaf<T>(d_pz.name(), d_pz.title(), d_pz.type(), d_pz.tree()),
      d_m));

  momenta_decay_angle_[1].leaf_px_->branch_address_ = d_px.branch_address();
  momenta_decay_angle_[1].leaf_py_->branch_address_ = d_py.branch_address();
  momenta_decay_angle_[1].leaf_pz_->branch_address_ = d_pz.branch_address();

  momenta_decay_angle_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(gd1_px.name(), gd1_px.title(), gd1_px.type(), gd1_px.tree()),
      new ReducerLeaf<T>(gd1_py.name(), gd1_py.title(), gd1_py.type(), gd1_py.tree()),
      new ReducerLeaf<T>(gd1_pz.name(), gd1_pz.title(), gd1_pz.type(), gd1_pz.tree()),
      gd1_m));

  momenta_decay_angle_[2].leaf_px_->branch_address_ = gd1_px.branch_address();
  momenta_decay_angle_[2].leaf_py_->branch_address_ = gd1_py.branch_address();
  momenta_decay_angle_[2].leaf_pz_->branch_address_ = gd1_pz.branch_address();

  momenta_decay_angle_.push_back(KinematicDaughterPropertiesFixedMass<T>(
      new ReducerLeaf<T>(gd2_px.name(), gd2_px.title(), gd2_px.type(), gd2_px.tree()),
      new ReducerLeaf<T>(gd2_py.name(), gd2_py.title(), gd2_py.type(), gd2_py.tree()),
      new ReducerLeaf<T>(gd2_pz.name(), gd2_pz.title(), gd2_pz.type(), gd2_pz.tree()),
      gd2_m));

  momenta_decay_angle_[3].leaf_px_->branch_address_ = gd2_px.branch_address();
  momenta_decay_angle_[3].leaf_py_->branch_address_ = gd2_py.branch_address();
  momenta_decay_angle_[3].leaf_pz_->branch_address_ = gd2_pz.branch_address();
}

template <class T>
void KinematicReducerLeaf<T>::EmptyDependantVectors() {
  using namespace doocore::io;
  for (typename std::vector<KinematicDaughterPropertiesFixedMass<T> >::iterator
       it = daughters_fixed_mass_.begin(), end = daughters_fixed_mass_.end();
       it != end; ++it) {
    delete it->leaf_px_;
    delete it->leaf_py_;
    delete it->leaf_pz_;
  }
  daughters_fixed_mass_.clear();
  
  for (typename std::vector<KinematicDaughterPropertiesVariableMass<T> >::iterator
       it = daughters_variable_mass_.begin(), end = daughters_variable_mass_.end();
       it != end; ++it) {
    delete it->leaf_px_;
    delete it->leaf_py_;
    delete it->leaf_pz_;
    delete it->leaf_m_;
  }
  daughters_variable_mass_.clear();

  for (typename std::vector<KinematicDaughterPropertiesFixedMass<T> >::iterator
       it = momenta_decay_angle_.begin(), end = momenta_decay_angle_.end();
       it != end; ++it) {
    delete it->leaf_px_;
    delete it->leaf_py_;
    delete it->leaf_pz_;
  }
  momenta_decay_angle_.clear();
}
  
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_KINEMATICREDUCERLEAF_H
