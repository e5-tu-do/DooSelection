#ifndef DOOSELECTION_REDUCER_ARRAYFLATTENERREDUCER_H
#define DOOSELECTION_REDUCER_ARRAYFLATTENERREDUCER_H

// from STL

// from ROOT

// from RooFit

// from DooCore

// from project
#include "Reducer.h"

// forward declarations

/** @class dooselection::reducer::ArrayFlattenerReducer
 *  @brief Derived Reducer to flatten a set of array-based leaves into flat leaves.
 *
 *  This is a Reducer derived from Reducer. It can be used to flatten array-
 *  based leaves into plain numbers. Each array entry will be put into a 
 *  separate event in the output tree. Other leaves will simply be duplicated.
 *  The current array index will be put into a separate variable.
 *
 **/
namespace dooselection {
  namespace reducer {
    class ArrayFlattenerReducer : virtual public Reducer {
    public:
      
    protected:
      virtual void CreateSpecialBranches();
      
    private:
    };
  } // namespace reducer
} // namespace dooselection


#endif // DOOSELECTION_REDUCER_ARRAYFLATTENERREDUCER_H
