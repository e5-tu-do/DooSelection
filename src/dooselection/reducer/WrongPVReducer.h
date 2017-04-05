#ifndef DOOSELECTION_REDUCER_WRONGPVREDUCER_H
#define DOOSELECTION_REDUCER_WRONGPVREDUCER_H

// from STL

// from STL11
#include <tuple>

// from ROOT

// from DooCore
#include "doocore/io/MsgStream.h"

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
  WrongPVReducer():
    chi2_leaf_(nullptr),
    idxPV_leaf_(nullptr),
    chi2_value_(nullptr),
    pv_x_leaf_(nullptr),
    pv_y_leaf_(nullptr),
    pv_z_leaf_(nullptr),
    pv_x_var_leaf_(nullptr),
    pv_y_var_leaf_(nullptr),
    pv_z_var_leaf_(nullptr),
    pv_x_true_leaf_(nullptr),
    pv_y_true_leaf_(nullptr),
    pv_z_true_leaf_(nullptr),
    pv_x_value_(nullptr),
    pv_y_value_(nullptr),
    pv_z_value_(nullptr),
    pv_x_var_value_(nullptr),
    pv_y_var_value_(nullptr),
    pv_z_var_value_(nullptr),
    pv_x_true_value_(nullptr),
    pv_y_true_value_(nullptr),
    pv_z_true_value_(nullptr),
    chi2_any_leaf_(nullptr),
    chi2_any_value_(nullptr),
    pv_x_res_leaf_(nullptr),
    pv_y_res_leaf_(nullptr),
    pv_z_res_leaf_(nullptr),
    pv_x_pull_leaf_(nullptr),
    pv_y_pull_leaf_(nullptr),
    pv_z_pull_leaf_(nullptr),
    pv_x_res_value_(nullptr),
    pv_y_res_value_(nullptr),
    pv_z_res_value_(nullptr),
    pv_x_pull_value_(nullptr),
    pv_y_pull_value_(nullptr),
    pv_z_pull_value_(nullptr),
    chi2_leaf_name_(""),
    chi2_any_leaf_name_(""),
    idxPV_leaf_name_(""),
    pv_x_leaf_name_(""),
    pv_x_var_leaf_name_(""),
    pv_x_true_leaf_name_(""),
    pv_x_res_leaf_name_("pv_x_res"),
    pv_x_pull_leaf_name_("pv_x_pull"),
    pv_y_leaf_name_(""),
    pv_y_var_leaf_name_(""),
    pv_y_true_leaf_name_(""),
    pv_y_res_leaf_name_("pv_y_res"),
    pv_y_pull_leaf_name_("pv_y_pull"),
    pv_z_leaf_name_(""),
    pv_z_var_leaf_name_(""),
    pv_z_true_leaf_name_(""),
    pv_z_res_leaf_name_("pv_z_res"),
    pv_z_pull_leaf_name_("pv_z_pull"),
    bkg_cat_leaf_name_(""),
    debug_mode_(false)
  {}
  
  virtual ~WrongPVReducer(){}
  void set_chi2_leaf_name(const std::string& chi2_leaf_name){chi2_leaf_name_ = chi2_leaf_name;}
  void set_chi2_any_leaf_name(const std::string& chi2_any_leaf_name){chi2_any_leaf_name_ = chi2_any_leaf_name;}
  void set_idxPV_leaf_name(const std::string& idxPV_leaf_name){idxPV_leaf_name_ = idxPV_leaf_name;}
  void set_pv_x_leaf_name(const std::string& pv_x_leaf_name){pv_x_leaf_name_ = pv_x_leaf_name;}
  void set_pv_x_var_leaf_name(const std::string& pv_x_var_leaf_name){pv_x_var_leaf_name_ = pv_x_var_leaf_name;}
  void set_pv_x_true_leaf_name(const std::string& pv_x_true_leaf_name){pv_x_true_leaf_name_ = pv_x_true_leaf_name;}
  void set_pv_y_leaf_name(const std::string& pv_y_leaf_name){pv_y_leaf_name_ = pv_y_leaf_name;}
  void set_pv_y_var_leaf_name(const std::string& pv_y_var_leaf_name){pv_y_var_leaf_name_ = pv_y_var_leaf_name;}
  void set_pv_y_true_leaf_name(const std::string& pv_y_true_leaf_name){pv_y_true_leaf_name_ = pv_y_true_leaf_name;}
  void set_pv_z_leaf_name(const std::string& pv_z_leaf_name){pv_z_leaf_name_ = pv_z_leaf_name;}
  void set_pv_z_var_leaf_name(const std::string& pv_z_var_leaf_name){pv_z_var_leaf_name_ = pv_z_var_leaf_name;}
  void set_pv_z_true_leaf_name(const std::string& pv_z_true_leaf_name){pv_z_true_leaf_name_ = pv_z_true_leaf_name;}
  void set_bkg_cat_leaf_name(const std::string& bkg_cat_leaf_name){bkg_cat_leaf_name_ = bkg_cat_leaf_name;}

 protected:
  virtual void CreateSpecialBranches();
  virtual bool EntryPassesSpecialCuts();
  virtual void UpdateSpecialLeaves();

 private:
  // leaves to read
  const dooselection::reducer::ReducerLeaf<Float_t>*  chi2_leaf_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  chi2_leaf_flat_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  idxPV_leaf_;
  Float_t*                                            chi2_value_;
  Float_t*                                            chi2_value_flat_;
  Int_t*                                              idxPV_value_; 

  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_x_leaf_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_y_leaf_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_z_leaf_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_x_var_leaf_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_y_var_leaf_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_z_var_leaf_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_x_true_leaf_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_y_true_leaf_;
  const dooselection::reducer::ReducerLeaf<Float_t>*  pv_z_true_leaf_;
  Double_t*  pv_x_value_;
  Double_t*  pv_y_value_;
  Double_t*  pv_z_value_;
  Double_t*  pv_x_var_value_;
  Double_t*  pv_y_var_value_;
  Double_t*  pv_z_var_value_;
  Double_t*  pv_x_true_value_;
  Double_t*  pv_y_true_value_;
  Double_t*  pv_z_true_value_;

  // leaves to write
  dooselection::reducer::ReducerLeaf<Double_t>* chi2_any_leaf_;
  Double_t*                                     chi2_any_value_;

  dooselection::reducer::ReducerLeaf<Double_t>* pv_x_res_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* pv_y_res_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* pv_z_res_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* pv_x_pull_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* pv_y_pull_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* pv_z_pull_leaf_;
  
  Double_t* pv_x_res_value_;
  Double_t* pv_y_res_value_;
  Double_t* pv_z_res_value_;
  Double_t* pv_x_pull_value_;
  Double_t* pv_y_pull_value_;
  Double_t* pv_z_pull_value_;

  std::string chi2_leaf_name_;
  std::string chi2_any_leaf_name_;
  std::string idxPV_leaf_name_;
  std::string pv_x_leaf_name_; 
  std::string pv_x_var_leaf_name_;
  std::string pv_x_true_leaf_name_;
  std::string pv_x_res_leaf_name_;
  std::string pv_x_pull_leaf_name_;
  std::string pv_y_leaf_name_; 
  std::string pv_y_var_leaf_name_;
  std::string pv_y_true_leaf_name_;
  std::string pv_y_res_leaf_name_;
  std::string pv_y_pull_leaf_name_;
  std::string pv_z_leaf_name_; 
  std::string pv_z_var_leaf_name_;
  std::string pv_z_true_leaf_name_;
  std::string pv_z_res_leaf_name_;
  std::string pv_z_pull_leaf_name_;
  std::string bkg_cat_leaf_name_;

  bool debug_mode_;
};

} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_WRONGPVREDUCER_H
