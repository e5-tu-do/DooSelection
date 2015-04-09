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
 *
 *  @author Christophe Cauet
 **/
namespace dooselection {
namespace reducer {
class TMVAClassificationReducer : virtual public Reducer {
 public:
  void SetTMVAMethodAndXMLFile(const TString & tmva_method, const TString & tmva_xml_file) {
 		tmva_method_ 		= tmva_method;
		tmva_xml_file_ 	= tmva_xml_file;
	}
  
  template<class T>
  void SetTMVAVariable(const std::string& var_name, const ReducerLeaf<T>& leaf) {
    if (tmva_xml_file_.Length() > 0) {
      if (leaf.type() == "Float_t") {
        tmva_reader_->AddVariable(var_name, (Float_t*)leaf.branch_address());
      } else {
        ReducerLeaf<Float_t>& tmva_float_copy_leaf = CreateFloatCopyLeaf(leaf.name()+"_tmvafloatcopy", leaf);
        tmva_reader_->AddVariable(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
      }
    }
  }
  
  template<class T>
  void SetTMVASpectatorVariable(const std::string& var_name, const ReducerLeaf<T>& leaf) {
    if (tmva_xml_file_.Length() > 0) {
      if (leaf.type() == "Float_t") {
        tmva_reader_->AddSpectator(var_name, (Float_t*)leaf.branch_address());
      } else {
        ReducerLeaf<Float_t>& tmva_float_copy_leaf = CreateFloatCopyLeaf(leaf.name()+"_tmvafloatcopy", leaf);
        tmva_reader_->AddSpectator(var_name, (Float_t*)tmva_float_copy_leaf.branch_address());
      }
    }
  }

  void SetTMVAVariable(const std::string& var_name, Float_t* addr) {
    if (tmva_xml_file_.Length() > 0) tmva_reader_->AddVariable(var_name.c_str(), addr);
  }
  
 protected:
  virtual void PrepareSpecialBranches() {
		if (tmva_xml_file_.Length() > 0) tmva_reader_->BookMVA( tmva_method_, tmva_xml_file_);
  }
  
  virtual void UpdateSpecialLeaves() {
    if (tmva_xml_file_.Length() > 0) *tmva_classifier_value_ = tmva_reader_->EvaluateMVA(tmva_method_);
  }
  
  virtual void CreateSpecialBranches() {
    using namespace doocore::io;
    if (tmva_xml_file_.Length() > 0) {
      sinfo << "TMVA weights are set. Creating BDT classifier." << endmsg;
      
      tmva_classifier_				= &(CreateDoubleLeaf(tmva_method_+"_classifier", tmva_method_+"_classifier", "Double_t"));
      tmva_classifier_value_	= (Double_t*)tmva_classifier_->branch_address();
      
      tmva_reader_ 						= new TMVA::Reader("!Color");
    }
  }

 private:
  ReducerLeaf<Double_t>	* tmva_classifier_;
  Double_t							* tmva_classifier_value_;
	TString									tmva_method_;
	TMVA::Reader					* tmva_reader_;
	TString									tmva_xml_file_;
};
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_TMVACLASSIFICATIONREDUCER_H

