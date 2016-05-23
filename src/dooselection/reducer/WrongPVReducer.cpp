#include "WrongPVReducer.h"

namespace dooselection {
namespace reducer {

using namespace doocore::io;
using namespace dooselection::reducer;

//------------------------------------------------------------------------------
//                   WrongPVReducer::CreateSpecialBranches()
//------------------------------------------------------------------------------
void WrongPVReducer::CreateSpecialBranches(){
  // Create/Get new leaves
  chi2_any_leaf_  = &CreateDoubleLeaf(chi2_any_leaf_name_);
  chi2_leaf_      = &GetInterimLeafByName(chi2_leaf_name_);
  chi2_leaf_flat_ = &GetInterimLeafByName(chi2_leaf_name_+"_flat");
  idxPV_leaf_       = &GetInterimLeafByName(idxPV_leaf_name_);
  // Link branch addresses
  chi2_any_value_  = (Double_t*)chi2_any_leaf_->branch_address();
  chi2_value_      = (Float_t*)chi2_leaf_->branch_address();
  chi2_value_flat_ = (Float_t*)chi2_leaf_flat_->branch_address();
  idxPV_value_       = (Int_t*)idxPV_leaf_->branch_address();

  pv_x_leaf_ = &GetInterimLeafByName(pv_x_leaf_name_);
  pv_y_leaf_ = &GetInterimLeafByName(pv_y_leaf_name_);
  pv_z_leaf_ = &GetInterimLeafByName(pv_z_leaf_name_);
 
  pv_x_var_leaf_ = &GetInterimLeafByName(pv_x_var_leaf_name_);
  pv_y_var_leaf_ = &GetInterimLeafByName(pv_x_var_leaf_name_);
  pv_z_var_leaf_ = &GetInterimLeafByName(pv_x_var_leaf_name_);

  if(LeafExists("B0_BKGCAT")) {
    std::cout << "This is an MC Sample " << std::endl;
    pv_x_true_leaf_ = &GetInterimLeafByName(pv_x_true_leaf_name_);
    pv_y_true_leaf_ = &GetInterimLeafByName(pv_y_true_leaf_name_);
    pv_z_true_leaf_ = &GetInterimLeafByName(pv_z_true_leaf_name_);
    pv_x_res_leaf_ = &CreateDoubleLeaf(pv_x_res_leaf_name_);
    pv_y_res_leaf_ = &CreateDoubleLeaf(pv_y_res_leaf_name_);
    pv_z_res_leaf_ = &CreateDoubleLeaf(pv_z_res_leaf_name_);
    pv_x_pull_leaf_ = &CreateDoubleLeaf(pv_x_pull_leaf_name_);
    pv_y_pull_leaf_ = &CreateDoubleLeaf(pv_y_pull_leaf_name_);
    pv_z_pull_leaf_ = &CreateDoubleLeaf(pv_z_pull_leaf_name_);
    pv_x_res_value_ = (Double_t*)pv_x_res_leaf_->branch_address();
    pv_y_res_value_ = (Double_t*)pv_y_res_leaf_->branch_address();
    pv_z_res_value_ = (Double_t*)pv_z_res_leaf_->branch_address();
    pv_x_pull_value_ = (Double_t*)pv_x_pull_leaf_->branch_address();
    pv_y_pull_value_ = (Double_t*)pv_y_pull_leaf_->branch_address();
    pv_z_pull_value_ = (Double_t*)pv_z_pull_leaf_->branch_address();
  } else {
    sinfo << "This is a Data Sample." << endmsg;
  }
}

//------------------------------------------------------------------------------
//                   WrongPVReducer::EntryPassesSpecialCuts()
//------------------------------------------------------------------------------
bool WrongPVReducer::EntryPassesSpecialCuts() { return true; }

//------------------------------------------------------------------------------
//                   WrongPVReducer::UpdateSpecialLeaves()
//------------------------------------------------------------------------------
void WrongPVReducer::UpdateSpecialLeaves() {
  unsigned int nPV = chi2_any_leaf_->Length();
  unsigned int idxPV = *idxPV_value_;
  double ip_chi2 = *chi2_value_flat_;

  // if (debug_mode_) sinfo << "Number of PVs in event: " << nPV << ", PV index of this (B, PV)-pair: " << idxPV << " with an IP chi2: " << ip_chi2 << endmsg;

  // sanity check if IP chi2 is smaller 0, set to -1
  if (ip_chi2 < 0){
    *chi2_any_value_ = -1;
    swarn << "IP chi2 < 0!" << endmsg;
  }
  // if number of PVs is one, set to default value 1e+6
  else if (nPV == 1){
    *chi2_any_value_ = 1e+12;
    // if (debug_mode_) sinfo << "Number of PVs in event: " << nPV << ". chi2_any_value is set to 1e+12." << endmsg;
  }
  else {
    // start with a reference IP of 1e+12, this also defines the larges possible value the variable can take!
    double min_ip_chi2 = 1e+12;
    if (debug_mode_) sinfo.increment_indent(5);
    for (unsigned int pv = 0; pv < nPV; pv++){

      if (pv != idxPV && debug_mode_) sinfo << "IP chi2 for PV No. " << pv << " is: " << chi2_any_leaf_->GetValue(pv) << endmsg;
      
      if (pv != idxPV && chi2_any_leaf_->GetValue(pv) > 0 && (min_ip_chi2 <= 0 || chi2_any_leaf_->GetValue(pv) < min_ip_chi2) ){
        min_ip_chi2 = chi2_any_leaf_->GetValue(pv);
        sinfo << "Min IP to any other PV is " << min_ip_chi2 << endmsg;
      }
    }
    if (debug_mode_) sinfo.increment_indent(-5);
    if (debug_mode_) sinfo << "Saved min IP chi2: " << min_ip_chi2 << endmsg;
    *chi2_any_value_ = min_ip_chi2;
  }

  if(pv_z_pull_leaf_) {
    *pv_x_res_leaf_ = pv_x_leaf_->GetValue() - pv_x_true_leaf_->GetValue();
    *pv_y_res_leaf_ = pv_y_leaf_->GetValue() - pv_y_true_leaf_->GetValue();
    *pv_z_res_leaf_ = pv_z_leaf_->GetValue() - pv_z_true_leaf_->GetValue();
    *pv_x_pull_value_ = pv_x_res_leaf_->GetValue()/sqrt(pv_x_var_leaf_->GetValue());
    *pv_y_pull_value_ = pv_y_res_leaf_->GetValue()/sqrt(pv_y_var_leaf_->GetValue());
    *pv_z_pull_value_ = pv_z_res_leaf_->GetValue()/sqrt(pv_z_var_leaf_->GetValue()); 
  }
}

} // namespace reducer
} // namespace dooselection
