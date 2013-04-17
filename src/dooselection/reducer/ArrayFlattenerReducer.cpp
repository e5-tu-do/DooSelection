#include "ArrayFlattenerReducer.h"

// from STL

// from ROOT

// from RooFit

// from DooCore

// from DooFit

namespace dooselection {
namespace reducer {
ArrayFlattenerReducer::ArrayFlattenerReducer()
  : leaf_array_length_(NULL)
{}
  
void ArrayFlattenerReducer::CreateSpecialBranches() {
  // fill leaves_array_double_ and leaves_array_int_ based on leaf_array_length_
  // create according flat leaves
}
  
void ArrayFlattenerReducer::FillOutputTree() {
  // loop over arrays and fill flat leaves
}
} // namespace reducer
} // namespace dooselection
