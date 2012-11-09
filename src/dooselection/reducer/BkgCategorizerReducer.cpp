#include "BkgCategorizerReducer.h"

// from STL
#include <string>
#include <map>
#include <utility>

// from ROOT
#include "TMath.h"
#include "TCut.h"

// from DooCore
#include <doocore/io/MsgStream.h>

// From project
#include "dooselection/mctools/idtranslator/IDTranslator.h"

using std::pair;
using namespace doocore::io;


namespace dooselection {
namespace reducer {

BkgCategorizerReducer::BkgCategorizerReducer() : 
  background_category_leaf_(NULL),
  background_category_lptr_(NULL),
  decay_depth_leaf_(NULL),
  decay_counter_(),
  max_number_decays_(40) 
{}

BkgCategorizerReducer::~BkgCategorizerReducer() {}

void BkgCategorizerReducer::PrepareSpecialBranches() {
  std::string decaystring = "";
  
  sinfo << "Starting analysis of MC associated decays." << endmsg;
  
  TBranch* br_matrix = interim_tree_->GetBranch(decay_matrix_name_.c_str());
  if (br_matrix != NULL) {
    br_matrix->SetAddress(&decay_matrix_);    
    TBranch* br_depth  = interim_tree_->GetBranch(decay_depth_leaf_->name());
        
    // Fill the map with decay-strings
    for (Int_t ev = 0; ev < interim_tree_->GetEntries(); ev++) {
      interim_tree_->GetEntry(ev);
      if (!(decay_depth_leaf_->GetValue() < 1)){
        std::string decay_string = IDTranslator::makedecaystring(decay_matrix_, rows_, columns_, 0, 0, decaystring, true, true);
        std::map<std::string,int>::const_iterator iter = decay_counter_.find(decay_string);

        if (iter == decay_counter_.end()) {
          decay_counter_.insert (pair<std::string,int>(decay_string,1));
        } else {
          decay_counter_[decay_string]++;
        }
      }
      decaystring = "";
    }
    
    std::map<std::string,int>::const_iterator iter; 
    
    sinfo << "Finished analysing most common decays: " << endmsg;
    for (int i=0; i< max_number_decays_ && i<decay_counter_.size(); ++i) {
      std::string max_key = "";
      int max_counts = 0;
      for (iter=decay_counter_.begin(); iter!=decay_counter_.end(); iter++) {
        if (iter->second > max_counts) {
          max_counts = iter->second;
          max_key    = iter->first;
        }
      }
      decay_vector_.push_back(std::pair<int,std::string>(decay_counter_[max_key],max_key));
      sinfo << "Number: " << i+1 << ", Decay: " << decay_vector_.at(i).second << ", Count: " << decay_vector_.at(i).first << endmsg;
      decay_counter_.erase(max_key);  
    }
    
  background_category_lptr_ = (Int_t*)background_category_leaf_->branch_address();
  
  sinfo << "Background categories according to this table will be put into leaf " << background_category_leaf_->name() << endmsg;
  } else {
    swarn << "Decay matrix not found. Skipping analysis." << endmsg;
  }
    
  PrepareFurtherSpecialLeaves();
}

void BkgCategorizerReducer::UpdateSpecialLeaves() {
  if (decay_depth_leaf_ != NULL) {
    if (!(decay_depth_leaf_->GetValue() < 1)) {
      std::string decaystring = IDTranslator::makedecaystring(decay_matrix_, rows_, columns_, 0, 0, "", true, true);
      int j = 0;
      while(j < max_number_decays_ && j < decay_vector_.size() && decaystring != decay_vector_.at(j).second){
        j++;
      }
      if (j==max_number_decays_){
        *background_category_lptr_ = 50;
      }
      else {
        *background_category_lptr_ = j+1;
      }
    } else {
      *background_category_lptr_ = 100;
    }
  }
  UpdateFurtherSpecialLeaves();
}

} // namespace reducer
} // namespace dooselection

