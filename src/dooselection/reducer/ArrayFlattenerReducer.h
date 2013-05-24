#ifndef DOOSELECTION_REDUCER_ARRAYFLATTENERREDUCER_H
#define DOOSELECTION_REDUCER_ARRAYFLATTENERREDUCER_H

// from STL
#include <map>

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
 *  Leaves to be flattened are determined by array length leaves. All leaves 
 *  having one of these as array length will be flattened.
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
       *  @brief Add a leaf containing array length
       *
       *  All leaves containing array lengths must be of equal size for each 
       *  event. Otherwise, flattening will not work.
       *
       *  @param leaf_array_length leaf containing the array length
       */
      void AddArrayLengthLeaf(const ReducerLeaf<Float_t>& leaf_array_length) { leaves_array_length_.push_back(&leaf_array_length); }
      
    protected:
      virtual void PrepareSpecialBranches();
      virtual void FillOutputTree();
      
    private:
      /**
       *  @brief Pointers to leaves holding array length.
       */
      std::vector<const ReducerLeaf<Float_t>*> leaves_array_length_;
      
      /**
       *  @brief Pointer to first leaf holding array length.
       */
      const ReducerLeaf<Float_t>* leaf_array_length_;
      
      /**
       *  @brief Pointer to new leaf holding array index.
       */
      ReducerLeaf<Int_t>* leaf_array_index_;
      
      /**
       *  @brief Map containing all created flat float leaves and according array-based leaves
       */
      std::map<ReducerLeaf<Float_t>*, const ReducerLeaf<Float_t>*> leaves_map_float_;
      
      /**
       *  @brief Map containing all created flat double leaves and according array-based leaves
       */
      std::map<ReducerLeaf<Double_t>*, const ReducerLeaf<Float_t>*> leaves_map_double_;
      
      /**
       *  @brief Map containing all created flat int leaves and according array-based leaves
       */
      std::map<ReducerLeaf<Int_t>*, const ReducerLeaf<Float_t>*> leaves_map_int_;
      
    };
  } // namespace reducer
} // namespace dooselection


#endif // DOOSELECTION_REDUCER_ARRAYFLATTENERREDUCER_H
