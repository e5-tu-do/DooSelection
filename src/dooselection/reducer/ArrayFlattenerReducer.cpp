#include "ArrayFlattenerReducer.h"

// from STL
#include <string>

// from ROOT
#include "TString.h"

// from RooFit

// from DooCore
#include "doocore/io/MsgStream.h"

// from DooFit

// from project

namespace dooselection {
namespace reducer {

using namespace doocore::io;
  
ArrayFlattenerReducer::ArrayFlattenerReducer()
  : leaf_array_length_(NULL)
{}
  
void ArrayFlattenerReducer::CreateSpecialBranches() {
  std::string name_array_length_(leaf_array_length_->name());
  
  // loop over all interim leaves and check if array length is name_array_length_
  // fill leaves_map_double_ and leaves_map_int_ based on leaf_array_length_
  for (std::vector<ReducerLeaf<Float_t>* >::const_iterator it = GetInterimLeavesBegin(); it != GetInterimLeavesEnd(); ++it) {
    if ((*it)->LengthLeafName() == name_array_length_) {
      sdebug << (*it)->name() << " will be flattened. " << endmsg;
    }
  }
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
