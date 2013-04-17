#ifndef DOOSELECTION_REDUCER_ARRAYFLATTENERREDUCER_H
#define DOOSELECTION_REDUCER_ARRAYFLATTENERREDUCER_H

// from STL
#include <vector>

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
      /**
       *  @brief Default constructor
       */
      ArrayFlattenerReducer();
      
      /**
       *  @brief Set leaf containing array length
       *
       *  @param leaf_array_length leaf containing the array length
       */
      void set_leaf_array_length(const ReducerLeaf<Int_t>& leaf_array_length) { leaf_array_length_ = &leaf_array_length; }
      
    protected:
      virtual void CreateSpecialBranches();
      virtual void FillOutputTree();
      
    private:
      /**
       *  @brief Pointer to leaf holding array length.
       */
      const ReducerLeaf<Int_t>* leaf_array_length_;
      
      /**
       *  @brief Vector of all double array-based leaves to flatten
       */
      std::vector<const ReducerLeaf<Double_t>*> leaves_array_double_;

      /**
       *  @brief Vector of all int array-based leaves to flatten
       */
      std::vector<const ReducerLeaf<Int_t>*> leaves_array_int_;
      
      /**
       *  @brief Vector of all created flat double leaves
       */
      std::vector<ReducerLeaf<Double_t>*> leaves_flat_double_;
      
      /**
       *  @brief Vector of all created flat int leaves
       */
      std::vector<ReducerLeaf<Int_t>*> leaves_flat_int_;
      
    };
  } // namespace reducer
} // namespace dooselection


#endif // DOOSELECTION_REDUCER_ARRAYFLATTENERREDUCER_H
