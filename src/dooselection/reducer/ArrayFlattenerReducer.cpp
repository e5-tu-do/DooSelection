#include "ArrayFlattenerReducer.h"

// from STL

// from ROOT

// from RooFit

// from DooCore
#include "doocore/io/MsgStream.h"

// from DooFit

namespace dooselection {
namespace reducer {

using namespace doocore::io;
  
ArrayFlattenerReducer::ArrayFlattenerReducer()
  : leaf_array_length_(NULL)
{}
  
void ArrayFlattenerReducer::CreateSpecialBranches() {
  // fill leaves_map_double_ and leaves_map_int_ based on leaf_array_length_
}
  
void ArrayFlattenerReducer::FillOutputTree() {
  int num_pvs = leaf_array_length_->GetValue();  
  for (int i=0; i<num_pvs; ++i) {
    sdebug << "i = " << i << endmsg;
    for (std::map<ReducerLeaf<Double_t>*, const ReducerLeaf<Double_t>*>::iterator it = leaves_map_double_.begin();
         it != leaves_map_double_.end(); ++it) {
      (*it->first) = it->second->GetValue(i);
      sdebug << " " << it->first->name() << " = " << it->first->GetValue() << endmsg;
    }
    for (std::map<ReducerLeaf<Int_t>*, const ReducerLeaf<Int_t>*>::iterator it = leaves_map_int_.begin();
         it != leaves_map_int_.end(); ++it) {
      (*it->first) = it->second->GetValue(i);
      sdebug << " " << it->first->name() << " = " << it->first->GetValue() << endmsg;
    }
    
    FlushEvent();
  }
}
} // namespace reducer
} // namespace dooselection
