#ifndef DOOSELECTION_REDUCER_TMVACLASSIFICATIONREDUCER_H
#define DOOSELECTION_REDUCER_TMVACLASSIFICATIONREDUCER_H

// from STL
#include <utility>
#include <vector>

// from ROOT
#include "TRandom3.h"
#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"
#include "TMVA/DataInputHandler.h"

// from DooCore
#include "doocore/io/MsgStream.h"

// from project
#include "Reducer.h"

/** @class dooselection::reducer::TMVAClassificationReducer
 *  @brief Derived Reducer to write TMVA classification into output tree
 *
 *  This is a Reducer derived from Reducer. It can be used to write TMVA based
 *  classification into the output tuple produced.
 *
 *  @section tmvared_usage Usage
 *
 *  Consider the following example:
 *
 *  @code
 *  TMVAClassificationReducer AReducer;
 *  AReducer.SetTMVAMethodAndXMLFile("BDT", "myweightfile.xml");
 *  
 *  AReducer.SetTMVAVariable("muminus_PT", (Float_t*)AReducer.GetInterimLeafByName("muminus_PT").branch_address());
 *  AReducer.SetTMVAVariable("muplus_PT", (Float_t*)AReducer.GetInterimLeafByName("muplus_PT").branch_address());
 *  @endcode
 **/
namespace dooselection {
namespace reducer {
class TMVAClassificationReducer : virtual public Reducer {
 public:
  void SetTMVAMethodAndXMLFile(const TString & tmva_method, const TString & tmva_xml_file) {
 		tmva_method_ 		= tmva_method;
		tmva_xml_file_ 	= tmva_xml_file;
	}
  void SetTMVAMethodsAndXMLFiles(const std::vector<std::string> & tmva_methods, const std::vector<std::string> & tmva_xml_files) {
    tmva_methods_   = tmva_methods;
    tmva_xml_files_ = tmva_xml_files;
  }
  
  template<class T>
  void SetTMVAVariable(const std::string& var_name, const ReducerLeaf<T>& leaf) {
    if (tmva_xml_file_.Length() > 0 || tmva_xml_files_.size() > 0) {
      if (leaf.type() == "Float_t") {
        if (tmva_xml_file_.Length() > 0) tmva_reader_->AddVariable(var_name, (Float_t*)leaf.branch_address());
        else {
                                        tmva_reader_1_->AddVariable(var_name, (Float_t*)leaf.branch_address());
          if (tmva_methods_.size() > 1) tmva_reader_2_->AddVariable(var_name, (Float_t*)leaf.branch_address());
          if (tmva_methods_.size() > 2) tmva_reader_3_->AddVariable(var_name, (Float_t*)leaf.branch_address());
          if (tmva_methods_.size() > 3) tmva_reader_4_->AddVariable(var_name, (Float_t*)leaf.branch_address());
          if (tmva_methods_.size() > 4) tmva_reader_5_->AddVariable(var_name, (Float_t*)leaf.branch_address());
        }
      } else if (LeafExists(std::string(leaf.name()+"_tmvafloatcopy"))){
        if (tmva_xml_file_.Length() > 0) tmva_reader_->AddVariable(var_name, (Float_t*)(GetInterimLeafByName(leaf.name()+"_tmvafloatcopy").branch_address()));
        else {
                                        tmva_reader_1_->AddVariable(var_name, (Float_t*)(GetInterimLeafByName(leaf.name()+"_tmvafloatcopy").branch_address()));
          if (tmva_methods_.size() > 1) tmva_reader_2_->AddVariable(var_name, (Float_t*)(GetInterimLeafByName(leaf.name()+"_tmvafloatcopy").branch_address()));
          if (tmva_methods_.size() > 2) tmva_reader_3_->AddVariable(var_name, (Float_t*)(GetInterimLeafByName(leaf.name()+"_tmvafloatcopy").branch_address()));
          if (tmva_methods_.size() > 3) tmva_reader_4_->AddVariable(var_name, (Float_t*)(GetInterimLeafByName(leaf.name()+"_tmvafloatcopy").branch_address()));
          if (tmva_methods_.size() > 4) tmva_reader_5_->AddVariable(var_name, (Float_t*)(GetInterimLeafByName(leaf.name()+"_tmvafloatcopy").branch_address()));
        }
      } else {
        ReducerLeaf<Float_t>& tmva_float_copy_leaf = CreateFloatCopyLeaf(leaf.name()+"_tmvafloatcopy", leaf);
        if (tmva_xml_file_.Length() > 0) tmva_reader_->AddVariable(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
        else {
                                        tmva_reader_1_->AddVariable(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
          if (tmva_methods_.size() > 1) tmva_reader_2_->AddVariable(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
          if (tmva_methods_.size() > 2) tmva_reader_3_->AddVariable(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
          if (tmva_methods_.size() > 3) tmva_reader_4_->AddVariable(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
          if (tmva_methods_.size() > 4) tmva_reader_5_->AddVariable(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
        }
      }
    }
  }
  
  template<class T>
  void SetTMVASpectatorVariable(const std::string& var_name, const ReducerLeaf<T>& leaf) {
    if (tmva_xml_file_.Length() > 0 || tmva_xml_files_.size() > 0) {
      if (leaf.type() == "Float_t") {
        if (tmva_xml_file_.Length() > 0) tmva_reader_->AddSpectator(var_name, (Float_t*)leaf.branch_address());
        else {
                                        tmva_reader_1_->AddSpectator(var_name, (Float_t*)leaf.branch_address());
          if (tmva_methods_.size() > 1) tmva_reader_2_->AddSpectator(var_name, (Float_t*)leaf.branch_address());
          if (tmva_methods_.size() > 2) tmva_reader_3_->AddSpectator(var_name, (Float_t*)leaf.branch_address());
          if (tmva_methods_.size() > 3) tmva_reader_4_->AddSpectator(var_name, (Float_t*)leaf.branch_address());
          if (tmva_methods_.size() > 4) tmva_reader_5_->AddSpectator(var_name, (Float_t*)leaf.branch_address());
        }
      } else if (LeafExists(std::string(leaf.name()+"_tmvafloatcopy"))){
        if (tmva_xml_file_.Length() > 0) tmva_reader_->AddSpectator(var_name, (Float_t*)(GetInterimLeafByName(leaf.name()+"_tmvafloatcopy").branch_address()));
        else {
                                        tmva_reader_1_->AddSpectator(var_name, (Float_t*)(GetInterimLeafByName(leaf.name()+"_tmvafloatcopy").branch_address()));
          if (tmva_methods_.size() > 1) tmva_reader_2_->AddSpectator(var_name, (Float_t*)(GetInterimLeafByName(leaf.name()+"_tmvafloatcopy").branch_address()));
          if (tmva_methods_.size() > 2) tmva_reader_3_->AddSpectator(var_name, (Float_t*)(GetInterimLeafByName(leaf.name()+"_tmvafloatcopy").branch_address()));
          if (tmva_methods_.size() > 3) tmva_reader_4_->AddSpectator(var_name, (Float_t*)(GetInterimLeafByName(leaf.name()+"_tmvafloatcopy").branch_address()));
          if (tmva_methods_.size() > 4) tmva_reader_5_->AddSpectator(var_name, (Float_t*)(GetInterimLeafByName(leaf.name()+"_tmvafloatcopy").branch_address()));
        }
      } else {
        ReducerLeaf<Float_t>& tmva_float_copy_leaf = CreateFloatCopyLeaf(leaf.name()+"_tmvafloatcopy", leaf);
        if (tmva_xml_file_.Length() > 0) tmva_reader_->AddSpectator(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
        else {
                                        tmva_reader_1_->AddSpectator(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
          if (tmva_methods_.size() > 1) tmva_reader_2_->AddSpectator(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
          if (tmva_methods_.size() > 2) tmva_reader_3_->AddSpectator(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
          if (tmva_methods_.size() > 3) tmva_reader_4_->AddSpectator(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
          if (tmva_methods_.size() > 4) tmva_reader_5_->AddSpectator(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
        }
      }
    }
  }

  void SetTMVAVariable(const std::string& var_name, Float_t* addr) {
    if (tmva_xml_file_.Length() > 0) tmva_reader_->AddVariable(var_name.c_str(), addr);
    if (tmva_xml_files_.size() > 0) tmva_reader_1_->AddVariable(var_name.c_str(), addr);
    if (tmva_xml_files_.size() > 1) tmva_reader_2_->AddVariable(var_name.c_str(), addr);
    if (tmva_xml_files_.size() > 2) tmva_reader_3_->AddVariable(var_name.c_str(), addr);
    if (tmva_xml_files_.size() > 3) tmva_reader_4_->AddVariable(var_name.c_str(), addr);
    if (tmva_xml_files_.size() > 4) tmva_reader_5_->AddVariable(var_name.c_str(), addr);
  }
  
 protected:
  virtual void PrepareSpecialBranches() {
		if (tmva_xml_file_.Length() > 0) tmva_reader_->BookMVA( tmva_method_, tmva_xml_file_);
    if (tmva_xml_files_.size() > 0) tmva_reader_1_->BookMVA( tmva_methods_.at(0), tmva_xml_files_.at(0));
    if (tmva_xml_files_.size() > 1) tmva_reader_2_->BookMVA( tmva_methods_.at(1), tmva_xml_files_.at(1));
    if (tmva_xml_files_.size() > 2) tmva_reader_3_->BookMVA( tmva_methods_.at(2), tmva_xml_files_.at(2));
    if (tmva_xml_files_.size() > 3) tmva_reader_4_->BookMVA( tmva_methods_.at(3), tmva_xml_files_.at(3));
    if (tmva_xml_files_.size() > 4) tmva_reader_5_->BookMVA( tmva_methods_.at(4), tmva_xml_files_.at(4));
  }
  
  virtual void UpdateSpecialLeaves() {
    if (tmva_xml_file_.Length() > 0) *tmva_classifier_value_ = tmva_reader_->EvaluateMVA(tmva_method_);
    if (tmva_xml_files_.size() > 0) *tmva_classifier_1_value_ = tmva_reader_1_->EvaluateMVA(tmva_methods_.at(0));
    if (tmva_xml_files_.size() > 1) *tmva_classifier_2_value_ = tmva_reader_2_->EvaluateMVA(tmva_methods_.at(1));
    if (tmva_xml_files_.size() > 2) *tmva_classifier_3_value_ = tmva_reader_3_->EvaluateMVA(tmva_methods_.at(2));
    if (tmva_xml_files_.size() > 3) *tmva_classifier_4_value_ = tmva_reader_4_->EvaluateMVA(tmva_methods_.at(3));
    if (tmva_xml_files_.size() > 4) *tmva_classifier_5_value_ = tmva_reader_5_->EvaluateMVA(tmva_methods_.at(4));
  }
  
  virtual void CreateSpecialBranches() {
    using namespace doocore::io;
    if (tmva_xml_file_.Length() > 0) {
      sinfo << "TMVA weights are set. Creating BDT classifier." << endmsg;
      tmva_classifier_        = &(CreateDoubleLeaf(tmva_method_+"_classifier", tmva_method_+"_classifier", "Double_t"));
      tmva_classifier_value_  = (Double_t*)tmva_classifier_->branch_address();
      
      tmva_reader_            = new TMVA::Reader("!Color");
    }
    else if (tmva_methods_.size() > 0) {
      sinfo << "TMVA weights are set. Creating BDT classifier." << endmsg;
      tmva_classifier_1_        = &(CreateDoubleLeaf(std::string(tmva_methods_.at(0)+"_classifier"), std::string(tmva_methods_.at(0)+"_classifier"), "Double_t"));
      tmva_classifier_1_value_  = (Double_t*)tmva_classifier_1_->branch_address();
      tmva_reader_1_            = new TMVA::Reader("!Color");
      if (tmva_methods_.size() > 1) {
        tmva_classifier_2_        = &(CreateDoubleLeaf(std::string(tmva_methods_.at(1)+"_classifier"), std::string(tmva_methods_.at(1)+"_classifier"), "Double_t"));
        tmva_classifier_2_value_  = (Double_t*)tmva_classifier_2_->branch_address();
        tmva_reader_2_            = new TMVA::Reader("!Color");
      }
      if (tmva_methods_.size() > 2) {
        tmva_classifier_3_        = &(CreateDoubleLeaf(std::string(tmva_methods_.at(2)+"_classifier"), std::string(tmva_methods_.at(2)+"_classifier"), "Double_t"));
        tmva_classifier_3_value_  = (Double_t*)tmva_classifier_3_->branch_address();
        tmva_reader_3_            = new TMVA::Reader("!Color");
      }
      if (tmva_methods_.size() > 3) {
        tmva_classifier_4_        = &(CreateDoubleLeaf(std::string(tmva_methods_.at(3)+"_classifier"), std::string(tmva_methods_.at(3)+"_classifier"), "Double_t"));
        tmva_classifier_4_value_  = (Double_t*)tmva_classifier_4_->branch_address();
        tmva_reader_4_            = new TMVA::Reader("!Color");
      }
      if (tmva_methods_.size() > 4) {
        tmva_classifier_5_        = &(CreateDoubleLeaf(std::string(tmva_methods_.at(4)+"_classifier"), std::string(tmva_methods_.at(4)+"_classifier"), "Double_t"));
        tmva_classifier_5_value_  = (Double_t*)tmva_classifier_5_->branch_address();
        tmva_reader_5_            = new TMVA::Reader("!Color");
      }
    }
  }

 private:
  ReducerLeaf<Double_t>	* tmva_classifier_;
  Double_t							* tmva_classifier_value_;
	TString									tmva_method_;
	TMVA::Reader					* tmva_reader_;
	TString									tmva_xml_file_;

  std::vector<std::string>  tmva_methods_;
  std::vector<std::string>  tmva_xml_files_;
  ReducerLeaf<Double_t>   * tmva_classifier_1_;
  Double_t                * tmva_classifier_1_value_;
  TMVA::Reader            * tmva_reader_1_;
  ReducerLeaf<Double_t>   * tmva_classifier_2_;
  Double_t                * tmva_classifier_2_value_;
  TMVA::Reader            * tmva_reader_2_;
  ReducerLeaf<Double_t>   * tmva_classifier_3_;
  Double_t                * tmva_classifier_3_value_;
  TMVA::Reader            * tmva_reader_3_;
  ReducerLeaf<Double_t>   * tmva_classifier_4_;
  Double_t                * tmva_classifier_4_value_;
  TMVA::Reader            * tmva_reader_4_;
  ReducerLeaf<Double_t>   * tmva_classifier_5_;
  Double_t                * tmva_classifier_5_value_;
  TMVA::Reader            * tmva_reader_5_;
};
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_TMVACLASSIFICATIONREDUCER_H

