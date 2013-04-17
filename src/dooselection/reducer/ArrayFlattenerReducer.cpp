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
  // fill leaves_map_double_ and leaves_map_int_ based on leaf_array_length_
}
  
void ArrayFlattenerReducer::FillOutputTree() {
  // loop over arrays and fill flat leaves

  int num_pvs = leaf_array_length_->GetValue();  
  for (int i=0; i<num_pvs; ++i) {
    for (std::map<ReducerLeaf<Double_t>*, const ReducerLeaf<Double_t>*>::iterator it = leaves_map_double_.begin();
         it != leaves_map_double_.end(); ++it) {
      
    }
    
    FlushEvent();
  }

}
} // namespace reducer
} // namespace dooselection
