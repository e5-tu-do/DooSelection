#include "WrongPVReducer.h"

namespace dooselection {
namespace reducer {

using namespace dooselection::reducer;

//------------------------------------------------------------------------------
//                   WrongPVReducer::CreateSpecialBranches()
//------------------------------------------------------------------------------
void WrongPVReducer::CreateSpecialBranches(){
  // Create/Get new leaves
  out_leaf_         = &CreateDoubleLeaf(out_leaf_name_);
  in_leaf_          = &GetInterimLeafByName(in_leaf_name_);
  in_leaf_flat_     = &GetInterimLeafByName(in_leaf_name_+"_flat");
  in_leaf_idx_pv_   = &GetInterimLeafByName(in_leaf_idx_pv_name_);
  // Link branch addresses
  out_value_        = (Double_t*)out_leaf_->branch_address();
  in_value_         = (Float_t*)in_leaf_->branch_address();
  in_value_flat_    = (Float_t*)in_leaf_flat_->branch_address();
  in_value_idx_pv_  = (Int_t*)in_leaf_idx_pv_->branch_address();

  if (LeafExists("B0_FitPVConst_PV_X_flat")) {
    pv_x_ = &GetInterimLeafByName("B0_FitPVConst_PV_X_flat");
    pv_y_ = &GetInterimLeafByName("B0_FitPVConst_PV_Y_flat");
    pv_z_ = &GetInterimLeafByName("B0_FitPVConst_PV_Z_flat");
  }
  if (LeafExists("B0_FitPVConst_PV_XVAR_flat")) {
    pv_x_var_ = &GetInterimLeafByName("B0_FitPVConst_PV_XVAR_flat");
    pv_y_var_ = &GetInterimLeafByName("B0_FitPVConst_PV_YVAR_flat");
    pv_z_var_ = &GetInterimLeafByName("B0_FitPVConst_PV_ZVAR_flat");
  }
  if (LeafExists("B0_TRUEORIGINVERTEX_X")) {
    pv_true_x_ = &GetInterimLeafByName("B0_TRUEORIGINVERTEX_X");
    pv_true_y_ = &GetInterimLeafByName("B0_TRUEORIGINVERTEX_Y");
    pv_true_z_ = &GetInterimLeafByName("B0_TRUEORIGINVERTEX_Z");
  }
  if (LeafExists("J_psi_1S_MINIPCHI2_OtherPVs_flat")) {
    comparison_leaf_ = &GetInterimLeafByName("J_psi_1S_MINIPCHI2_OtherPVs_flat");
    comparison_value_ = (Float_t*)comparison_leaf_->branch_address();
  }

  if(LeafExists("B0_BKGCAT")) {
    std::cout << "This is an MC Sample " << std::endl;
    if(LeafExists("B0_FitPVConst_PV_ZVAR_flat")){
      sinfo << "B0_FitPVConst_PV_ZVAR_flat" << endmsg;
      pv_res_x_ = &CreateDoubleLeaf("pv_res_x");
      pv_res_y_ = &CreateDoubleLeaf("pv_res_y");
      pv_res_z_ = &CreateDoubleLeaf("pv_res_z");
      pv_pull_x_ = &CreateDoubleLeaf("pv_pull_x");
      pv_pull_y_ = &CreateDoubleLeaf("pv_pull_y");
      pv_pull_z_ = &CreateDoubleLeaf("pv_pull_z");
      pv_res_x_val_ = (Double_t*)pv_res_x_->branch_address();
      pv_res_y_val_ = (Double_t*)pv_res_y_->branch_address();
      pv_res_z_val_ = (Double_t*)pv_res_z_->branch_address();
      pv_pull_x_val_ = (Double_t*)pv_pull_x_->branch_address();
      pv_pull_y_val_ = (Double_t*)pv_pull_y_->branch_address();
      pv_pull_z_val_ = (Double_t*)pv_pull_z_->branch_address();
      //Filled in UpdateSpecialBranches
    } else {
      sinfo << "B0_FitPVConst_PV_ZVAR_flat" << endmsg;
    }
  } else {
    sinfo << "This is a Data Sample" << endmsg;
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
  unsigned int nPV = in_leaf_->Length();
  unsigned int idxPV = *in_value_idx_pv_;
  double ip_chi2 = *in_value_flat_;

  if (debug_mode_) sinfo << "Number of PVs in event: " << nPV << ", PV index of this (B, PV)-pair: " << idxPV << " with an IP chi2: " << ip_chi2 << endmsg;

  // sanity check if IP chi2 is smaller 0, set to -1
  if (ip_chi2 < 0){
    *out_value_ = -1;
    swarn << "IP chi2 <0!" << endmsg;
  }
  // if number of PVs is one, set to default value 1e+6
  else if (nPV == 1){
    *out_value_ = 1e+12;
  }
  else {
    // start with a reference IP of 1e+12, this also defines the larges possible value the variable can take!
    double min_ip_chi2 = 1e+12;
    if (debug_mode_) sinfo.increment_indent(5);
    for (unsigned int pv = 0; pv < nPV; pv++){

      if (pv != idxPV && debug_mode_) sinfo << "IP chi2 for PV No. " << pv << " is: " << in_leaf_->GetValue(pv) << endmsg;
      
      if (pv != idxPV && in_leaf_->GetValue(pv) > 0 && (min_ip_chi2 <= 0 || in_leaf_->GetValue(pv) < min_ip_chi2) ){
        min_ip_chi2 = in_leaf_->GetValue(pv);
      }
    }
    if (debug_mode_) sinfo.increment_indent(-5);
    if (debug_mode_) sinfo << "Saved min IP chi2: " << min_ip_chi2 << endmsg;
    *out_value_ = min_ip_chi2;
  }

  if(pv_pull_z_) {
    *pv_res_x_ = pv_x_->GetValue() - pv_true_x_->GetValue();
    *pv_res_y_ = pv_y_->GetValue() - pv_true_y_->GetValue();
    *pv_res_z_ = pv_z_->GetValue() - pv_true_z_->GetValue();
    *pv_pull_x_val_ = pv_res_x_->GetValue()/sqrt(pv_x_var_->GetValue());
    *pv_pull_y_val_ = pv_res_y_->GetValue()/sqrt(pv_y_var_->GetValue());
    *pv_pull_z_val_ = pv_res_z_->GetValue()/sqrt(pv_z_var_->GetValue()); 
  }
}

} // namespace reducer
} // namespace dooselection
