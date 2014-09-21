/******************************************/
// FlavourTaggingCalibrationGrimReaper.cpp
//
// Standalone GrimReaper that performs a FT
// calibration on for a given tagger.
//
// Author: Christophe Cauet
// Date: 2014-09-21
/******************************************/

// from BOOST
#include "boost/lexical_cast.hpp"

// from ROOT
#include "TRandom3.h"
#include "TCut.h"

// from DooCore
#include "doocore/io/MsgStream.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"
#include "dooselection/reducer/ReducerLeaf.h"
#include "dooselection/reducer/KinematicReducerLeaf.h"

using namespace dooselection::reducer;

class TaggingRdcr : virtual public dooselection::reducer::Reducer {
 public:
  TaggingRdcr():
    // create leaves
    var_out_tag_leaf_(NULL),
    var_out_eta_leaf_(NULL),
    cat_out_tagged_leaf_(NULL),

    // leaves to read
    var_in_tag_(NULL),
    var_in_eta_(NULL),

    // leaves to write
    var_out_tag_value_(NULL),
    var_out_eta_value_(NULL),
    cat_out_tagged_value_(NULL),

    head_(""),

    p0_(0),
    p1_(1),
    avg_eta_(0),

    tag_(""),
    eta_(""),
    cat_(""),
    appendix_("")
  {}
  virtual ~TaggingRdcr(){}
  void set_head(const std::string& head){head_ = head;}
  void set_calibration_parameters(const std::string& input_var_tag, const std::string& input_var_eta, const std::string& input_var_cat, const std::string& cal_p0, const std::string& cal_p1, const std::string& cal_avg_eta, const std::string& new_appendix){
    p0_      = boost::lexical_cast<double>(cal_p0);
    p1_      = boost::lexical_cast<double>(cal_p1);
    avg_eta_ = boost::lexical_cast<double>(cal_avg_eta);

    tag_ = input_var_tag;
    eta_ = input_var_eta;
    cat_ = input_var_cat;
    appendix_ = new_appendix;
  }
 protected:
  virtual void CreateSpecialBranches();
  virtual bool EntryPassesSpecialCuts();
  virtual void UpdateSpecialLeaves();
 private:
  // create leaves
  dooselection::reducer::ReducerLeaf<Int_t>*    var_out_tag_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* var_out_eta_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_out_tagged_leaf_;

  // leaves to read
  Int_t*    var_in_tag_; 
  Double_t* var_in_eta_;      

  // leaves to write
  Int_t*    var_out_tag_value_;
  Double_t* var_out_eta_value_;
  Int_t*    cat_out_tagged_value_;

  // head name
  std::string head_;

  // calibration parameters
  double p0_;
  double p1_;
  double avg_eta_;

  std::string tag_;
  std::string eta_;
  std::string cat_;
  std::string appendix_;
};

void TaggingRdcr::CreateSpecialBranches(){
  // Create new leaves
  var_out_tag_leaf_    = &CreateIntLeaf(tag_+appendix_);
  var_out_eta_leaf_    = &CreateDoubleLeaf(eta_+appendix_);
  cat_out_tagged_leaf_ = &CreateIntLeaf(cat_+appendix_);

  // Link branch addresses
  var_in_tag_     = (Int_t*)GetInterimLeafByName(tag_).branch_address();
  var_in_eta_     = (Double_t*)GetInterimLeafByName(eta_).branch_address();

  var_out_tag_value_    = (Int_t*)var_out_tag_leaf_->branch_address();
  var_out_eta_value_    = (Double_t*)var_out_eta_leaf_->branch_address();
  cat_out_tagged_value_ = (Int_t*)cat_out_tagged_leaf_->branch_address();

  doocore::io::sinfo << "-info- \t" << "FlavourTaggingCalibrationGrimReaper \t" << "Calibrating: " << tag_ << " and " << eta_ << " using: " << doocore::io::endmsg;
  doocore::io::sinfo << "-info- \t" << "FlavourTaggingCalibrationGrimReaper \t" << "p0 = " << p0_ << doocore::io::endmsg;  
  doocore::io::sinfo << "-info- \t" << "FlavourTaggingCalibrationGrimReaper \t" << "p1 = " << p1_ << doocore::io::endmsg;  
  doocore::io::sinfo << "-info- \t" << "FlavourTaggingCalibrationGrimReaper \t" << "<eta> = " << avg_eta_ << doocore::io::endmsg;
  doocore::io::sinfo << "-info- \t" << "FlavourTaggingCalibrationGrimReaper \t" << "with output appendix: " << appendix_ << doocore::io::endmsg;
}

bool TaggingRdcr::EntryPassesSpecialCuts(){return true;}

void TaggingRdcr::UpdateSpecialLeaves(){
  // Always using the FT standard combination
  // see page 106 of: https://cds.cern.ch/record/1456804/files/CERN-THESIS-2012-075.pdf
  // as a reference.
  bool debug = false;

  double decision = *var_in_tag_;
  double mistag   = *var_in_eta_;

  if (mistag >= 0.5){
    *var_out_tag_value_ = 1; 
    *var_out_eta_value_ = 0.5;    
    *cat_out_tagged_value_ = 0;
  }
  else{
    *var_out_tag_value_ = decision; 
    *var_out_eta_value_ = p0_ + p1_ * ( mistag - avg_eta_ );    
    *cat_out_tagged_value_ = 1;
  }

  if (debug){
    std::cout << "" << std::endl;
    std::cout << "Calibration" << std::endl;
    std::cout << "Tag / mistag (with address): \t" << decision << " (" << &decision << ") / " << mistag << " (" << &mistag << ")" << std::endl;
    std::cout << "Calibrated tag / mistag " << *var_out_tag_value_ << " / " << *var_out_eta_value_ << std::endl;
  }
}

int B2JpsiKS(const std::string& inputfile, const std::string& inputtree, const std::string& outputfile, const std::string& outputtree, const std::string& decay_channel, const std::string& input_var_tag, const std::string& input_var_eta, const std::string& input_var_cat, const std::string& cal_p0, const std::string& cal_p1, const std::string& cal_avg_eta, const std::string& new_appendix);

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "FlavourTaggingCalibrationGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string decay_channel = "Bd2JpsiKS";
  std::string inputfile, inputtree, outputfile, outputtree, input_var_tag, input_var_eta, input_var_cat, cal_p0, cal_p1, cal_avg_eta, new_appendix;
  if (argc == 12){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    input_var_tag = argv[5];
    input_var_eta = argv[6];
    input_var_cat = argv[7];
    cal_p0 = argv[8];
    cal_p1 = argv[9];
    cal_avg_eta = argv[10];
    new_appendix = argv[11];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "FlavourTaggingCalibrationGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "FlavourTaggingCalibrationGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name input_var_tag input_var_eta input_var_cat cal_p0 cal_p1 cal_avg_eta new_appendix" << doocore::io::endmsg;
    return 1;
  }

  B2JpsiKS(inputfile, inputtree, outputfile, outputtree, decay_channel, input_var_tag, input_var_eta, input_var_cat, cal_p0, cal_p1, cal_avg_eta, new_appendix);

  doocore::io::sinfo << "-info- \t" << "FlavourTaggingCalibrationGrimReaper \t" << "Done!" << doocore::io::endmsg;
  return 0;
}

int B2JpsiKS(const std::string& inputfile, const std::string& inputtree, const std::string& outputfile, const std::string& outputtree, const std::string& decay_channel, const std::string& input_var_tag, const std::string& input_var_eta, const std::string& input_var_cat, const std::string& cal_p0, const std::string& cal_p1, const std::string& cal_avg_eta, const std::string& new_appendix){
  TaggingRdcr reducer;
  std::string head = "";
  if (decay_channel == "Bd2JpsiKS"){
    head = "B0";
  }
  else if (decay_channel == "Bs2JpsiKS"){
    head = "B_s0";
  }
  reducer.set_head(head);

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);
  reducer.set_calibration_parameters(input_var_tag, input_var_eta, input_var_cat, cal_p0, cal_p1, cal_avg_eta, new_appendix);

  reducer.Initialize();
  reducer.Run();
  reducer.Finalize();

  return 0;
}

