/******************************************/
// WrongPVGrimReaper.cpp
//
// Standalone GrimReaper adds the minimal 
// J/psi IP chi2 w.r.t any PV to a tuple.
//
// Author: Christophe Cauet
// Date: 2014-10-26
/******************************************/

// from DooCore
#include "doocore/io/MsgStream.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"

using namespace doocore::io;
using namespace dooselection::reducer;

//==============================================================================
//                     WrongPVReducer class definition
//==============================================================================
class WrongPVReducer : virtual public dooselection::reducer::Reducer {
 public:
  WrongPVReducer():
    in_leaf_(nullptr),
    in_value_(nullptr),
    out_leaf_(nullptr),
    out_value_(nullptr),
    in_leaf_name_("B0_FitDaughtersPVConst_J_psi_1S_IPCHI2"),
    in_leaf_idx_pv_name_("idxPV"),
    out_leaf_name_("varJpsiMinIPCHI2anyPV"),
    vertex_x_(nullptr),
    vertex_y_(nullptr),
    vertex_z_(nullptr),
    vertex_xvar_(nullptr),
    vertex_yvar_(nullptr),
    vertex_zvar_(nullptr),
    vertex_res_x_(nullptr),
    vertex_res_y_(nullptr),
    vertex_res_z_(nullptr),
    vertex_pull_x_(nullptr),
    vertex_pull_y_(nullptr),
    vertex_pull_z_(nullptr),
    vertex_pull_x_val_(nullptr),
    vertex_pull_y_val_(nullptr),
    vertex_pull_z_val_(nullptr),
    debug_mode_(false)
  {}
  virtual ~WrongPVReducer(){}

 protected:
  virtual void CreateSpecialBranches();
  virtual bool EntryPassesSpecialCuts();
  virtual void UpdateSpecialLeaves();
 private:
  // leaves to read
  const dooselection::reducer::ReducerLeaf<Float_t>*  in_leaf_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  in_leaf_flat_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  in_leaf_idx_pv_;
  Float_t*                                            in_value_;
  Float_t*                                            in_value_flat_;
  Int_t*                                              in_value_idx_pv_; 
  
  const dooselection::reducer::ReducerLeaf<Float_t>*  vertex_x_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  vertex_y_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  vertex_z_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  vertex_xvar_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  vertex_yvar_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  vertex_zvar_;

  const dooselection::reducer::ReducerLeaf<Float_t>*  vertex_true_x_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  vertex_true_y_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  vertex_true_z_;

  // leaves to write
  dooselection::reducer::ReducerLeaf<Double_t>* out_leaf_;
  Double_t*                                     out_value_;

  dooselection::reducer::ReducerLeaf<Double_t>* vertex_res_x_;
  dooselection::reducer::ReducerLeaf<Double_t>* vertex_res_y_;
  dooselection::reducer::ReducerLeaf<Double_t>* vertex_res_z_;

  dooselection::reducer::ReducerLeaf<Double_t>* vertex_pull_x_;
  dooselection::reducer::ReducerLeaf<Double_t>* vertex_pull_y_;
  dooselection::reducer::ReducerLeaf<Double_t>* vertex_pull_z_;
  Double_t* vertex_pull_x_val_;
  Double_t* vertex_pull_y_val_;
  Double_t* vertex_pull_z_val_;

  // 
  std::string in_leaf_name_;
  std::string in_leaf_idx_pv_name_;
  std::string out_leaf_name_;

  bool debug_mode_;
};

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

  vertex_x_ = &GetInterimLeafByName("B0_FitPVConst_PV_X_flat");
  vertex_y_ = &GetInterimLeafByName("B0_FitPVConst_PV_Y_flat");
  vertex_z_ = &GetInterimLeafByName("B0_FitPVConst_PV_Z_flat");
  vertex_xvar_ = &GetInterimLeafByName("B0_FitPVConst_PV_XVAR_flat");
  vertex_yvar_ = &GetInterimLeafByName("B0_FitPVConst_PV_YVAR_flat");
  vertex_zvar_ = &GetInterimLeafByName("B0_FitPVConst_PV_ZVAR_flat");
  vertex_true_x_ = &GetInterimLeafByName("B0_TRUEORIGINVERTEX_X");
  vertex_true_y_ = &GetInterimLeafByName("B0_TRUEORIGINVERTEX_Y");
  vertex_true_z_ = &GetInterimLeafByName("B0_TRUEORIGINVERTEX_Z");

  if(LeafExists("B0_BKGCAT")) {
    std::cout << "This is an MC Sample " << std::endl;
    if(LeafExists("B0_FitPVConst_PV_ZVAR_flat")){
      sinfo << "B0_FitPVConst_PV_ZVAR_flat" << endmsg;
      vertex_res_x_ = &CreateDoubleLeaf("vertex_res_x");
      vertex_res_y_ = &CreateDoubleLeaf("vertex_res_y");
      vertex_res_z_ = &CreateDoubleLeaf("vertex_res_z");
      vertex_res_x_->Add(*vertex_x_, *vertex_true_x_, 1., -1.);
      vertex_res_y_->Add(*vertex_y_, *vertex_true_y_, 1., -1.);
      vertex_res_z_->Add(*vertex_z_, *vertex_true_z_, 1., -1.);

      vertex_pull_x_ = &CreateDoubleLeaf("vertex_pull_x");
      vertex_pull_y_ = &CreateDoubleLeaf("vertex_pull_y");
      vertex_pull_z_ = &CreateDoubleLeaf("vertex_pull_z");
      vertex_pull_x_val_ = (Double_t*)vertex_pull_x_->branch_address();
      vertex_pull_y_val_ = (Double_t*)vertex_pull_y_->branch_address();
      vertex_pull_z_val_ = (Double_t*)vertex_pull_z_->branch_address();
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
bool WrongPVReducer::EntryPassesSpecialCuts(){return true;}

//------------------------------------------------------------------------------
//                   WrongPVReducer::UpdateSpecialLeaves()
//------------------------------------------------------------------------------
void WrongPVReducer::UpdateSpecialLeaves(){
  unsigned int nPV = in_leaf_->Length();
  unsigned int idxPV = *in_value_idx_pv_;
  double ip_chi2 = *in_value_flat_;

  if (debug_mode_) sinfo << "Number of PVs in event: " << nPV << ", PV index of this (B, PV)-pair: " << idxPV << " with an IP chi2: " << ip_chi2 << endmsg;

  // sanity check if IP chi2 is smaller 0, set to -999
  if (ip_chi2 < 0){
    *out_value_ = -999;
  }
  // if number of PVs is one, set to default value -1
  else if (nPV == 1){
    *out_value_ = 99999;
  }
  else {
    double min_ip_chi2 = 99999;
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

  if(vertex_pull_z_) {
    vertex_res_x_->UpdateValue();
    vertex_res_y_->UpdateValue();
    vertex_res_z_->UpdateValue();
    *vertex_pull_x_val_ = vertex_res_x_->GetValue()/sqrt(vertex_xvar_->GetValue()); 
    *vertex_pull_y_val_ = vertex_res_y_->GetValue()/sqrt(vertex_yvar_->GetValue());
    *vertex_pull_z_val_ = vertex_res_z_->GetValue()/sqrt(vertex_zvar_->GetValue()); 
  }

}

//==============================================================================
//                                   MAIN
//==============================================================================
int main(int argc, char * argv[]){
  sinfo << "-info-  \t" << "WrongPVGrimReaper \t" << "Welcome!" << endmsg;
  std::string inputfile, inputtree, outputfile, outputtree;
  if (argc == 5){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
  }
  else{
    serr << "-ERROR- \t" << "WrongPVGrimReaper \t" << "Parameters needed:" << endmsg;
    serr << "-ERROR- \t" << "WrongPVGrimReaper \t" << "input_file_name input_tree_name output_file_name output_tree_name" << endmsg;
    return 1;
  }
  WrongPVReducer reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  reducer.Initialize();
  reducer.Run();
  reducer.Finalize();

  sinfo << "-info- \t" << "WrongPVGrimReaper \t" << "Done!" << endmsg;
}
