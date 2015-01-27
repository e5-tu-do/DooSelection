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
    var_out_eta_B0_leaf_(NULL),
    var_out_eta_B0bar_leaf_(NULL),
    cat_out_tagged_leaf_(NULL),

    // leaves to read
    var_in_tag_OS_(NULL),
    var_in_tag_SS_(NULL),
    var_in_eta_OS_(NULL),
    var_in_eta_SS_(NULL),

    // leaves to write
    var_out_tag_value_(NULL),
    var_out_eta_B0_value_(NULL),
    var_out_eta_B0bar_value_(NULL),
    cat_out_tagged_value_(NULL),

    p0_OS_(0.3816),
    p1_OS_(0.97814),
    avg_eta_OS_(0.3786),
    delta_p0_OS_(0.01456),
    delta_p1_OS_(0.069399),
    p0_SS_(0.42280),
    p1_SS_(1.0131),
    avg_eta_SS_(0.42484),
    delta_p0_SS_(-0.00153078),
    delta_p1_SS_(-0.160527),
    
    tag_("obsTag"),
    eta_B0_("obsEta_B0"),
    eta_B0bar_("obsEta_B0bar"),
    cat_("catTagged"),
    appendix_("_combined")
  {}
  virtual ~TaggingRdcr(){}
 protected:
  virtual void CreateSpecialBranches();
  virtual bool EntryPassesSpecialCuts();
  virtual void UpdateSpecialLeaves();
 private:
  // create leaves
  dooselection::reducer::ReducerLeaf<Int_t>*    var_out_tag_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* var_out_eta_B0_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* var_out_eta_B0bar_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_out_tagged_leaf_;

  // leaves to read
  Int_t*    var_in_tag_OS_;
  Int_t*    var_in_tag_SS_;
  Double_t* var_in_eta_OS_;
  Double_t* var_in_eta_SS_;

  // leaves to write
  Int_t*    var_out_tag_value_;
  Double_t* var_out_eta_B0_value_;
  Double_t* var_out_eta_B0bar_value_;
  Int_t*    cat_out_tagged_value_;

  // calibration parameters
  double p0_OS_;
  double p1_OS_;
  double avg_eta_OS_;
  double delta_p0_OS_;
  double delta_p1_OS_;
  double p0_SS_;
  double p1_SS_;
  double avg_eta_SS_;
  double delta_p0_SS_;
  double delta_p1_SS_;

  std::string tag_;
  std::string eta_B0_;
  std::string eta_B0bar_;
  std::string cat_;
  std::string appendix_;
};

void TaggingRdcr::CreateSpecialBranches(){
  // Create new leaves
  var_out_tag_leaf_        = &CreateIntLeaf(tag_+appendix_);
  var_out_eta_B0_leaf_     = &CreateDoubleLeaf(eta_B0_+appendix_);
  var_out_eta_B0bar_leaf_  = &CreateDoubleLeaf(eta_B0bar_+appendix_);
  cat_out_tagged_leaf_     = &CreateIntLeaf(cat_+appendix_);

  // Link branch addresses
  var_in_tag_OS_     = (Int_t*)GetInterimLeafByName("obsTagOS_StdComb_CutOff_NoZero").branch_address();
  var_in_eta_OS_     = (Double_t*)GetInterimLeafByName("obsEtaOS_StdComb_CutOff").branch_address();
  var_in_tag_SS_     = (Int_t*)GetInterimLeafByName("obsTagSSPion_NoZero").branch_address();
  var_in_eta_SS_     = (Double_t*)GetInterimLeafByName("obsEtaSSPion_TupleCalib").branch_address();

  var_out_tag_value_        = (Int_t*)var_out_tag_leaf_->branch_address();
  var_out_eta_B0_value_     = (Double_t*)var_out_eta_B0_leaf_->branch_address();
  var_out_eta_B0bar_value_  = (Double_t*)var_out_eta_B0bar_leaf_->branch_address();
  cat_out_tagged_value_     = (Int_t*)cat_out_tagged_leaf_->branch_address();
}

bool TaggingRdcr::EntryPassesSpecialCuts(){return true;}

void TaggingRdcr::UpdateSpecialLeaves(){
  // Always using the FT standard combination
  // see page 106 of: https://cds.cern.ch/record/1456804/files/CERN-THESIS-2012-075.pdf
  // as a reference.

  double tag_OS    = *var_in_tag_OS_;
  double tag_SS    = *var_in_tag_SS_;
  double mistag_OS = *var_in_eta_OS_;
  double mistag_SS = *var_in_eta_SS_;
  double omega_OS  = p0_OS_ + p1_OS_ * ( mistag_OS - avg_eta_OS_ );
  double omega_SS  = p0_SS_ + p1_SS_ * ( mistag_SS - avg_eta_SS_ );
  double omega_OS_B0  = p0_OS_ + 0.5 * delta_p0_OS_ + ( p1_OS_ + 0.5 * delta_p1_OS_ ) * ( mistag_OS - avg_eta_OS_ );
  double omega_SS_B0  = p0_SS_ + 0.5 * delta_p0_SS_ + ( p1_SS_ + 0.5 * delta_p1_SS_ ) * ( mistag_SS - avg_eta_SS_ );
  double omega_OS_B0bar  = p0_OS_ - 0.5 * delta_p0_OS_ + ( p1_OS_ - 0.5 * delta_p1_OS_ ) * ( mistag_OS - avg_eta_OS_ );
  double omega_SS_B0bar  = p0_SS_ - 0.5 * delta_p0_SS_ + ( p1_SS_ - 0.5 * delta_p1_SS_ ) * ( mistag_SS - avg_eta_SS_ );

  if (mistag_OS >= 0.5 && mistag_SS >= 0.5){
    *var_out_tag_value_ = 1;
    *var_out_eta_B0_value_ = 0.5;
    *var_out_eta_B0bar_value_ = 0.5;
    *cat_out_tagged_value_ = 0;
  }
  else if (mistag_OS < 0.5 && mistag_SS >= 0.5){
    *var_out_tag_value_ = -tag_OS;
    *var_out_eta_B0_value_ = omega_OS_B0;
    *var_out_eta_B0bar_value_ = omega_OS_B0bar;
    *cat_out_tagged_value_ = 1;
  }
  else if (mistag_OS >= 0.5 && mistag_SS < 0.5){
    *var_out_tag_value_ = -tag_SS;
    *var_out_eta_B0_value_ = omega_SS_B0;
    *var_out_eta_B0bar_value_ = omega_SS_B0bar;
    *cat_out_tagged_value_ = 1;
  }
  else {
    if (tag_OS == tag_SS){
      *var_out_tag_value_ = -tag_OS; 
      *var_out_eta_B0_value_    = ( omega_OS_B0 * omega_SS_B0 ) / ( omega_OS_B0 * omega_SS_B0 + ( 1 - omega_OS_B0 ) * ( 1 - omega_SS_B0 ));
      *var_out_eta_B0bar_value_ = ( omega_OS_B0bar * omega_SS_B0bar ) / ( omega_OS_B0bar * omega_SS_B0bar + ( 1 - omega_OS_B0bar ) * ( 1 - omega_SS_B0bar ));
      *cat_out_tagged_value_ = 1;
    }
    else if (omega_OS <= omega_SS){
      *var_out_tag_value_ = -tag_OS; 
      *var_out_eta_B0_value_ = ( omega_OS_B0 * ( 1 - omega_SS_B0 )) / ( omega_OS_B0 * ( 1 - omega_SS_B0 ) + ( 1 - omega_OS_B0 ) * omega_SS_B0 );
      *var_out_eta_B0bar_value_ = ( omega_OS_B0bar * ( 1 - omega_SS_B0bar )) / ( omega_OS_B0bar * ( 1 - omega_SS_B0bar ) + ( 1 - omega_OS_B0bar ) * omega_SS_B0bar );
      *cat_out_tagged_value_ = 1; 
    }
    else {
      *var_out_tag_value_ = -tag_SS; 
      *var_out_eta_B0_value_ = ( omega_SS_B0 * ( 1 - omega_OS_B0 )) / ( omega_SS_B0 * ( 1 - omega_OS_B0 ) + ( 1 - omega_SS_B0 ) * omega_OS_B0 );
      *var_out_eta_B0bar_value_ = ( omega_SS_B0bar * ( 1 - omega_OS_B0bar )) / ( omega_SS_B0bar * ( 1 - omega_OS_B0bar ) + ( 1 - omega_SS_B0bar ) * omega_OS_B0bar );
      *cat_out_tagged_value_ = 1;
    }
  }
}

int B2JpsiKS(const std::string& inputfile, const std::string& inputtree, const std::string& outputfile, const std::string& outputtree);

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "FlavourTaggingCombinationGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree;
  if (argc == 5){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "FlavourTaggingCombinationGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "FlavourTaggingCombinationGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name" << doocore::io::endmsg;
    return 1;
  }

  B2JpsiKS(inputfile, inputtree, outputfile, outputtree);

  doocore::io::sinfo << "-info- \t" << "FlavourTaggingCombinationGrimReaper \t" << "Done!" << doocore::io::endmsg;
  return 0;
}

int B2JpsiKS(const std::string& inputfile, const std::string& inputtree, const std::string& outputfile, const std::string& outputtree){
  TaggingRdcr reducer;
  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  reducer.Initialize();
  reducer.Run();
  reducer.Finalize();

  return 0;
}

