#ifndef DOOSELECTION_REDUCER_LEAFDOUBLERREDUCER_H
#define DOOSELECTION_REDUCER_LEAFDOUBLERREDUCER_H

// from STL

// from ROOT

// from RooFit

// from DooCore

// from project
#include "Reducer.h"

// forward declarations

/** @class dooselection::reducer::LeafDoublerReducer
 *  @brief Derived Reducer to copy all non-double/non-float/non-int leaves into double leaves
 *
 *  This is a Reducer derived from Reducer. It can be used to copy all leaves 
 *  that are not of type Float_t or Double_t or Int_t into a Double_t copy to 
 *  fix a problem in RooFit which cannot import tree leaves of other types.
 *
 **/
namespace dooselection {
namespace reducer {
class LeafDoublerReducer : virtual public Reducer {
 public:
  
 protected:
  virtual void CreateSpecialBranches();
  
 private:
};
} // namespace reducer
} // namespace dooselection


#endif // DOOSELECTION_REDUCER_LEAFDOUBLERREDUCER_H
