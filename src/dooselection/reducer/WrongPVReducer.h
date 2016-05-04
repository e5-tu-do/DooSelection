#ifndef DOOSELECTION_REDUCER_WRONGPVREDUCER_H
#define DOOSELECTION_REDUCER_WRONGPVREDUCER_H

// from STL

// from STL11
#include <tuple>

// from ROOT

// from DooCore

// from BOOST

// from project
#include "Reducer.h"
#include "ReducerLeaf.h"

/** @class dooselection::reducer::WrongPVReducer
 *  
 **/
namespace dooselection {
namespace reducer {

class WrongPVReducer : virtual public Reducer {
 public:
  WrongPVReducer();
    chi2_leaf_(nullptr),
    chi2_any_leaf_(nullptr),
    pv_x_leaf_(nullptr),
    pv_y_leaf_(nullptr),
    pv_y_leaf_(nullptr),
    pv_z_leaf_(nullptr),
    pv_x_var_leaf_(nullptr),
    pv_y_var_leaf_(nullptr),
    pv_z_var_leaf_(nullptr),
    pv_res_x_leaf_(nullptr),
    pv_res_y_leaf_(nullptr),
    pv_res_z_leaf_(nullptr),
    pv_pull_x_leaf_(nullptr),
    pv_pull_y_leaf_(nullptr),
    pv_pull_z_leaf_(nullptr),
    pv_pull_x_val_leaf_(nullptr),
    pv_pull_y_val_leaf_(nullptr),
    pv_pull_z_val_leaf_(nullptr),
    chi2_value_(nullptr),
    chi2_any_value_(nullptr),
    pv_x_value_(nullptr),
    pv_y_value_(nullptr),
    pv_y_value_(nullptr),
    pv_z_value_(nullptr),
    pv_x_var_value_(nullptr),
    pv_y_var_value_(nullptr),
    pv_z_var_value_(nullptr),
    pv_res_x_value_(nullptr),
    pv_res_y_value_(nullptr),
    pv_res_z_value_(nullptr),
    pv_pull_x_value_(nullptr),
    pv_pull_y_value_(nullptr),
    pv_pull_z_value_(nullptr),
    pv_pull_x_val_value_(nullptr),
    pv_pull_y_val_value_(nullptr),
    pv_pull_z_val_value_(nullptr),
    chi2_leaf_name_(""),
    chi2_any_leaf_name_(""),
    pv_x_leaf_name_(""),
    pv_y_leaf_name_(""),
    pv_z_leaf_name_(""),
    pv_x_var_leaf_name_(""),
    pv_y_var_leaf_name_(""),
    pv_z_var_leaf_name_(""),
    pv_res_x_leaf_name_(""),
    pv_res_y_leaf_name_(""),
    pv_res_z_leaf_name_(""),
    pv_pull_x_leaf_name_(""),
    pv_pull_y_leaf_name_(""),
    pv_pull_z_leaf_name_(""),
    pv_pull_x_val_leaf_name_(""),
    pv_pull_y_val_leaf_name_(""),
    pv_pull_z_val_leaf_name_(""),
    debug_mode_(false)
  {}
  
  virtual ~WrongPVReducer(){}
  void set_chi2_leaf_name(const std::string& chi2_leaf_name){chi2_leaf_name_ = chi2_leaf_name;}
  void set_chi2_any_leaf_name(const std::string& chi2_any_leaf_name){chi2_any_leaf_name_ = chi2_any_leaf_name;}
  void set_idx_leaf_name(const std::string& idx_leaf_name){idx_leaf_name_ = idx_leaf_name;}
  void set_pv_x_leaf_name(const std::string& pv_x_leaf_name){pv_x_leaf_name_ = pv_x_leaf_name;}
  void set_pv_x_var_leaf_name(const std::string& pv_x_var_leaf_name){pv_x_var_leaf_name_ = pv_x_var_leaf_name;}
  void set_pv_x_true_leaf_name(const std::string& pv_x_true_leaf_name){pv_x_true_leaf_name_ = pv_x_true_leaf_name;}
  void set_pv_y_leaf_name(const std::string& pv_y_leaf_name){pv_y_leaf_name_ = pv_y_leaf_name;}
  void set_pv_y_var_leaf_name(const std::string& pv_y_var_leaf_name){pv_y_var_leaf_name_ = pv_y_var_leaf_name;}
  void set_pv_y_true_leaf_name(const std::string& pv_y_true_leaf_name){pv_y_true_leaf_name_ = pv_y_true_leaf_name;}
  void set_pv_z_leaf_name(const std::string& pv_z_leaf_name){pv_z_leaf_name_ = pv_z_leaf_name;}
  void set_pv_z_var_leaf_name(const std::string& pv_z_var_leaf_name){pv_z_var_leaf_name_ = pv_z_var_leaf_name;}
  void set_pv_z_true_leaf_name(const std::string& pv_z_true_leaf_name){pv_z_true_leaf_name_ = pv_z_true_leaf_name;}
 
 protected:
  virtual void CreateSpecialBranches();
  virtual bool EntryPassesSpecialCuts();
  virtual void UpdateSpecialLeaves();

 private:
  // leaves to read
  const dooselection::reducer::ReducerLeaf<Float_t>*  chi2_leaf_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  chi2_leaf_flat_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  idx_leaf_;
  Float_t*                                            chi2_value_;
  Float_t*                                            chi2_value_flat_;
  Int_t*                                              idx_value_; 

  const dooselection::reducer::ReducerLeaf<Float_t>*  comparison_leaf_;
  Float_t*                                            comparison_value_;

  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_x_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_y_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_z_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_x_var_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_y_var_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_z_var_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_true_x_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_true_y_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_true_z_;

  // leaves to write
  dooselection::reducer::ReducerLeaf<Double_t>* out_leaf_;
  Double_t*                                     out_value_;

  dooselection::reducer::ReducerLeaf<Double_t>* pv_res_x_;
  dooselection::reducer::ReducerLeaf<Double_t>* pv_res_y_;
  dooselection::reducer::ReducerLeaf<Double_t>* pv_res_z_;
  dooselection::reducer::ReducerLeaf<Double_t>* pv_pull_x_;
  dooselection::reducer::ReducerLeaf<Double_t>* pv_pull_y_;
  dooselection::reducer::ReducerLeaf<Double_t>* pv_pull_z_;
  
  Double_t* pv_res_x_val_;
  Double_t* pv_res_y_val_;
  Double_t* pv_res_z_val_;
  Double_t* pv_pull_x_val_;
  Double_t* pv_pull_y_val_;
  Double_t* pv_pull_z_val_;

  std::string chi2_leaf_name_;
  std::string chi2_any_leaf_name_;
  std::string idx_leaf_name_;
  std::string pv_x_leaf_name_; 
  std::string pv_x_var_leaf_name_;
  std::string pv_x_true_leaf_name_;
  std::string pv_y_leaf_name_; 
  std::string pv_y_var_leaf_name_;
  std::string pv_y_true_leaf_name_;
  std::string pv_z_leaf_name_; 
  std::string pv_z_var_leaf_name_;
  std::string pv_z_true_leaf_name_;

  bool debug_mode_;
};

} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_WRONGPVREDUCER_H
