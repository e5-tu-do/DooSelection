#include "ArrayFlattenerReducer.h"

// from STL
#include <string>
#include <vector>
#include <utility>

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
  : name_array_index_("flat_array_index"),
  leaves_array_length_()
{}
  
void ArrayFlattenerReducer::PrepareSpecialBranches() {
  if (leaves_array_length_.size() == 0) {
    sinfo << "ArrayFlattenerReducer: Array length leaf not set. Will not flatten." << endmsg;
  } else {
    
    leaf_array_length_ = leaves_array_length_.front();
    
    for (std::vector<const ReducerLeaf<Float_t>*>::const_iterator it=leaves_array_length_.begin(), end=leaves_array_length_.end(); it != end; ++it) {
      std::string name_array_length((*it)->name());
      sinfo << "ArrayFlattenerReducer: Flattening all leaves that have array size " << name_array_length << endmsg;
    }
    
    if (leaves_array_length_.size() > 1) {
      sinfo << "ArrayFlattenerReducer: More than one length leaf set. Checking for consistency on a small sample of events." << endmsg;
      int stepping_check = interim_tree_->GetEntries()/1000;
      std::vector<std::pair<TBranch*,TLeaf*> > branches_array_length;
      
      for (std::vector<const ReducerLeaf<Float_t>*>::const_iterator it=leaves_array_length_.begin(), end=leaves_array_length_.end(); it != end; ++it) {
        branches_array_length.push_back(std::make_pair(interim_tree_->GetBranch((*it)->name()),
                                                       interim_tree_->GetLeaf((*it)->name())));
      }
      
      double leaf_value = 0.0;
      for (int i=0; i<interim_tree_->GetEntries(); i+=stepping_check) {
        for (std::vector<std::pair<TBranch*,TLeaf*> >::const_iterator it=branches_array_length.begin(), end=branches_array_length.end(); it != end; ++it) {
          it->first->GetEvent(i);
          
          if (it == branches_array_length.begin()) {
            leaf_value = it->second->GetValue();
          } else {
            if (it->second->GetValue() != leaf_value) {
              serr << "ArrayFlattenerReducer: For event #" << i << " leaf " << it->second->GetName() << " does not have equal content as other array length leaves. This will break array flattening! Continue at own risk." << endmsg;
              throw 1;
            }
          }
        }
      }
    }
    
//    std::string name_array_length(leaf_array_length_->name());
//    std::string name_array_index = name_array_length + "_index";
    leaf_array_index_ = &CreateIntLeaf(name_array_index_);
    sinfo << "Array index will be written into: " << name_array_index_ << endmsg;
    
    sinfo << "The following leaves will be flattened: " << endmsg;
    // loop over all interim leaves and check if array length is name_array_length_
    // fill leaves_map_double_ and leaves_map_int_ based on leaf_array_length_
    for (std::vector<ReducerLeaf<Float_t>* >::const_iterator it = GetInterimLeavesBegin(); it != GetInterimLeavesEnd(); ++it) {
      
      // iterate over all set length leaves and check if interim leaf is matching
      for (std::vector<const ReducerLeaf<Float_t>*>::const_iterator it_lengths=leaves_array_length_.begin(), end=leaves_array_length_.end(); it_lengths != end; ++it_lengths) {
        std::string name_array_length((*it_lengths)->name());
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
}
  
void ArrayFlattenerReducer::FillOutputTree() {
  if (leaves_array_length_.size() == 0) {
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
