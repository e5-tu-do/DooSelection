#ifndef DOOSELECTION_REDUCER_KINEMATICREDUCERLEAF_H
#define DOOSELECTION_REDUCER_KINEMATICREDUCERLEAF_H

// from STL

// from ROOT

// from DooCore

// from project
#include "dooselection/reducer/ReducerLeaf.h"

// forward decalarations

/**
 * @class dooselection::reducer::KinematicReducerLeaf class
 *
 * This helper class represents a leaf in a tree. It provides functionality to
 * do kinematic calculations based on other leaves.
 */

namespace dooselection {
namespace reducer {
template <typename T>
class KinematicReducerLeaf : public ReducerLeaf<T> {
public:
  KinematicReducerLeaf(TString name, TString title, TString type, TTree* tree, T default_value=T());
  
  virtual ~KinematicReducerLeaf() {}
  
private:
};

template <class T>
KinematicReducerLeaf<T>::KinematicReducerLeaf(TString name, TString title, TString type, TTree* tree, T default_value)
: ReducerLeaf<T>(name, title, type, tree, default_value)
{
}
  
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_KINEMATICREDUCERLEAF_H
