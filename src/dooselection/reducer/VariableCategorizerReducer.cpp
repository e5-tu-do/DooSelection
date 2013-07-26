#include "VariableCategorizerReducer.h"

// from ROOT
#include "TMath.h"

namespace dooselection {
namespace reducer {

using namespace dooselection::reducer;

VariableCategorizerReducer::VariableCategorizerReducer(const std::string& variable_name, const std::string& prefix_name, int nbins, double range_min, double range_max):
  variable_name_(variable_name),
  prefix_name_(prefix_name),
  nbins_(nbins),
  range_min_(range_min),
  range_max_(range_max),
  variable_category_(NULL),
  variable_category_leaf_(NULL),
  variable_(NULL)
{
  if (!LeafExists(variable_name_)){
    doocore::io::serr << "-ERROR- \t" << "VariableCategorizerReducer \t" << "The variable '" << variable_name_ << "' does not exist in this tree!" << doocore::io::endmsg;
    assert(0);
  }
}

void VariableCategorizerReducer::CreateSpecialBranches(){
  variable_category_leaf_ = &(CreateIntLeaf(prefix_name_+"_"+variable_name_, prefix_name_+"_"+variable_name_, "Int_t", -1));
  variable_category_  = (Int_t*)variable_category_leaf_->branch_address();

  variable_  = (Double_t*)GetInterimLeafByName(variable_name_).branch_address();
}

void VariableCategorizerReducer::PrepareSpecialBranches(){
  interim_tree_->SetBranchStatus("*", false);
  interim_tree_->SetBranchStatus(variable_name_.c_str(), true);

  int nevents = interim_tree_->GetEntries();

  doocore::io::sinfo << "Computing p-quantiles for " << variable_name_  << doocore::io::endmsg;
  for (Int_t ev = 0; ev < nevents; ev++){
    double frac = static_cast<double>(ev)/nevents;
    if ( (ev%100) == 0 ) printf("Progress %.2f % \xd", frac*100.0);
    interim_tree_->GetEvent(ev);
    if ((*variable_ > range_min_) && (*variable_ < range_max_)) data_points_.push_back(*variable_);
    fflush(stdout);
  }
  interim_tree_->SetBranchStatus("*", true);

  sort(data_points_.begin(), data_points_.end());

  std::vector<double> probabilities;
  for (unsigned int i = 1; i < nbins_; i++) {
    probabilities.push_back(1.*i/nbins_);
  }

  std::vector<double> quantiles(nbins_+1,0);
  quantiles.front() = range_min_;
  quantiles.back() = range_max_;

  TMath::Quantiles(data_points_.size(), nbins_-1, &data_points_[0], &quantiles[1], &probabilities[0]);

  doocore::io::sinfo << "Vector of p-quantiles" << doocore::io::endmsg;
  for(std::vector<double>::const_iterator it = quantiles.begin(); it != quantiles.end(); it++){
    doocore::io::sinfo << *it << doocore::io::endmsg;
  }

  quantiles_ = quantiles;
}

bool VariableCategorizerReducer::EntryPassesSpecialCuts(){return true;}

void VariableCategorizerReducer::UpdateSpecialLeaves(){
  int category = -1;
  int nquantiles = quantiles_.size();
  for (unsigned int i=0; i<nquantiles; i++){
    if ((*variable_) < range_min_){
      category = 0; // underflow bin
      break;
    }
    else if ((*variable_) > range_max_){
      category = nquantiles; // overflow bin
      break;
    }
    else if ( ((*variable_) >= quantiles_[i]) && ((*variable_) < quantiles_[i+1]) ){
      category = i+1; // regular bins
      break;
    }
  }
  *variable_category_=category;  
}

} // namespace reducer
} // namespace bd2jpsiks