#include "MomentumArrayFlattenerReducer.h"

// from STL
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

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
  
MomentumArrayFlattenerReducer::MomentumArrayFlattenerReducer() {}
  
void MomentumArrayFlattenerReducer::PrepareSpecialBranches() {
  if (threemomentum_array_leaves_.size() == 0 && fourmomentum_array_leaves_.size() == 0) {
    sinfo << "MomentumArrayFlattenerReducer: No momentum leafs specified. Will not create new branches." << endmsg;
  } else {
    for (std::vector<ReducerLeaf<Float_t>* >::const_iterator it = GetInterimLeavesBegin(); it != GetInterimLeavesEnd(); ++it) {
      if (threemomentum_array_leaves_.size() > 0) {
        for (std::vector<const ReducerLeaf<Float_t>*>::const_iterator it_three=threemomentum_array_leaves_.begin(), end=threemomentum_array_leaves_.end(); it_three != end; ++it_three) {
          if ((*it)->Length() == 3 && (*it)->name().EqualTo((*it_three)->name())) {
            if ((*it)->type() == "Double_t") {
              ReducerLeaf<Double_t>& x_leaf = CreateDoubleLeaf((*it)->name()+"_x");
              ReducerLeaf<Double_t>& y_leaf = CreateDoubleLeaf((*it)->name()+"_y");
              ReducerLeaf<Double_t>& z_leaf = CreateDoubleLeaf((*it)->name()+"_z");
              three_leaves_map_double_[&x_leaf] = *it;
              three_leaves_map_double_[&y_leaf] = *it;
              three_leaves_map_double_[&z_leaf] = *it;
              // sdebug << "  " << (*it)->name() << " -> " << x_leaf.name() << " (double)" << endmsg;
            } else if ((*it)->type() == "Float_t") {
              ReducerLeaf<Float_t>& x_leaf = CreateFloatLeaf((*it)->name()+"_x");
              ReducerLeaf<Float_t>& y_leaf = CreateFloatLeaf((*it)->name()+"_y");
              ReducerLeaf<Float_t>& z_leaf = CreateFloatLeaf((*it)->name()+"_z");
              three_leaves_map_float_[&x_leaf] = *it;
              three_leaves_map_float_[&y_leaf] = *it;
              three_leaves_map_float_[&z_leaf] = *it;
              // sdebug << "  " << (*it)->name() << " -> " << x_leaf.name() << " (float)" << endmsg;
            } else if ((*it)->type() == "Int_t") {
              ReducerLeaf<Int_t>& x_leaf = CreateIntLeaf((*it)->name()+"_x");
              ReducerLeaf<Int_t>& y_leaf = CreateIntLeaf((*it)->name()+"_y");
              ReducerLeaf<Int_t>& z_leaf = CreateIntLeaf((*it)->name()+"_z");
              three_leaves_map_int_[&x_leaf] = *it;
              three_leaves_map_int_[&y_leaf] = *it;
              three_leaves_map_int_[&z_leaf] = *it;
              // sdebug << "  " << (*it)->name() << " -> " << x_leaf.name() << " (int)" << endmsg;
            }
          } else if ((*it)->name().EqualTo((*it_three)->name())) {
            sinfo << "MomentumArrayFlattenerReducer: The branch " << (*it)->name() << " has not length three. Will ignore." << endmsg;
          }
        }
      }
      if (fourmomentum_array_leaves_.size() > 0) {
        for (std::vector<const ReducerLeaf<Float_t>*>::const_iterator it_four=fourmomentum_array_leaves_.begin(), end=fourmomentum_array_leaves_.end(); it_four != end; ++it_four) {
          if ((*it)->Length() == 4 && (*it)->name().EqualTo((*it_four)->name())) {
            if ((*it)->type() == "Double_t") {
              ReducerLeaf<Double_t>& x_leaf = CreateDoubleLeaf((*it)->name()+"_x");
              ReducerLeaf<Double_t>& y_leaf = CreateDoubleLeaf((*it)->name()+"_y");
              ReducerLeaf<Double_t>& z_leaf = CreateDoubleLeaf((*it)->name()+"_z");
              ReducerLeaf<Double_t>& E_leaf = CreateDoubleLeaf((*it)->name()+"_E");
              four_leaves_map_double_[&E_leaf] = *it;
              four_leaves_map_double_[&x_leaf] = *it;
              four_leaves_map_double_[&y_leaf] = *it;
              four_leaves_map_double_[&z_leaf] = *it;
              // sdebug << "  " << (*it)->name() << " -> " << E_leaf.name() << " (double)" << endmsg;
            } else if ((*it)->type() == "Float_t") {
              ReducerLeaf<Float_t>& x_leaf = CreateFloatLeaf((*it)->name()+"_x");
              ReducerLeaf<Float_t>& y_leaf = CreateFloatLeaf((*it)->name()+"_y");
              ReducerLeaf<Float_t>& z_leaf = CreateFloatLeaf((*it)->name()+"_z");
              ReducerLeaf<Float_t>& E_leaf = CreateFloatLeaf((*it)->name()+"_E");
              four_leaves_map_float_[&E_leaf] = *it;
              four_leaves_map_float_[&x_leaf] = *it;
              four_leaves_map_float_[&y_leaf] = *it;
              four_leaves_map_float_[&z_leaf] = *it;
              // sdebug << "  " << (*it)->name() << " -> " << E_leaf.name() << " (float)" << endmsg;
            } else if ((*it)->type() == "Int_t") {
              ReducerLeaf<Int_t>& x_leaf = CreateIntLeaf((*it)->name()+"_x");
              ReducerLeaf<Int_t>& y_leaf = CreateIntLeaf((*it)->name()+"_y");
              ReducerLeaf<Int_t>& z_leaf = CreateIntLeaf((*it)->name()+"_z");
              ReducerLeaf<Int_t>& E_leaf = CreateIntLeaf((*it)->name()+"_E");
              four_leaves_map_int_[&E_leaf] = *it;
              four_leaves_map_int_[&x_leaf] = *it;
              four_leaves_map_int_[&y_leaf] = *it;
              four_leaves_map_int_[&z_leaf] = *it;
              // sdebug << "  " << (*it)->name() << " -> " << E_leaf.name() << " (int)" << endmsg;
            }
          } else if ((*it)->name().EqualTo((*it_four)->name())) {
            sinfo << "MomentumArrayFlattenerReducer: The branch " << (*it)->name() << " has length of " << (*it)->Length() << " and not of 4. Will ignore." << endmsg;
          }
        }
      }
    }
  }
}
  
bool MomentumArrayFlattenerReducer::FlatLeavesPassSpecialCuts() {
  return true;
}

void MomentumArrayFlattenerReducer::FillOutputTree() {
  if (threemomentum_array_leaves_.size() == 0 && fourmomentum_array_leaves_.size() == 0) {
    if (FlatLeavesPassSpecialCuts()) {
      FlushEvent();
    }
  } else {
    if (threemomentum_array_leaves_.size() > 0) {
      int i = 0;
      for (std::map<ReducerLeaf<Double_t>*, const ReducerLeaf<Float_t>*>::iterator it = three_leaves_map_double_.begin(); it != three_leaves_map_double_.end(); ++it) {
        (*it->first) = it->second->GetValue(i);
        // sdebug << " " << it->first->name() << " = " << it->first->GetValue() << endmsg;
        i = (i+1)%3;
      }
      for (std::map<ReducerLeaf<Float_t>*, const ReducerLeaf<Float_t>*>::iterator it = three_leaves_map_float_.begin(); it != three_leaves_map_float_.end(); ++it) {
        (*it->first) = it->second->GetValue(i);
        // sdebug << " " << it->first->name() << " = " << it->first->GetValue() << endmsg;
        i = (i+1)%3; 
      }
      for (std::map<ReducerLeaf<Int_t>*, const ReducerLeaf<Float_t>*>::iterator it = three_leaves_map_int_.begin(); it != three_leaves_map_int_.end(); ++it) {
        (*it->first) = it->second->GetValue(i);
        // sdebug << " " << it->first->name() << " = " << it->first->GetValue() << endmsg;
        i = (i+1)%3;
      }
    }
    if (fourmomentum_array_leaves_.size() > 0) {
      int i=0;
      for (std::map<ReducerLeaf<Double_t>*, const ReducerLeaf<Float_t>*>::iterator it = four_leaves_map_double_.begin(); it != four_leaves_map_double_.end(); ++it) {
        (*it->first) = it->second->GetValue(i);
        // sdebug << " " << it->first->name() << " = " << it->first->GetValue() << endmsg;
        i = (i+1)%4;
      }
      for (std::map<ReducerLeaf<Float_t>*, const ReducerLeaf<Float_t>*>::iterator it = four_leaves_map_float_.begin(); it != four_leaves_map_float_.end(); ++it) {
        (*it->first) = it->second->GetValue(i);
        // sdebug << i << ": " << it->first->name() << " = " << it->first->GetValue() << endmsg;
        i = (i+1)%4;
      }
      for (std::map<ReducerLeaf<Int_t>*, const ReducerLeaf<Float_t>*>::iterator it = four_leaves_map_int_.begin(); it != four_leaves_map_int_.end(); ++it) {
        (*it->first) = it->second->GetValue(i);
        // sdebug << " " << it->first->name() << " = " << it->first->GetValue() << endmsg;
        i = (i+1)%4;
      }
    }
    if(FlatLeavesPassSpecialCuts()) {
      FlushEvent();
    } else {
      // skip this index
    }
  }
}
} // namespace reducer
} // namespace dooselection
