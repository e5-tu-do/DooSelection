#include "ShufflerReducer.h"

// from STL
#include <utility>
#include <vector>

// from ROOT
#include "TRandom3.h"

// from DooCore
#include <doocore/io/MsgStream.h>

using namespace doocore::io;

int ShufflerReducer::RegisterShuffler() {
  shufflers_.push_back(std::pair<int, ShuffleLeafVec>(0,ShuffleLeafVec()));
  return shufflers_.size()-1;
}

void ShufflerReducer::SetShuffleLeaves(int shuffler_idx, ReducerLeaf<Float_t>* new_leaf1, ReducerLeaf<Float_t>* new_leaf2,
                      const ReducerLeaf<Float_t>* base_leaf1, const ReducerLeaf<Float_t>* base_leaf2) {
  if (shuffler_idx >= shufflers_.size()) {
    serr << "Shuffler #" << shuffler_idx << " not registered!" << endmsg;
  } else {
    if (shufflers_[shuffler_idx].first != 0 && shufflers_[shuffler_idx].first != 2) {
      serr << "Cannot add elements to shuffle list. Number of elements to shuffle is: " << shufflers_[shuffler_idx].first << endmsg;
    } else {
      sinfo << "Shuffler #" << shuffler_idx << ": (" << base_leaf1->name() << ", " << base_leaf2->name() << ") -> (" << new_leaf1->name() << ", " << new_leaf2->name() << ")" << endmsg;
      
      shufflers_[shuffler_idx].first = 2;
      
      LeafPtrVec temp_vector_out;
      temp_vector_out.push_back(new_leaf1);
      temp_vector_out.push_back(new_leaf2);
      
      ConstLeafPtrVec temp_vector_base;
      temp_vector_base.push_back(base_leaf1);
      temp_vector_base.push_back(base_leaf2);
      
      shufflers_[shuffler_idx].second.push_back(std::make_pair(temp_vector_out, temp_vector_base));
    }
  }
}

void ShufflerReducer::SetShuffleLeaves(int shuffler_idx, ReducerLeaf<Float_t>* new_leaf1,
                      ReducerLeaf<Float_t>* new_leaf2,
                      ReducerLeaf<Float_t>* new_leaf3,
                      const ReducerLeaf<Float_t>* base_leaf1,
                      const ReducerLeaf<Float_t>* base_leaf2,
                      const ReducerLeaf<Float_t>* base_leaf3) {
  if (shuffler_idx >= shufflers_.size()) {
    serr << "Shuffler #" << shuffler_idx << " not registered!" << endmsg;
  } else {
    if (shufflers_[shuffler_idx].first != 0 && shufflers_[shuffler_idx].first != 3) {
      serr << "Cannot add elements to shuffle list. Number of elements to shuffle is: " << shufflers_[shuffler_idx].first << endmsg;
    } else {
      sinfo << "Shuffler #" << shuffler_idx << ": (" << base_leaf1->name() << ", " << base_leaf2->name() << ", " << base_leaf3->name() << ") -> (" << new_leaf1->name() << ", " << new_leaf2->name() << ", " << new_leaf3->name() << ")" << endmsg;
      
      shufflers_[shuffler_idx].first = 3;
      
      LeafPtrVec temp_vector_out;
      temp_vector_out.push_back(new_leaf1);
      temp_vector_out.push_back(new_leaf2);
      temp_vector_out.push_back(new_leaf3);
      
      ConstLeafPtrVec temp_vector_base;
      temp_vector_base.push_back(base_leaf1);
      temp_vector_base.push_back(base_leaf2);
      temp_vector_base.push_back(base_leaf3);
      
      shufflers_[shuffler_idx].second.push_back(std::make_pair(temp_vector_out, temp_vector_base));
    }
  }
}

void ShufflerReducer::UpdateSpecialLeaves() {
  int i = 0;
  
  for (std::vector<std::pair<int, ShuffleLeafVec> >::iterator it_shuffler = shufflers_.begin();
       it_shuffler != shufflers_.end(); ++it_shuffler) {
    int num_shuffle_elements      = it_shuffler->first;
    ShuffleLeafVec& shuffle_leafs = it_shuffler->second;
    
    if (shuffle_leafs.size() > 0) {
      // Fisher-Yates shuffle
      std::vector<int> new_order(num_shuffle_elements);
      new_order[0] = 0;
      int j;
      for (int i=1; i<num_shuffle_elements; ++i) {
        j = r_.Integer(i+1);
        new_order[i] = new_order[j];
        new_order[j] = i;
      }
      
      for (ShuffleLeafVec::iterator it = shuffle_leafs.begin();
           it != shuffle_leafs.end(); ++it) {
        for (int i=0; i<num_shuffle_elements; ++i) {
          *(it->first[i]) = it->second[new_order[i]]->GetValue();
        }
      }
    }
  }
}

