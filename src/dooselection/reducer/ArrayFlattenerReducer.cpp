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
  
void ArrayFlattenerReducer::PrepareSpecialBranches() {
  if (leaf_array_length_ == NULL) {
    sinfo << "ArrayFlattenerReducer: Array length leaf not set. Will not flatten." << endmsg;
  } else {
    std::string name_array_length(leaf_array_length_->name());
    sinfo << "ArrayFlattenerReducer: Flattening all leaves that have array size " << name_array_length << endmsg;
    
    std::string name_array_index = name_array_length + "_index";
    leaf_array_index_ = &CreateIntLeaf(name_array_index);
    sinfo << "Array index will be written into: " << name_array_index << endmsg;
    
    sinfo << "The following leaves will be flattened: " << endmsg;
    // loop over all interim leaves and check if array length is name_array_length_
    // fill leaves_map_double_ and leaves_map_int_ based on leaf_array_length_
    for (std::vector<ReducerLeaf<Float_t>* >::const_iterator it = GetInterimLeavesBegin(); it != GetInterimLeavesEnd(); ++it) {
      if ((*it)->LengthLeafName() == name_array_length) {
        if ((*it)->type() == "Double_t") {
          ReducerLeaf<Double_t>& flat_leaf = CreateDoubleLeaf((*it)->name()+"_flat");
          leaves_map_double_[&flat_leaf] = *it;
          sdebug << "  " << (*it)->name() << " -> " << flat_leaf.name() << " (double)" << endmsg;
        } else if ((*it)->type() == "Float_t") {
          ReducerLeaf<Float_t>& flat_leaf = CreateFloatLeaf((*it)->name()+"_flat");
          leaves_map_float_[&flat_leaf] = *it;
          sdebug << "  " << (*it)->name() << " -> " << flat_leaf.name() << " (float)" << endmsg;
        } else if ((*it)->type() == "Int_t") {
          ReducerLeaf<Int_t>& flat_leaf = CreateIntLeaf((*it)->name()+"_flat");
          leaves_map_int_[&flat_leaf] = *it;
          sdebug << "  " << (*it)->name() << " -> " << flat_leaf.name() << " (int)" << endmsg;
        }
      }
    }
  }
}
  
void ArrayFlattenerReducer::FillOutputTree() {
  if (leaf_array_length_ == NULL) {
    FlushEvent();
  } else {
    int num_pvs = leaf_array_length_->GetValue();
    for (int i=0; i<num_pvs; ++i) {
      // sdebug << "i = " << i << endmsg;
      *leaf_array_index_ = i;
      for (std::map<ReducerLeaf<Double_t>*, const ReducerLeaf<Float_t>*>::iterator it = leaves_map_double_.begin();
           it != leaves_map_double_.end(); ++it) {
        (*it->first) = it->second->GetValue(i);
        // sdebug << " " << it->first->name() << " = " << it->first->GetValue() << endmsg;
      }
      for (std::map<ReducerLeaf<Float_t>*, const ReducerLeaf<Float_t>*>::iterator it = leaves_map_float_.begin();
           it != leaves_map_float_.end(); ++it) {
        (*it->first) = it->second->GetValue(i);
        //sdebug << " " << it->first->name() << " = " << it->first->GetValue() << endmsg;
      }
      for (std::map<ReducerLeaf<Int_t>*, const ReducerLeaf<Float_t>*>::iterator it = leaves_map_int_.begin();
           it != leaves_map_int_.end(); ++it) {
        (*it->first) = it->second->GetValue(i);
        //sdebug << " " << it->first->name() << " = " << it->first->GetValue() << endmsg;
      }
      
      FlushEvent();
    }
  }
}
} // namespace reducer
} // namespace dooselection
