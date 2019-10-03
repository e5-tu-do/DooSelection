#include "VariableCategorizerReducer.h"
#include "doocore/io/Progress.h"

// from ROOT
#include "TMath.h"

// from BOOST
#include "boost/lexical_cast.hpp"

namespace dooselection {
namespace reducer {

VariableCategorizerReducer::VariableCategorizerReducer(const std::string& prefix_name):
  prefix_name_(prefix_name),
  variables_()
{}

void VariableCategorizerReducer::set_variable(std::string variable_name, int nbins, double range_min, double range_max){

  // else{
    std::vector<double> variable_quantiles;
    std::vector<double> variable_data_points;
    /// vector containing a tuple with the following entries:
    /// 0 variable_name 
    /// 1 nbins 
    /// 2 range min 
    /// 3 range max 
    /// 4 vector of quantiles 
    /// 5 vector of data points 
    /// 6 int pointer to variable category 
    /// 7 double pointer to variable value 
    /// 8 reducer leaf pointer to variable category leaf
    int* int_ptr=NULL; Double_t* double_ptr=NULL; dooselection::reducer::ReducerLeaf<Int_t>* leaf_ptr=NULL;
    auto t = std::make_tuple(variable_name, nbins, range_min, range_max, variable_quantiles, variable_data_points, int_ptr, double_ptr, leaf_ptr);
    variables_.push_back(t);
    doocore::io::sinfo << "-info-  \t" << "VariableCategorizerReducer \t" << "Added variable '" << variable_name << "' to list of variables to categorize" << " (" << nbins << " bins in range " << range_min << " - " << range_max << ")." << doocore::io::endmsg;
  // }
}

void VariableCategorizerReducer::CreateSpecialBranches(){
  for (auto& variable: variables_){
    std::string variable_name = std::get<0>(variable);
    if (!LeafExists(variable_name)){
      doocore::io::serr << "-ERROR- \t" << "VariableCategorizerReducer \t" << "The variable '" << variable_name << "' does not exist in this tree!" << doocore::io::endmsg;
      continue;
    }
    int variable_binning = std::get<1>(variable);

    dooselection::reducer::ReducerLeaf<Int_t>* variable_category_leaf = &(CreateIntLeaf(prefix_name_+std::to_string(variable_binning)+"_"+variable_name, prefix_name_+std::to_string(variable_binning)+"_"+variable_name, "Int_t", -1));

    std::get<6>(variable) = (Int_t*)variable_category_leaf->branch_address();
    std::get<7>(variable) = (Double_t*)GetInterimLeafByName(variable_name).branch_address();
    std::get<8>(variable) = variable_category_leaf;
  }
}

void VariableCategorizerReducer::PrepareSpecialBranches(){
  int nevents = interim_tree_->GetEntries();
  for (auto& variable: variables_){
    std::string variable_name = std::get<0>(variable);
    unsigned int variable_binning = std::get<1>(variable);
    double variable_range_min = std::get<2>(variable);
    double variable_range_max = std::get<3>(variable);
    std::vector<double> data_points;

    doocore::io::sinfo << "-info-  \t" << "VariableCategorizerReducer \t" << "Computing p-quantiles for " << variable_name  << " (" << variable_binning << " bins, from " << variable_range_min << " to " << variable_range_max << ")." << doocore::io::endmsg;
    interim_tree_->SetBranchStatus("*", false);
    interim_tree_->SetBranchStatus(variable_name.c_str(), true);
    double value;
    doocore::io::Progress p("", nevents);
    for (Int_t ev = 0; ev < nevents; ev++){
      interim_tree_->GetEvent(ev);
      value = *std::get<7>(variable);
      if ((value > variable_range_min) && (value < variable_range_max)) data_points.push_back(value);
      ++p;
    }
    p.Finish();
    interim_tree_->SetBranchStatus("*", true);
    sort(data_points.begin(), data_points.end());

    std::vector<double> probabilities;
    for (unsigned int i = 1; i < variable_binning; i++) {
      probabilities.push_back(1.*i/variable_binning);
    }

    std::vector<double> quantiles(variable_binning+1,0);
    quantiles.front() = variable_range_min;
    quantiles.back() = variable_range_max;

    TMath::Quantiles(data_points.size(), variable_binning-1, &data_points[0], &quantiles[1], &probabilities[0]);

    // print out all quantiles
    doocore::io::sinfo << "-info-  \t" << "VariableCategorizerReducer \t" << "The calculated quantiles are:" << doocore::io::endmsg;
    for(std::vector<double>::const_iterator it = quantiles.begin(); it != quantiles.end(); it++){
      doocore::io::sinfo << *it << doocore::io::endmsg;
    }

    // now compute the weighted bin center for each quantile
    std::vector<double> weighted_bin_centers;
    std::vector<double>::const_iterator it_quantiles = quantiles.begin();
    it_quantiles++; // jump over greatest lower bound
    double count = 0;
    double sum = 0;
    for (auto data_point : data_points){
      if (data_point < *it_quantiles){
        count++;
        sum += data_point;
      }
      else{
        weighted_bin_centers.push_back(sum/count);
        count = 1;
        sum =  data_point;
        it_quantiles++;
      }
    }
    weighted_bin_centers.push_back(sum/count);

    doocore::io::sinfo << "-info-  \t" << "VariableCategorizerReducer \t" << "The weighted bin centers are:" << doocore::io::endmsg;
    for(std::vector<double>::const_iterator it = weighted_bin_centers.begin(); it != weighted_bin_centers.end(); it++){
      doocore::io::sinfo << *it << doocore::io::endmsg;
    }

    std::get<4>(variable) = quantiles;
    std::get<5>(variable) = data_points;
  }
}

bool VariableCategorizerReducer::EntryPassesSpecialCuts(){return true;}

void VariableCategorizerReducer::UpdateSpecialLeaves(){
  for (auto& variable: variables_){
    std::string variable_name = std::get<0>(variable);
//    unsigned int variable_binning = std::get<1>(variable);
    double variable_range_min = std::get<2>(variable);
    double variable_range_max = std::get<3>(variable);
    std::vector<double> quantiles = std::get<4>(variable);
    unsigned int nquantiles = quantiles.size();

    int category = -1;
    for (unsigned int i=0; i<nquantiles; i++){
      double variable_value = *std::get<7>(variable);
      if (variable_value < variable_range_min){
        category = 0; // underflow bin
        break;
      }
      else if (variable_value > variable_range_max){
        category = nquantiles; // overflow bin
        break;
      }
      else if ( (variable_value >= quantiles[i]) && (variable_value < quantiles[i+1]) ){
        category = i+1; // regular bins
        break;
      }
    }
    *std::get<6>(variable)=category;
  }
}

} // namespace reducer
} // namespace bd2jpsiks
