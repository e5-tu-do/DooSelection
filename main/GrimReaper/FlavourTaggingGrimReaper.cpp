/******************************************/
// FlavourTaggingGrimReaper.cpp
//
// Standalone GrimReaper that adds a lot of
// common variables to trees.
//
// Author: Christophe Cauet
// Date: 2014-02-04 
/******************************************/

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
    // ______________________________________________________________________________________
    // create leaves
    // tag leaves
    var_tag_os_std_comb_leaf_(NULL),
    var_tag_os_std_comb_nozero_leaf_(NULL),
    var_tag_os_std_comb_babar_leaf_(NULL),
    var_tag_os_nozero_leaf_(NULL),
    var_tag_ss_pion_nozero_leaf_(NULL),
    var_tag_os_ss_pion_leaf_(NULL),
    var_tag_os_ss_pion_babar_leaf_(NULL),
    var_tag_os_ss_pion_nozero_leaf_(NULL),
    var_tag_os_exclusive_ss_pion_leaf_(NULL),
    var_tag_os_exclusive_ss_pion_babar_leaf_(NULL),
    var_tag_os_exclusive_ss_pion_nozero_leaf_(NULL),
    var_tag_exclusive_os_ss_pion_leaf_(NULL),
    var_tag_exclusive_os_ss_pion_babar_leaf_(NULL),
    var_tag_exclusive_os_ss_pion_nozero_leaf_(NULL),
    var_tag_exclusive_os_std_comb_ss_pion_leaf_(NULL),
    var_tag_exclusive_os_std_comb_ss_pion_babar_leaf_(NULL),
    var_tag_exclusive_os_std_comb_ss_pion_nozero_leaf_(NULL),
    var_tag_os_with_nnet_kaon_sspion_leaf_(NULL),
    var_tag_os_with_nnet_kaon_sspion_babar_leaf_(NULL),
    var_tag_os_with_nnet_kaon_sspion_nozero_leaf_(NULL),
    var_tag_os_comb_with_nnet_kaon_leaf_(NULL),
    var_tag_os_comb_with_nnet_kaon_babar_leaf_(NULL),
    var_tag_ss_pion_calibrated_leaf_(NULL),
    // eta leaves
    var_tag_eta_os_std_comb_leaf_(NULL),
    var_tag_eta_os_ss_pion_leaf_(NULL),
    var_tag_eta_os_exclusive_ss_pion_leaf_(NULL),
    var_tag_eta_exclusive_os_ss_pion_leaf_(NULL),
    var_tag_eta_exclusive_os_std_comb_ss_pion_leaf_(NULL),
    var_tag_eta_os_comb_with_nnet_kaon_leaf_(NULL),
    var_tag_eta_os_with_nnet_kaon_sspion_leaf_(NULL),
    var_tag_eta_ss_pion_calibrated_leaf_(NULL),
    // cat tagged leaves
    cat_tagged_os_std_comb_leaf_(NULL),
    cat_tagged_os_comb_with_nnet_kaon_leaf_(NULL),
    cat_tagged_os_or_ss_pion_leaf_(NULL),
    cat_tagged_os_xor_ss_pion_leaf_(NULL),
    cat_tagged_os_ss_pion_leaf_(NULL),
    cat_tagged_os_std_comb_ss_pion_calibrated_leaf_(NULL),
    cat_tagged_os_exclusive_ss_pion_leaf_(NULL),
    cat_tagged_exclusive_os_ss_pion_leaf_(NULL),
    cat_tagged_exclusive_os_std_comb_ss_pion_leaf_(NULL),
    cat_tagged_os_with_nnet_kaon_or_sspion_leaf_(NULL),
    cat_tagged_os_with_nnet_kaon_xor_ss_pion_leaf_(NULL),
    cat_tagged_ss_pion_calibrated_leaf_(NULL),
    // ______________________________________________________________________________________
    // leaves to read
    // tag leaves
    var_tag_os_(NULL),
    var_tag_os_muon_(NULL),
    var_tag_os_electron_(NULL),
    var_tag_os_kaon_(NULL),
    var_tag_os_nnet_kaon_(NULL),
    var_tag_os_vtx_charge_(NULL),
    var_tag_ss_pion_(NULL),
    // eta leaves
    var_tag_eta_os_(NULL),
    var_tag_eta_os_muon_(NULL),
    var_tag_eta_os_electron_(NULL),
    var_tag_eta_os_kaon_(NULL),
    var_tag_eta_os_nnet_kaon_(NULL),
    var_tag_eta_os_vtx_charge_(NULL),
    var_tag_eta_ss_pion_(NULL),
    // cat tagged leaves
    cat_tagged_os_(NULL),
    cat_tagged_ss_pion_(NULL),
    // ______________________________________________________________________________________
    // leaves to write
    // tag leaves
    var_tag_os_std_comb_value_(NULL),
    var_tag_os_std_comb_nozero_value_(NULL),
    var_tag_os_std_comb_babar_value_(NULL),
    var_tag_os_nozero_value_(NULL),
    var_tag_ss_pion_nozero_value_(NULL),
    var_tag_os_ss_pion_value_(NULL),
    var_tag_os_ss_pion_babar_value_(NULL),
    var_tag_os_ss_pion_nozero_value_(NULL),
    var_tag_os_exclusive_ss_pion_value_(NULL),
    var_tag_os_exclusive_ss_pion_babar_value_(NULL),
    var_tag_os_exclusive_ss_pion_nozero_value_(NULL),
    var_tag_exclusive_os_ss_pion_value_(NULL),
    var_tag_exclusive_os_ss_pion_babar_value_(NULL),
    var_tag_exclusive_os_ss_pion_nozero_value_(NULL),
    var_tag_exclusive_os_std_comb_ss_pion_value_(NULL),
    var_tag_exclusive_os_std_comb_ss_pion_babar_value_(NULL),
    var_tag_exclusive_os_std_comb_ss_pion_nozero_value_(NULL),
    var_tag_os_with_nnet_kaon_sspion_value_(NULL),
    var_tag_os_with_nnet_kaon_sspion_babar_value_(NULL),
    var_tag_os_with_nnet_kaon_sspion_nozero_value_(NULL),
    var_tag_os_comb_with_nnet_kaon_value_(NULL),
    var_tag_os_comb_with_nnet_kaon_babar_value_(NULL),
    var_tag_ss_pion_calibrated_value_(NULL),
    // eta leaves
    var_tag_eta_os_std_comb_value_(NULL),
    var_tag_eta_os_ss_pion_value_(NULL),
    var_tag_eta_os_exclusive_ss_pion_value_(NULL),
    var_tag_eta_exclusive_os_ss_pion_value_(NULL),
    var_tag_eta_exclusive_os_std_comb_ss_pion_value_(NULL),
    var_tag_eta_os_with_nnet_kaon_sspion_value_(NULL),
    var_tag_eta_os_comb_with_nnet_kaon_value_(NULL),
    var_tag_eta_ss_pion_calibrated_value_(NULL),
    // cat tagged leaves
    cat_tagged_os_std_comb_value_(NULL),
    cat_tagged_os_or_ss_pion_value_(NULL),
    cat_tagged_os_xor_ss_pion_value_(NULL),
    cat_tagged_os_ss_pion_value_(NULL),
    cat_tagged_os_std_comb_ss_pion_calibrated_value_(NULL),
    cat_tagged_os_exclusive_ss_pion_value_(NULL),
    cat_tagged_exclusive_os_ss_pion_value_(NULL),
    cat_tagged_exclusive_os_std_comb_ss_pion_value_(NULL),
    cat_tagged_os_comb_with_nnet_kaon_value_(NULL),
    cat_tagged_os_with_nnet_kaon_or_sspion_value_(NULL),
    cat_tagged_os_with_nnet_kaon_xor_ss_pion_value_(NULL),
    cat_tagged_ss_pion_calibrated_value_(NULL),
    head_("")
  {}
  virtual ~TaggingRdcr(){}
  void set_head(const std::string& head){head_ = head;}
 protected:
  virtual void CreateSpecialBranches();
  virtual bool EntryPassesSpecialCuts();
  virtual void UpdateSpecialLeaves();
 private:
  // ______________________________________________________________________________________
  // create leaves
  // tag leaves
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_std_comb_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_std_comb_nozero_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_std_comb_babar_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_nozero_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_ss_pion_nozero_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_ss_pion_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_ss_pion_babar_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_ss_pion_nozero_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_exclusive_ss_pion_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_exclusive_ss_pion_babar_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_exclusive_ss_pion_nozero_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_exclusive_os_ss_pion_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_exclusive_os_ss_pion_babar_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_exclusive_os_ss_pion_nozero_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_exclusive_os_std_comb_ss_pion_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_exclusive_os_std_comb_ss_pion_babar_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_exclusive_os_std_comb_ss_pion_nozero_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_with_nnet_kaon_sspion_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_with_nnet_kaon_sspion_babar_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_with_nnet_kaon_sspion_nozero_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_comb_with_nnet_kaon_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_os_comb_with_nnet_kaon_babar_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>*    var_tag_ss_pion_calibrated_leaf_;
  // eta leaves
  dooselection::reducer::ReducerLeaf<Double_t>* var_tag_eta_os_std_comb_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* var_tag_eta_os_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* var_tag_eta_os_exclusive_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* var_tag_eta_exclusive_os_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* var_tag_eta_exclusive_os_std_comb_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* var_tag_eta_os_comb_with_nnet_kaon_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* var_tag_eta_os_with_nnet_kaon_sspion_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* var_tag_eta_ss_pion_calibrated_leaf_;
  // cat tagged leaves
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_tagged_os_std_comb_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_tagged_os_comb_with_nnet_kaon_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_tagged_os_or_ss_pion_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_tagged_os_xor_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_tagged_os_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_tagged_os_std_comb_ss_pion_calibrated_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_tagged_os_exclusive_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_tagged_exclusive_os_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_tagged_exclusive_os_std_comb_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_tagged_os_with_nnet_kaon_or_sspion_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_tagged_os_with_nnet_kaon_xor_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>*    cat_tagged_ss_pion_calibrated_leaf_;


  // ______________________________________________________________________________________
  // leaves to read
  // tag leaves
  Int_t* var_tag_os_; 
  Short_t* var_tag_os_muon_;         
  Short_t* var_tag_os_electron_; 
  Short_t* var_tag_os_kaon_; 
  Short_t* var_tag_os_nnet_kaon_; 
  Short_t* var_tag_os_vtx_charge_;            
  Short_t* var_tag_ss_pion_;    
  // eta leaves
  Double_t* var_tag_eta_os_;      
  Float_t* var_tag_eta_os_muon_;      
  Float_t* var_tag_eta_os_electron_; 
  Float_t* var_tag_eta_os_kaon_; 
  Float_t* var_tag_eta_os_nnet_kaon_; 
  Float_t* var_tag_eta_os_vtx_charge_; 
  Float_t* var_tag_eta_ss_pion_;
  // cat tagged leaves
  Int_t* cat_tagged_os_;      
  Int_t* cat_tagged_ss_pion_;

  // ______________________________________________________________________________________
  // leaves to write
  // tag leaves
  Int_t* var_tag_os_std_comb_value_;
  Int_t* var_tag_os_std_comb_nozero_value_;
  Int_t* var_tag_os_std_comb_babar_value_;
  Int_t* var_tag_os_nozero_value_;
  Int_t* var_tag_ss_pion_nozero_value_;
  Int_t* var_tag_os_ss_pion_value_;         
  Int_t* var_tag_os_ss_pion_babar_value_;
  Int_t* var_tag_os_ss_pion_nozero_value_;
  Int_t* var_tag_os_exclusive_ss_pion_value_;
  Int_t* var_tag_os_exclusive_ss_pion_babar_value_;
  Int_t* var_tag_os_exclusive_ss_pion_nozero_value_;
  Int_t* var_tag_exclusive_os_ss_pion_value_;
  Int_t* var_tag_exclusive_os_ss_pion_babar_value_;
  Int_t* var_tag_exclusive_os_ss_pion_nozero_value_;
  Int_t* var_tag_exclusive_os_std_comb_ss_pion_value_;
  Int_t* var_tag_exclusive_os_std_comb_ss_pion_babar_value_;
  Int_t* var_tag_exclusive_os_std_comb_ss_pion_nozero_value_;
  Int_t* var_tag_os_with_nnet_kaon_sspion_value_;
  Int_t* var_tag_os_with_nnet_kaon_sspion_babar_value_;
  Int_t* var_tag_os_with_nnet_kaon_sspion_nozero_value_;
  Int_t* var_tag_os_comb_with_nnet_kaon_value_;
  Int_t* var_tag_os_comb_with_nnet_kaon_babar_value_;
  Int_t* var_tag_ss_pion_calibrated_value_;
  // eta leaves
  Double_t* var_tag_eta_os_std_comb_value_;
  Double_t* var_tag_eta_os_ss_pion_value_;
  Double_t* var_tag_eta_os_exclusive_ss_pion_value_;
  Double_t* var_tag_eta_exclusive_os_ss_pion_value_;
  Double_t* var_tag_eta_exclusive_os_std_comb_ss_pion_value_;
  Double_t* var_tag_eta_os_with_nnet_kaon_sspion_value_;
  Double_t* var_tag_eta_os_comb_with_nnet_kaon_value_; 
  Double_t* var_tag_eta_ss_pion_calibrated_value_;
  // cat tagged leaves
  Int_t* cat_tagged_os_std_comb_value_;
  Int_t* cat_tagged_os_or_ss_pion_value_;  
  Int_t* cat_tagged_os_xor_ss_pion_value_; 
  Int_t* cat_tagged_os_ss_pion_value_;
  Int_t* cat_tagged_os_std_comb_ss_pion_calibrated_value_;
  Int_t* cat_tagged_os_exclusive_ss_pion_value_; 
  Int_t* cat_tagged_exclusive_os_ss_pion_value_;
  Int_t* cat_tagged_exclusive_os_std_comb_ss_pion_value_;
  Int_t* cat_tagged_os_comb_with_nnet_kaon_value_;
  Int_t* cat_tagged_os_with_nnet_kaon_or_sspion_value_;
  Int_t* cat_tagged_os_with_nnet_kaon_xor_ss_pion_value_;
  Int_t* cat_tagged_ss_pion_calibrated_value_;

  // head name
  std::string head_;
};

void TaggingRdcr::CreateSpecialBranches(){
  // ______________________________________________________________________________________
  // Create new leaves
  // tag
  var_tag_os_std_comb_leaf_                           = &CreateIntLeaf("obsTagOS_StdComb");            // 1,-1,0 for std combination
  var_tag_os_std_comb_nozero_leaf_                    = &CreateIntLeaf("obsTagOS_StdComb_NoZero");     // no zero tag but 1 for untagged
  var_tag_os_std_comb_babar_leaf_                     = &CreateIntLeaf("obsTagOS_StdComb_BaBar");      // see two above, tag flipped
  var_tag_os_nozero_leaf_                             = &CreateIntLeaf("obsTagOS_NoZero");            // no zero tag but 1 for untagged
  var_tag_ss_pion_nozero_leaf_                        = &CreateIntLeaf("obsTagSSPion_NoZero");        // no zero tag but 1 for untagged
  var_tag_os_ss_pion_leaf_                            = &CreateIntLeaf("obsTagOSSSPion");             // OS if excl OS, SSPion if excl SSPion, OS+SSPion combination if both, 0 otherwise
  var_tag_os_ss_pion_babar_leaf_                      = &CreateIntLeaf("obsTagOSSSPion_BaBar");       // see above, tag flipped
  var_tag_os_ss_pion_nozero_leaf_                     = &CreateIntLeaf("obsTagOSSSPion_NoZero");      // see above, no zero tag but 1 for untagged
  var_tag_os_exclusive_ss_pion_leaf_                  = &CreateIntLeaf("obsTagOSExclSSPion");         // OS if OS tag available, SSPion if excl SSPion, 0 otherwise
  var_tag_os_exclusive_ss_pion_babar_leaf_            = &CreateIntLeaf("obsTagOSExclSSPion_BaBar");   // see above, tag flipped
  var_tag_os_exclusive_ss_pion_nozero_leaf_           = &CreateIntLeaf("obsTagOSExclSSPion_NoZero");  // see above, no zero tag but 1 for untagged
  var_tag_exclusive_os_ss_pion_leaf_                  = &CreateIntLeaf("obsTagExclOSSSPion");         // OS if excl OS, OS+SSPion combination if SSPion tag available, 0 otherwise
  var_tag_exclusive_os_ss_pion_babar_leaf_            = &CreateIntLeaf("obsTagExclOSSSPion_BaBar");   // see above, tag flipped
  var_tag_exclusive_os_ss_pion_nozero_leaf_           = &CreateIntLeaf("obsTagExclOSSSPion_NoZero");  // see above, no zero tag but 1 for untagged
  var_tag_exclusive_os_std_comb_ss_pion_leaf_         = &CreateIntLeaf("obsTagExclOSSSPion_StdComb");         // OS if excl OS, OS+SSPion combination if SSPion tag available, 0 otherwise
  var_tag_exclusive_os_std_comb_ss_pion_babar_leaf_   = &CreateIntLeaf("obsTagExclOSSSPion_StdComb_BaBar");   // see above, tag flipped
  var_tag_exclusive_os_std_comb_ss_pion_nozero_leaf_  = &CreateIntLeaf("obsTagExclOSSSPion_StdComb_NoZero");  // see above, no zero tag but 1 for untagged
  var_tag_os_comb_with_nnet_kaon_leaf_                = &CreateIntLeaf("obsTagOSwNNKaon");
  var_tag_os_comb_with_nnet_kaon_babar_leaf_          = &CreateIntLeaf("obsTagOSwNNKaon_BaBar");
  var_tag_ss_pion_calibrated_leaf_                    = &CreateIntLeaf("obsTagSSPion_TupleCalib_NoZero");    // tag after foreseen calibration on tuple level
  // eta      
  var_tag_eta_os_std_comb_leaf_                       = &CreateDoubleLeaf("obsEtaOS_StdComb");
  var_tag_eta_os_ss_pion_leaf_                        = &CreateDoubleLeaf("obsEtaOSSSPion");          //
  var_tag_eta_os_exclusive_ss_pion_leaf_              = &CreateDoubleLeaf("obsEtaOSExclSSPion");      //  same as corresponding tag decision
  var_tag_eta_exclusive_os_ss_pion_leaf_              = &CreateDoubleLeaf("obsEtaExclOSSSPion");      //
  var_tag_eta_exclusive_os_std_comb_ss_pion_leaf_     = &CreateDoubleLeaf("obsEtaExclOSSSPion_StdComb");
  var_tag_eta_os_comb_with_nnet_kaon_leaf_            = &CreateDoubleLeaf("obsEtaOSwNNKaon");
  var_tag_eta_ss_pion_calibrated_leaf_                = &CreateDoubleLeaf("obsEtaSSPion_TupleCalib"); // mistag after foreseen calibration on tuple level
  // cat tagged     
  cat_tagged_os_std_comb_leaf_                        = &CreateIntLeaf("catTaggedOS_StdComb");        // 0 for untagged, 1 for tagged
  cat_tagged_os_or_ss_pion_leaf_                      = &CreateIntLeaf("catTaggedOSorSSPion");        // 0 for untagged, 1 for tagged
  cat_tagged_os_xor_ss_pion_leaf_                     = &CreateIntLeaf("catTaggedOSxorSSPion");       // 0 for untagged, 1 for OS tag, -1 for SSPion tag
  cat_tagged_os_ss_pion_leaf_                         = &CreateIntLeaf("catTaggedOSSSPion");          // 0 for untagged, 1 for excl. OS tag, -1 for excl. SSPion tag, 10 for combination of OS and SSPion
  cat_tagged_os_std_comb_ss_pion_calibrated_leaf_     = &CreateIntLeaf("catTaggedOSSSPion_StdComb_TupleCalib"); // 0 for untagged, 1 for excl. OS tag, -1 for excl. SSPion tag, 10 for combination of OS and SSPion
  cat_tagged_os_exclusive_ss_pion_leaf_               = &CreateIntLeaf("catTaggedOSExclSSPion");      // 0 for untagged, 1 for (OS&&SSPion)||OS tag, -1 for SSPion excl. 
  cat_tagged_exclusive_os_ss_pion_leaf_               = &CreateIntLeaf("catTaggedExclOSSSPion");      // 0 for untagged, 1 for excl. OS tag, -1 for (OS&&SSPion)||SSPion tag
  cat_tagged_exclusive_os_std_comb_ss_pion_leaf_      = &CreateIntLeaf("catTaggedExclOSSSPion_StdComb");  // 0 for untagged, 1 for excl. OS tag, -1 for (OS&&SSPion)||SSPion tag (for own std combination)
  cat_tagged_os_comb_with_nnet_kaon_leaf_             = &CreateIntLeaf("catTaggedOSwNNKaon");         // 0 for untagged, 1 for tagged
  cat_tagged_ss_pion_calibrated_leaf_                 = &CreateIntLeaf("catTaggedSSPion_TupleCalib"); // 0 for untagged, 1 for tagged

  // OS NNKaon
  if (LeafExists(head_+"_OS_nnetKaon_DEC")){
    var_tag_os_with_nnet_kaon_sspion_leaf_          = &CreateIntLeaf("obsTagOSwNNKaonSSPion");
    var_tag_os_with_nnet_kaon_sspion_babar_leaf_    = &CreateIntLeaf("obsTagOSwNNKaonSSPion_BaBar");
    var_tag_eta_os_with_nnet_kaon_sspion_leaf_      = &CreateDoubleLeaf("obsEtaOSwNNKaonSSPion");
    cat_tagged_os_with_nnet_kaon_or_sspion_leaf_    = &CreateIntLeaf("catTaggedOSwNNKaonorSSPion");     // 0 for untagged, 1 for tagged
    cat_tagged_os_with_nnet_kaon_xor_ss_pion_leaf_  = &CreateIntLeaf("catTaggedOSwNNKaonxorSSPion");    // 0 for untagged, 1 for OS tag, -1 for SSPion tag
  }

  // ______________________________________________________________________________________
  // Link branch addresses
  // tag
  var_tag_os_                                         = (Int_t*)GetInterimLeafByName(head_+"_TAGDECISION_OS").branch_address();
  var_tag_ss_pion_                                    = (Short_t*)GetInterimLeafByName(head_+"_SS_Pion_DEC").branch_address();
  var_tag_os_muon_                                    = (Short_t*)GetInterimLeafByName(head_+"_OS_Muon_DEC").branch_address();     
  var_tag_os_electron_                                = (Short_t*)GetInterimLeafByName(head_+"_OS_Electron_DEC").branch_address();
  var_tag_os_kaon_                                    = (Short_t*)GetInterimLeafByName(head_+"_OS_Kaon_DEC").branch_address();
  var_tag_os_vtx_charge_                              = (Short_t*)GetInterimLeafByName(head_+"_VtxCharge_DEC").branch_address();
  var_tag_os_std_comb_value_                          = (Int_t*)var_tag_os_std_comb_leaf_->branch_address();
  var_tag_os_std_comb_nozero_value_                   = (Int_t*)var_tag_os_std_comb_nozero_leaf_->branch_address();
  var_tag_os_std_comb_babar_value_                    = (Int_t*)var_tag_os_std_comb_babar_leaf_->branch_address();
  var_tag_os_nozero_value_                            = (Int_t*)var_tag_os_nozero_leaf_->branch_address();
  var_tag_ss_pion_nozero_value_                       = (Int_t*)var_tag_ss_pion_nozero_leaf_->branch_address();
  var_tag_os_ss_pion_value_                           = (Int_t*)var_tag_os_ss_pion_leaf_->branch_address();
  var_tag_os_ss_pion_value_                           = (Int_t*)var_tag_os_ss_pion_leaf_->branch_address();  
  var_tag_os_ss_pion_babar_value_                     = (Int_t*)var_tag_os_ss_pion_babar_leaf_->branch_address();
  var_tag_os_ss_pion_nozero_value_                    = (Int_t*)var_tag_os_ss_pion_nozero_leaf_->branch_address();
  var_tag_os_exclusive_ss_pion_value_                 = (Int_t*)var_tag_os_exclusive_ss_pion_leaf_->branch_address();  
  var_tag_os_exclusive_ss_pion_babar_value_           = (Int_t*)var_tag_os_exclusive_ss_pion_babar_leaf_->branch_address();
  var_tag_os_exclusive_ss_pion_nozero_value_          = (Int_t*)var_tag_os_exclusive_ss_pion_nozero_leaf_->branch_address();
  var_tag_exclusive_os_ss_pion_value_                 = (Int_t*)var_tag_exclusive_os_ss_pion_leaf_->branch_address();  
  var_tag_exclusive_os_ss_pion_babar_value_           = (Int_t*)var_tag_exclusive_os_ss_pion_babar_leaf_->branch_address();
  var_tag_exclusive_os_ss_pion_nozero_value_          = (Int_t*)var_tag_exclusive_os_ss_pion_nozero_leaf_->branch_address();
  var_tag_exclusive_os_std_comb_ss_pion_value_        = (Int_t*)var_tag_exclusive_os_std_comb_ss_pion_leaf_->branch_address();  
  var_tag_exclusive_os_std_comb_ss_pion_babar_value_  = (Int_t*)var_tag_exclusive_os_std_comb_ss_pion_babar_leaf_->branch_address();
  var_tag_exclusive_os_std_comb_ss_pion_nozero_value_ = (Int_t*)var_tag_exclusive_os_std_comb_ss_pion_nozero_leaf_->branch_address();
  var_tag_ss_pion_calibrated_value_                   = (Int_t*)var_tag_ss_pion_calibrated_leaf_->branch_address();
  // eta    
  var_tag_eta_os_                                   = (Double_t*)GetInterimLeafByName(head_+"_TAGOMEGA_OS").branch_address();
  var_tag_eta_ss_pion_                              = (Float_t*)GetInterimLeafByName(head_+"_SS_Pion_PROB").branch_address();
  var_tag_eta_os_muon_                              = (Float_t*)GetInterimLeafByName(head_+"_OS_Muon_PROB").branch_address();      
  var_tag_eta_os_electron_                          = (Float_t*)GetInterimLeafByName(head_+"_OS_Electron_PROB").branch_address();
  var_tag_eta_os_kaon_                              = (Float_t*)GetInterimLeafByName(head_+"_OS_Kaon_PROB").branch_address();     
  var_tag_eta_os_vtx_charge_                        = (Float_t*)GetInterimLeafByName(head_+"_VtxCharge_PROB").branch_address();
  var_tag_eta_os_std_comb_value_                    = (Double_t*)var_tag_eta_os_std_comb_leaf_->branch_address();
  var_tag_eta_os_ss_pion_value_                     = (Double_t*)var_tag_eta_os_ss_pion_leaf_->branch_address();
  var_tag_eta_os_exclusive_ss_pion_value_           = (Double_t*)var_tag_eta_os_exclusive_ss_pion_leaf_->branch_address();
  var_tag_eta_exclusive_os_ss_pion_value_           = (Double_t*)var_tag_eta_exclusive_os_ss_pion_leaf_->branch_address();
  var_tag_eta_exclusive_os_std_comb_ss_pion_value_  = (Double_t*)var_tag_eta_exclusive_os_std_comb_ss_pion_leaf_->branch_address();
  var_tag_eta_ss_pion_calibrated_value_             = (Double_t*)var_tag_eta_ss_pion_calibrated_leaf_->branch_address();
  // cat tagged   
  cat_tagged_os_                                    = (Int_t*)GetInterimLeafByName(head_+"_TAGDECISION_OS").branch_address();
  cat_tagged_ss_pion_                               = (Int_t*)GetInterimLeafByName(head_+"_SS_Pion_DEC").branch_address();
  cat_tagged_os_std_comb_value_                     = (Int_t*)cat_tagged_os_std_comb_leaf_->branch_address();
  cat_tagged_os_or_ss_pion_value_                   = (Int_t*)cat_tagged_os_or_ss_pion_leaf_->branch_address();
  cat_tagged_os_xor_ss_pion_value_                  = (Int_t*)cat_tagged_os_xor_ss_pion_leaf_->branch_address();
  cat_tagged_os_ss_pion_value_                      = (Int_t*)cat_tagged_os_ss_pion_leaf_->branch_address();
  cat_tagged_os_std_comb_ss_pion_calibrated_value_  = (Int_t*)cat_tagged_os_std_comb_ss_pion_calibrated_leaf_->branch_address();
  cat_tagged_os_exclusive_ss_pion_value_            = (Int_t*)cat_tagged_os_exclusive_ss_pion_leaf_->branch_address();
  cat_tagged_exclusive_os_ss_pion_value_            = (Int_t*)cat_tagged_exclusive_os_ss_pion_leaf_->branch_address();
  cat_tagged_exclusive_os_std_comb_ss_pion_value_   = (Int_t*)cat_tagged_exclusive_os_std_comb_ss_pion_leaf_->branch_address();
  cat_tagged_os_comb_with_nnet_kaon_value_          = (Int_t*)cat_tagged_os_comb_with_nnet_kaon_leaf_->branch_address();
  cat_tagged_ss_pion_calibrated_value_              = (Int_t*)cat_tagged_ss_pion_calibrated_leaf_->branch_address();

  // OS NNKaon
  if (LeafExists(head_+"_OS_nnetKaon_DEC")){
    // tag
    var_tag_os_nnet_kaon_                           = (Short_t*)GetInterimLeafByName(head_+"_OS_nnetKaon_DEC").branch_address();
    var_tag_os_with_nnet_kaon_sspion_value_         = (Int_t*)var_tag_os_with_nnet_kaon_sspion_leaf_->branch_address();  
    var_tag_os_with_nnet_kaon_sspion_babar_value_   = (Int_t*)var_tag_os_with_nnet_kaon_sspion_babar_leaf_->branch_address();
    var_tag_os_comb_with_nnet_kaon_value_           = (Int_t*)var_tag_os_comb_with_nnet_kaon_leaf_->branch_address();
    var_tag_os_comb_with_nnet_kaon_babar_value_     = (Int_t*)var_tag_os_comb_with_nnet_kaon_babar_leaf_->branch_address();
    // eta
    var_tag_eta_os_nnet_kaon_                       = (Float_t*)GetInterimLeafByName(head_+"_OS_nnetKaon_PROB").branch_address();
    var_tag_eta_os_with_nnet_kaon_sspion_value_     = (Double_t*)var_tag_eta_os_with_nnet_kaon_sspion_leaf_->branch_address();
    var_tag_eta_os_comb_with_nnet_kaon_value_       = (Double_t*)var_tag_eta_os_comb_with_nnet_kaon_leaf_->branch_address();
    // cat tagged
    cat_tagged_os_with_nnet_kaon_or_sspion_value_   = (Int_t*)cat_tagged_os_with_nnet_kaon_or_sspion_leaf_->branch_address();
    cat_tagged_os_with_nnet_kaon_xor_ss_pion_value_ = (Int_t*)cat_tagged_os_with_nnet_kaon_xor_ss_pion_leaf_->branch_address();
  }
}

bool TaggingRdcr::EntryPassesSpecialCuts(){return true;}

void TaggingRdcr::UpdateSpecialLeaves(){
  // Always using the FT standard combination
  // see page 106 of: https://cds.cern.ch/record/1456804/files/CERN-THESIS-2012-075.pdf
  // as a reference.
  bool debug = false;

  // ______________________________________________________________________________________
  // SSPion calibration ({obsTag,obsEta,catTagged}SSPion_TupleCalib)
  // 
  // since the SSpion is pre-calibrated in new DaVinci tuples this calibration 
  // has to be done afterwards for older tuples:
  // p0 = 0.425
  // p1 = 0.939
  // <eta> = 0.379
  // 
  // Categories:
  //   0: untagged
  //  +1: SSPion tagged
  // ______________________________________________________________________________________

  double m_P0_Cal_SSPion = 0.425;
  double m_P1_Cal_SSPion = 0.939;
  double m_Eta_Cal_SSPion = 0.379;

  double ss_pion_decision = *var_tag_ss_pion_;
  double ss_pion_mistag   = *var_tag_eta_ss_pion_;

  if (ss_pion_mistag >= 0.5){
    *var_tag_ss_pion_calibrated_value_ = 1; 
    *var_tag_eta_ss_pion_calibrated_value_ = 0.5;    
    *cat_tagged_ss_pion_calibrated_value_ = 0;
  }
  else{
    *var_tag_ss_pion_calibrated_value_ = ss_pion_decision; 
    *var_tag_eta_ss_pion_calibrated_value_ = m_P0_Cal_SSPion + m_P1_Cal_SSPion * ( ss_pion_mistag - m_Eta_Cal_SSPion );    
    *cat_tagged_ss_pion_calibrated_value_ = 1;
  }

  if (debug){
    std::cout << "" << std::endl;
    std::cout << "SSPion tuple based calibration" << std::endl;
    std::cout << "SS Pion tag / mistag (with address): \t" << ss_pion_decision << " (" << &ss_pion_decision << ") / " << ss_pion_mistag << " (" << &ss_pion_mistag << ")" << std::endl;
    std::cout << "Calibrated tag / mistag " << *var_tag_ss_pion_calibrated_value_ << " / " << *var_tag_eta_ss_pion_calibrated_value_ << std::endl;
  }

  // ______________________________________________________________________________________
  // Create new OS and SSPion tag observables that not contain zero tags
  // ______________________________________________________________________________________
  *var_tag_os_nozero_value_ = *var_tag_os_;
  *var_tag_ss_pion_nozero_value_ = *var_tag_ss_pion_;
  if ((*var_tag_os_==0)) *var_tag_os_nozero_value_ = 1;
  if ((*var_tag_ss_pion_==0)) *var_tag_ss_pion_nozero_value_ = 1;

  // ______________________________________________________________________________________
  // OS COMBINATION ({obsTag,obsEta,catTagged}OS_StdComb)
  // OS standard combination
  // Categories:
  //   0: untagged
  //  +1: tagged
  // ______________________________________________________________________________________
  double tagdecision=0; 
  double pnsum_a= 0.50; //hypothesis of truetag=+1 
  double pnsum_b= 0.50; //hypothesis of truetag=-1 
  double pnsum= 0.; 

  Double_t osw[4]; 
  Int_t osdec[4]; 
  osw[0] = *var_tag_eta_os_muon_; 
  osw[1] = *var_tag_eta_os_electron_; 
  osw[2] = *var_tag_eta_os_kaon_; 
  osw[3] = *var_tag_eta_os_vtx_charge_; 

  osdec[0] = *var_tag_os_muon_; 
  osdec[1] = *var_tag_os_electron_; 
  osdec[2] = *var_tag_os_kaon_;
  osdec[3] = *var_tag_os_vtx_charge_; 

   // cout << "========> ol2 " << osw[1] << endl; 

  // Ol recompute the combined probability and tag decision for SSK+OS 
  // First recompute the OS decision and mistag, with B0_OS_nnetKaon instead of B0_OS_Kaon 
  // to be sure redefine the variables
  tagdecision=0; 
  pnsum_a= 0.50; //hypothesis of truetag=+1 
  pnsum_b= 0.50; //hypothesis of truetag=-1 
  pnsum= 0.; 

  // calibration for Reco14 (2011 and 2012)
  // see Stefanias email 20140523
  double m_ProbMin_OS = 0.5;
  double m_P0_Cal_OS = 0.390;
  double m_P1_Cal_OS = 0.899;
  double m_Eta_Cal_OS = 0.365;
  
  // no calibration
  // double m_ProbMin_OS = 0.5; 
  // double m_P0_Cal_OS = 0.0; 
  // double m_P1_Cal_OS = 1.0;
  // double m_Eta_Cal_OS = 0.0; 

  for( int i = 0; i != 4; ++i ){ //multiply all probabilities 
    const double mtag = osdec[i]; 
    // std::cout << " in loop, i = " << i << " dec = " << osdec[i] << " w = " << osw[i] << std::endl; 

    if(!mtag) continue; 

    const double pn = 1-osw[i]; //probability of 'right' 
    pnsum_a *= ((1-mtag)/2 + mtag*pn ); // p 
    pnsum_b *= ((1+mtag)/2 - mtag*pn ); //(1-p) 
  } 
  if(pnsum_a > pnsum_b) tagdecision = +1; 
  if(pnsum_a < pnsum_b) tagdecision = -1; 

  //normalise probability to the only two possible flavours: 
  pnsum = std::max(pnsum_a,pnsum_b) /(pnsum_a + pnsum_b); 

  //Calibration (w=1-pn) w' = p0 + p1(w-eta) 
  pnsum = 1 - m_P0_Cal_OS - m_P1_Cal_OS * ( (1-pnsum)-m_Eta_Cal_OS); 

  //throw away poorly significant tags
  if(pnsum < m_ProbMin_OS || tagdecision == 0){ 
    pnsum = 0.50; 
    tagdecision = 0; 
  }
  // B0_TAGDECISION_OS = tagdecision; 
  // B0_TAGOMEGA_OS = 1-pnsum ; 

  *var_tag_os_std_comb_value_ = tagdecision;
  *var_tag_os_std_comb_babar_value_ = -1.*tagdecision; 
  *var_tag_eta_os_std_comb_value_ = 1-pnsum; 
  if (tagdecision==0){
    *var_tag_os_std_comb_nozero_value_ = 1;
    *cat_tagged_os_std_comb_value_ = 0;       
  }
  else{
    *var_tag_os_std_comb_nozero_value_ = tagdecision;
    *cat_tagged_os_std_comb_value_ = 1;
  }

  if (debug){
    std::cout << "" << std::endl;
    std::cout << "OS standard combination" << std::endl;
    std::cout << "OS muon tag / mistag (with address): \t" << osdec[0] << " (" << &osdec[0] << ") / " << osw[0] << " (" << &osw[0] << ")" << std::endl;
    std::cout << "OS electron tag / mistag (with address): " << osdec[1] << " (" << &osdec[1] << ") / " << osw[1] << " (" << &osw[1] << ")" << std::endl;
    std::cout << "OS kaon tag / mistag (with address): \t" << osdec[2] << " (" << &osdec[2] << ") / " << osw[2] << " (" << &osw[2] << ")" << std::endl;
    std::cout << "OS VtxQ tag / mistag (with address): \t" << osdec[3] << " (" << &osdec[3] << ") / " << osw[3] << " (" << &osw[3] << ")" << std::endl;
    std::cout << "Comb tag / mistag " << *var_tag_os_std_comb_value_ << " / " << *var_tag_eta_os_std_comb_value_ << std::endl;
  }

  // ______________________________________________________________________________________
  // OS+SS COMBINATION ({obsTag,obsEta,catTagged}OSSSPion)
  // OS (excl) + SSPion (excl) + standard combination
  // Categories:
  //   0: untagged
  //  +1: OS excl tagged
  //  -1: SSPion excl tagged
  //  10: OS+SSPion combination
  // ______________________________________________________________________________________

  if ((*var_tag_os_==0) && (*var_tag_ss_pion_==0)){           // if OS and SSPion tags are 0, set everything to untagged
    *var_tag_os_ss_pion_value_        = 0;
    *var_tag_os_ss_pion_babar_value_  = 0;
    *var_tag_os_ss_pion_nozero_value_ = 1;
    *var_tag_eta_os_ss_pion_value_    = 0.5;
    *cat_tagged_os_ss_pion_value_     = 0;
  }
  else if ((*var_tag_os_!=0) && (*var_tag_ss_pion_==0)){      // if OS tagger exclusively has tag, write OS tag to combination
    *var_tag_os_ss_pion_value_        = *var_tag_os_;
    *var_tag_os_ss_pion_babar_value_  = -(*var_tag_os_);
    *var_tag_os_ss_pion_nozero_value_ = *var_tag_os_;
    *var_tag_eta_os_ss_pion_value_    = *var_tag_eta_os_;
    *cat_tagged_os_ss_pion_value_     = 1;
  }
  else if ((*var_tag_os_==0) && (*var_tag_ss_pion_!=0)){      // if SSPion tagger exclusively has tag, write SSPion tag to combination
    *var_tag_os_ss_pion_value_        = *var_tag_ss_pion_;
    *var_tag_os_ss_pion_babar_value_  = -(*var_tag_ss_pion_);
    *var_tag_os_ss_pion_nozero_value_ = *var_tag_ss_pion_;
    *var_tag_eta_os_ss_pion_value_    = *var_tag_eta_ss_pion_;
    *cat_tagged_os_ss_pion_value_     = -1;
  }
  else{                                                       // else, combine OS and SSPion tag decision and eta
    int os_dec = *var_tag_os_;
    int ss_dec = *var_tag_ss_pion_;
    double os_mistag = *var_tag_eta_os_;
    double ss_mistag = *var_tag_eta_ss_pion_;

    double os_prob_b = (1.+os_dec)/2. - os_dec * (1.-os_mistag);
    double os_prob_bbar = (1.-os_dec)/2. + os_dec * (1.-os_mistag);
    double ss_prob_b = (1.+ss_dec)/2. - ss_dec * (1.-ss_mistag);
    double ss_prob_bbar = (1.-ss_dec)/2. + ss_dec * (1.-ss_mistag);

    double prob_b = os_prob_b * ss_prob_b;
    double prob_bbar = os_prob_bbar * ss_prob_bbar;

    double comb_prob_b = prob_b / (prob_b + prob_bbar);
    double comb_prob_bbar = 1 - comb_prob_b;

    // tag decision
    if (comb_prob_b > comb_prob_bbar) *var_tag_os_ss_pion_value_        = -1;   // probability for b quark content is larger than for bbar quark
    if (comb_prob_b < comb_prob_bbar) *var_tag_os_ss_pion_value_        = +1;   // probability for b quark content is smaller than for bbar quark
    if (comb_prob_b > comb_prob_bbar) *var_tag_os_ss_pion_babar_value_  = +1;   // vice versa for babar notation
    if (comb_prob_b < comb_prob_bbar) *var_tag_os_ss_pion_babar_value_  = -1;   //
    if (comb_prob_b > comb_prob_bbar) *var_tag_os_ss_pion_nozero_value_ = -1;   // the same, just for the variable containing no zero entries
    if (comb_prob_b < comb_prob_bbar) *var_tag_os_ss_pion_nozero_value_ = +1;   //

    // mistag
    double comb_prob = std::max(comb_prob_b,comb_prob_bbar) /(comb_prob_b + comb_prob_bbar);
    *var_tag_eta_os_ss_pion_value_ = 1. - comb_prob;

    // tag category
    *cat_tagged_os_ss_pion_value_ = 10;

    if (debug){
      std::cout << "" << std::endl;
      std::cout << "OSSSPion combination" << std::endl;
      std::cout << "OS / SS tag address " << var_tag_os_ << " / " << var_tag_ss_pion_ << std::endl;
      std::cout << "OS / SS mistag address " << var_tag_eta_os_ << " / " << var_tag_eta_ss_pion_ << std::endl;
      std::cout << "OS tag / mistag " << os_dec << " / " << os_mistag << std::endl;
      std::cout << "SS tag / mistag " << ss_dec << " / " << ss_mistag << std::endl;
      std::cout << "Comb tag / mistag " << *var_tag_os_ss_pion_value_ << " / " << *var_tag_eta_os_ss_pion_value_ << std::endl;
    }
  }

  // ______________________________________________________________________________________
  // OS+SS COMBINATION ({obsTag,obsEta,catTagged}OSExclSSPion)
  // OS + SSPion (excl)
  // Categories:
  //   0: untagged
  //  +1: OS tagged
  //  -1: SSPion excl tagged
  // ______________________________________________________________________________________

  if ((*var_tag_os_==0) && (*var_tag_ss_pion_==0)){           // if OS and SSPion tags are 0, set everything to untagged
    *var_tag_os_exclusive_ss_pion_value_        = 0;
    *var_tag_os_exclusive_ss_pion_babar_value_  = 0;
    *var_tag_os_exclusive_ss_pion_nozero_value_ = 1;
    *var_tag_eta_os_exclusive_ss_pion_value_    = 0.5;
    *cat_tagged_os_exclusive_ss_pion_value_     = 0;
  }
  else if ((*var_tag_os_==0) && (*var_tag_ss_pion_!=0)){      // if SSPion tagger exclusively has tag, write SSPion tag to combination
    *var_tag_os_exclusive_ss_pion_value_        = *var_tag_ss_pion_;
    *var_tag_os_exclusive_ss_pion_babar_value_  = -(*var_tag_ss_pion_);
    *var_tag_os_exclusive_ss_pion_nozero_value_ = *var_tag_ss_pion_;
    *var_tag_eta_os_exclusive_ss_pion_value_    = *var_tag_eta_ss_pion_;
    *cat_tagged_os_exclusive_ss_pion_value_     = -1;
  }
  else{                                                       // else, combine OS and SSPion tag decision and eta
    int os_dec = *var_tag_os_;
    int ss_dec = *var_tag_ss_pion_;
    double os_mistag = *var_tag_eta_os_;
    double ss_mistag = *var_tag_eta_ss_pion_;

    double os_prob_b = (1.+os_dec)/2. - os_dec * (1.-os_mistag);
    double os_prob_bbar = (1.-os_dec)/2. + os_dec * (1.-os_mistag);
    double ss_prob_b = (1.+ss_dec)/2. - ss_dec * (1.-ss_mistag);
    double ss_prob_bbar = (1.-ss_dec)/2. + ss_dec * (1.-ss_mistag);

    double prob_b = os_prob_b * ss_prob_b;
    double prob_bbar = os_prob_bbar * ss_prob_bbar;

    double comb_prob_b = prob_b / (prob_b + prob_bbar);
    double comb_prob_bbar = 1 - comb_prob_b;

    // tag decision
    if (comb_prob_b > comb_prob_bbar) *var_tag_os_exclusive_ss_pion_value_        = -1;   // probability for b quark content is larger than for bbar quark
    if (comb_prob_b < comb_prob_bbar) *var_tag_os_exclusive_ss_pion_value_        = +1;   // probability for b quark content is smaller than for bbar quark
    if (comb_prob_b > comb_prob_bbar) *var_tag_os_exclusive_ss_pion_babar_value_  = +1;   // vice versa for babar notation
    if (comb_prob_b < comb_prob_bbar) *var_tag_os_exclusive_ss_pion_babar_value_  = -1;   // 
    if (comb_prob_b > comb_prob_bbar) *var_tag_os_exclusive_ss_pion_nozero_value_ = -1;   // the same, just for the variable containing no zero entries
    if (comb_prob_b < comb_prob_bbar) *var_tag_os_exclusive_ss_pion_nozero_value_ = +1;   //

    // mistag
    double comb_prob = std::max(comb_prob_b,comb_prob_bbar) /(comb_prob_b + comb_prob_bbar);
    *var_tag_eta_os_exclusive_ss_pion_value_ = 1. - comb_prob;

    // tag category
    *cat_tagged_os_exclusive_ss_pion_value_ = 1;

    if (debug){
      std::cout << "" << std::endl;
      std::cout << "OSExclSSPion combination" << std::endl;
      std::cout << "OS / SS tag address " << var_tag_os_ << " / " << var_tag_ss_pion_ << std::endl;
      std::cout << "OS / SS mistag address " << var_tag_eta_os_ << " / " << var_tag_eta_ss_pion_ << std::endl;
      std::cout << "OS tag / mistag " << os_dec << " / " << os_mistag << std::endl;
      std::cout << "SS tag / mistag " << ss_dec << " / " << ss_mistag << std::endl;
      std::cout << "Comb tag / mistag " << *var_tag_os_exclusive_ss_pion_value_ << " / " << *var_tag_eta_os_exclusive_ss_pion_value_ << std::endl;
    }
  }

  // ______________________________________________________________________________________
  // OS+SS COMBINATION ({obsTag,obsEta,catTagged}ExclOSSSPion)
  // OS (excl) + SSPion
  // Categories:
  //   0: untagged
  //  +1: OS excl tagged
  //  -1: SSPion tagged
  // ______________________________________________________________________________________

  if ((*var_tag_os_==0) && (*var_tag_ss_pion_==0)){           // if OS and SSPion tags are 0, set everything to untagged
    *var_tag_exclusive_os_ss_pion_value_        = 0;
    *var_tag_exclusive_os_ss_pion_babar_value_  = 0;
    *var_tag_exclusive_os_ss_pion_nozero_value_ = 1;
    *var_tag_eta_exclusive_os_ss_pion_value_    = 0.5;
    *cat_tagged_exclusive_os_ss_pion_value_     = 0;
  }
  else if ((*var_tag_os_!=0) && (*var_tag_ss_pion_==0)){      // if OS tagger exclusively has tag, write OS tag to combination
    *var_tag_exclusive_os_ss_pion_value_        = *var_tag_os_;
    *var_tag_exclusive_os_ss_pion_babar_value_  = -(*var_tag_os_);
    *var_tag_exclusive_os_ss_pion_nozero_value_ = *var_tag_os_;
    *var_tag_eta_exclusive_os_ss_pion_value_    = *var_tag_eta_os_;
    *cat_tagged_exclusive_os_ss_pion_value_     = 1;
  }
  else{                                                       // else, combine OS and SSPion tag decision and eta
    int os_dec = *var_tag_os_;
    int ss_dec = *var_tag_ss_pion_;
    double os_mistag = *var_tag_eta_os_;
    double ss_mistag = *var_tag_eta_ss_pion_;

    double os_prob_b = (1.+os_dec)/2. - os_dec * (1.-os_mistag);
    double os_prob_bbar = (1.-os_dec)/2. + os_dec * (1.-os_mistag);
    double ss_prob_b = (1.+ss_dec)/2. - ss_dec * (1.-ss_mistag);
    double ss_prob_bbar = (1.-ss_dec)/2. + ss_dec * (1.-ss_mistag);

    double prob_b = os_prob_b * ss_prob_b;
    double prob_bbar = os_prob_bbar * ss_prob_bbar;

    double comb_prob_b = prob_b / (prob_b + prob_bbar);
    double comb_prob_bbar = 1 - comb_prob_b;

    // tag decision
    if (comb_prob_b > comb_prob_bbar) *var_tag_exclusive_os_ss_pion_value_        = -1;   // probability for b quark content is larger than for bbar quark
    if (comb_prob_b < comb_prob_bbar) *var_tag_exclusive_os_ss_pion_value_        = +1;   // probability for b quark content is smaller than for bbar quark
    if (comb_prob_b > comb_prob_bbar) *var_tag_exclusive_os_ss_pion_babar_value_  = +1;   // vice versa for babar notation
    if (comb_prob_b < comb_prob_bbar) *var_tag_exclusive_os_ss_pion_babar_value_  = -1;   // 
    if (comb_prob_b > comb_prob_bbar) *var_tag_exclusive_os_ss_pion_nozero_value_ = -1;   // the same, just for the variable containing no zero entries
    if (comb_prob_b < comb_prob_bbar) *var_tag_exclusive_os_ss_pion_nozero_value_ = +1;   //

    // mistag
    double comb_prob = std::max(comb_prob_b,comb_prob_bbar) /(comb_prob_b + comb_prob_bbar);
    *var_tag_eta_exclusive_os_ss_pion_value_ = 1. - comb_prob;

    // tag category
    *cat_tagged_exclusive_os_ss_pion_value_ = -1;

    if (debug){
      std::cout << "" << std::endl;
      std::cout << "ExclOSSSPion combination" << std::endl;
      std::cout << "OS / SS tag address " << var_tag_os_ << " / " << var_tag_ss_pion_ << std::endl;
      std::cout << "OS / SS mistag address " << var_tag_eta_os_ << " / " << var_tag_eta_ss_pion_ << std::endl;
      std::cout << "OS tag / mistag " << os_dec << " / " << os_mistag << std::endl;
      std::cout << "SS tag / mistag " << ss_dec << " / " << ss_mistag << std::endl;
      std::cout << "Comb tag / mistag " << *var_tag_exclusive_os_ss_pion_value_ << " / " << *var_tag_eta_exclusive_os_ss_pion_value_ << std::endl;
    }
  }

  // ______________________________________________________________________________________
  // OS+SS COMBINATION with own standard combination ({obsTag,obsEta,catTagged}ExclOSSSPion_StdComb{_NoZero, _BaBar})
  // OS (excl) + SSPion
  // Categories:
  //   0: untagged
  //  +1: OS excl tagged
  //  -1: SSPion tagged
  // ______________________________________________________________________________________
  // SPECIAL CAT: additionaly fill catTaggedOSSSPion_StdComb_TupleCalib
  // ______________________________________________________________________________________

  if ((*var_tag_os_std_comb_value_==0) && (*var_tag_ss_pion_==0)){           // if OS and SSPion tags are 0, set everything to untagged
    *var_tag_exclusive_os_std_comb_ss_pion_value_        = 0;
    *var_tag_exclusive_os_std_comb_ss_pion_babar_value_  = 0;
    *var_tag_exclusive_os_std_comb_ss_pion_nozero_value_ = 1;
    *var_tag_eta_exclusive_os_std_comb_ss_pion_value_    = 0.5;
    *cat_tagged_exclusive_os_std_comb_ss_pion_value_     = 0; 
    *cat_tagged_os_std_comb_ss_pion_calibrated_value_    = 0; // (SPECIAL CAT)
  }
  else if ((*var_tag_os_std_comb_value_!=0) && (*var_tag_ss_pion_==0)){      // if OS tagger exclusively has tag, write OS tag to combination
    *var_tag_exclusive_os_std_comb_ss_pion_value_        = *var_tag_os_std_comb_value_;
    *var_tag_exclusive_os_std_comb_ss_pion_babar_value_  = -(*var_tag_os_std_comb_value_);
    *var_tag_exclusive_os_std_comb_ss_pion_nozero_value_ = *var_tag_os_std_comb_value_;
    *var_tag_eta_exclusive_os_std_comb_ss_pion_value_    = *var_tag_eta_os_std_comb_value_;
    *cat_tagged_exclusive_os_std_comb_ss_pion_value_     = 1;
    *cat_tagged_os_std_comb_ss_pion_calibrated_value_ =  1; // (SPECIAL CAT)
  }
  else{                                                       // else, combine OS and SSPion tag decision and eta

    if ((*var_tag_os_std_comb_value_==0) && (*var_tag_ss_pion_!=0)){ // (SPECIAL CAT)
      *cat_tagged_os_std_comb_ss_pion_calibrated_value_ = -1;
    }
    else{
      *cat_tagged_os_std_comb_ss_pion_calibrated_value_ = 10;
    }

    int os_dec = *var_tag_os_std_comb_value_;
    int ss_dec = *var_tag_ss_pion_;
    double os_mistag = *var_tag_eta_os_std_comb_value_;
    double ss_mistag = *var_tag_eta_ss_pion_;

    double os_prob_b = (1.+os_dec)/2. - os_dec * (1.-os_mistag);
    double os_prob_bbar = (1.-os_dec)/2. + os_dec * (1.-os_mistag);
    double ss_prob_b = (1.+ss_dec)/2. - ss_dec * (1.-ss_mistag);
    double ss_prob_bbar = (1.-ss_dec)/2. + ss_dec * (1.-ss_mistag);

    double prob_b = os_prob_b * ss_prob_b;
    double prob_bbar = os_prob_bbar * ss_prob_bbar;

    double comb_prob_b = prob_b / (prob_b + prob_bbar);
    double comb_prob_bbar = 1 - comb_prob_b;

    // tag decision
    if (comb_prob_b > comb_prob_bbar) *var_tag_exclusive_os_std_comb_ss_pion_value_        = -1;   // probability for b quark content is larger than for bbar quark
    if (comb_prob_b < comb_prob_bbar) *var_tag_exclusive_os_std_comb_ss_pion_value_        = +1;   // probability for b quark content is smaller than for bbar quark
    if (comb_prob_b > comb_prob_bbar) *var_tag_exclusive_os_std_comb_ss_pion_babar_value_  = +1;   // vice versa for babar notation
    if (comb_prob_b < comb_prob_bbar) *var_tag_exclusive_os_std_comb_ss_pion_babar_value_  = -1;   // 
    if (comb_prob_b > comb_prob_bbar) *var_tag_exclusive_os_std_comb_ss_pion_nozero_value_ = -1;   // the same, just for the variable containing no zero entries
    if (comb_prob_b < comb_prob_bbar) *var_tag_exclusive_os_std_comb_ss_pion_nozero_value_ = +1;   //

    // mistag
    double comb_prob = std::max(comb_prob_b,comb_prob_bbar) /(comb_prob_b + comb_prob_bbar);
    *var_tag_eta_exclusive_os_std_comb_ss_pion_value_ = 1. - comb_prob;

    // tag category
    *cat_tagged_exclusive_os_std_comb_ss_pion_value_  = -1;

    if (debug){
      std::cout << "" << std::endl;
      std::cout << "ExclOSSSPion_StdComb combination" << std::endl;
      std::cout << "OS / SS tag address " << var_tag_os_std_comb_value_ << " / " << var_tag_ss_pion_ << std::endl;
      std::cout << "OS / SS mistag address " << var_tag_eta_os_std_comb_value_ << " / " << var_tag_eta_ss_pion_ << std::endl;
      std::cout << "OS tag / mistag " << os_dec << " / " << os_mistag << std::endl;
      std::cout << "SS tag / mistag " << ss_dec << " / " << ss_mistag << std::endl;
      std::cout << "Comb tag / mistag " << *var_tag_exclusive_os_std_comb_ss_pion_value_ << " / " << *var_tag_eta_exclusive_os_std_comb_ss_pion_value_ << std::endl;
    }
  }

  // ______________________________________________________________________________________
  // OS with neural net kaon
  // ______________________________________________________________________________________

  if (LeafExists("B0_OS_nnetKaon_DEC")){
    // ______________________________________________________________________________________
    // OS with neural net kaon combination ({obsTag,obsEta,catTagged}OSwNNKaon)
    // Categories:
    //   0: untagged
    //  +1: tagged
    // ______________________________________________________________________________________

    // Get original tagging decision and mistag from the DTT 
    // in order to recompute OS and total decision and mistag, with nnetKaon (OS and SS) instead of classic ones 
    Double_t osw[4]; 
    Int_t osdec[4]; 
    osw[0] = *var_tag_eta_os_muon_; 
    osw[1] = *var_tag_eta_os_electron_; 
    osw[2] = *var_tag_eta_os_nnet_kaon_; 
    //osw[2] = B0_OS_Kaon_PROB; /// ---- cut based 
    osw[3] = *var_tag_eta_os_vtx_charge_; 

    osdec[0] = *var_tag_os_muon_; 
    osdec[1] = *var_tag_os_electron_; 
    osdec[2] = *var_tag_os_nnet_kaon_; 
    //osdec[2] = B0_OS_Kaon_DEC; /// ---- cut based 
    osdec[3] = *var_tag_os_vtx_charge_; 

     // cout << "========> ol2 " << osw[1] << endl; 

    // Ol recompute the combined probability and tag decision for SSK+OS 
    // First recompute the OS decision and mistag, with B0_OS_nnetKaon instead of B0_OS_Kaon 
    // to be sure redefine the variables
    tagdecision=0; 
    pnsum_a= 0.50; //hypothesis of truetag=+1 
    pnsum_b= 0.50; //hypothesis of truetag=-1 
    pnsum= 0.; 

    // calibration for Reco14 (2011 and 2012)
    // see Stefanias email 20131012
    double m_ProbMin_OS = 0.5; 
    double m_P0_Cal_OS = 0.423; 
    double m_P1_Cal_OS = 0.875;
    double m_Eta_Cal_OS = 0.403; 

    for( int i = 0; i != 4; ++i ){ //multiply all probabilities 
      const double mtag = osdec[i]; 
      // std::cout << " in loop, i = " << i << " dec = " << osdec[i] << " w = " << osw[i] << std::endl; 

      if(!mtag) continue; 

      const double pn = 1-osw[i]; //probability of 'right' 
      pnsum_a *= ((1-mtag)/2 + mtag*pn ); // p 
      pnsum_b *= ((1+mtag)/2 - mtag*pn ); //(1-p) 
    } 
    if(pnsum_a > pnsum_b) tagdecision = +1; 
    if(pnsum_a < pnsum_b) tagdecision = -1; 

    //normalise probability to the only two possible flavours: 
    pnsum = std::max(pnsum_a,pnsum_b) /(pnsum_a + pnsum_b); 

    //Calibration (w=1-pn) w' = p0 + p1(w-eta) 
    pnsum = 1 - m_P0_Cal_OS - m_P1_Cal_OS * ( (1-pnsum)-m_Eta_Cal_OS); 

    //throw away poorly significant tags 
    if(pnsum < m_ProbMin_OS || tagdecision == 0){ 
      pnsum = 0.50; 
      tagdecision = 0; 
    }
    // B0_TAGDECISION_OS = tagdecision; 
    // B0_TAGOMEGA_OS = 1-pnsum ; 

    *var_tag_os_comb_with_nnet_kaon_value_ = tagdecision;       
    *var_tag_os_comb_with_nnet_kaon_babar_value_ = -1.*tagdecision; 
    *var_tag_eta_os_comb_with_nnet_kaon_value_ = 1-pnsum; 
    if (tagdecision==0){
      *cat_tagged_os_comb_with_nnet_kaon_value_ = 0;       
    }
    else{
      *cat_tagged_os_comb_with_nnet_kaon_value_ = 1;
    }

  // ______________________________________________________________________________________
  // OS with neural net kaon + SS COMBINATION ({obsTag,obsEta,catTagged}OSwNNKaonSSPion)
  // OS (excl) + SSPion
  // Categories:
  //   0: untagged
  //  +1: OS excl tagged
  //  -1: SSPion tagged
  // ______________________________________________________________________________________

    if (tagdecision!=0){ // if OS tagger has tag, write OS tag to combination and 
      *var_tag_os_with_nnet_kaon_sspion_value_ = tagdecision;
      *var_tag_os_with_nnet_kaon_sspion_babar_value_ = -1.*tagdecision;
      *cat_tagged_os_with_nnet_kaon_or_sspion_value_ = 1;
      *cat_tagged_os_with_nnet_kaon_xor_ss_pion_value_ = 1;
      *var_tag_eta_os_with_nnet_kaon_sspion_value_ = 1-pnsum;
    }
    else if ((tagdecision==0) && (*var_tag_ss_pion_!=0)){
      *var_tag_os_with_nnet_kaon_sspion_value_ = *var_tag_ss_pion_;
      *var_tag_os_with_nnet_kaon_sspion_babar_value_ = -(*var_tag_ss_pion_);
      *cat_tagged_os_with_nnet_kaon_or_sspion_value_ = 1;
      *cat_tagged_os_with_nnet_kaon_xor_ss_pion_value_ = -1;
      *var_tag_eta_os_with_nnet_kaon_sspion_value_ = *var_tag_eta_ss_pion_;
    }
    else{
      *var_tag_os_with_nnet_kaon_sspion_value_ = 0;
      *var_tag_os_with_nnet_kaon_sspion_babar_value_ = 0;
      *cat_tagged_os_with_nnet_kaon_or_sspion_value_ = 0;
      *cat_tagged_os_with_nnet_kaon_xor_ss_pion_value_ = 0;
      *var_tag_eta_os_with_nnet_kaon_sspion_value_ = 0.5;
    }
  }
}

int B2JpsiKS(const std::string& inputfile, const std::string& inputtree, const std::string& outputfile, const std::string& outputtree, const std::string& decay_channel);

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "FlavourTaggingGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, decay_channel;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    decay_channel = argv[5];
    // decay_channel = "Bd2JpsiKS"; // always use this channel for now
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "FlavourTaggingGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "FlavourTaggingGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name decay_channel" << doocore::io::endmsg;
    return 1;
  }

  if (decay_channel=="Bd2JpsiKS" || decay_channel=="Bs2JpsiKS"){
    doocore::io::sinfo << "-info-  \t" << "FlavourTaggingGrimReaper \t" << "Decay: " << decay_channel << doocore::io::endmsg;
    B2JpsiKS(inputfile, inputtree, outputfile, outputtree, decay_channel);
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "FlavourTaggingGrimReaper \t" << "'" << decay_channel << "' is no valid decay channel. Possible decay channels are:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "FlavourTaggingGrimReaper \t" << "- Bd2JpsiKS" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "FlavourTaggingGrimReaper \t" << "- Bs2JpsiKS" << doocore::io::endmsg;
  }
  doocore::io::sinfo << "-info- \t" << "FlavourTaggingGrimReaper \t" << "Done!" << doocore::io::endmsg;
}

int B2JpsiKS(const std::string& inputfile, const std::string& inputtree, const std::string& outputfile, const std::string& outputtree, const std::string& decay_channel){
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

  reducer.Initialize();

  // handle MC specific stuff
  if (reducer.LeafExists(head+"_BKGCAT") && reducer.LeafExists(head+"_TRUEID")){
    // flavour tagging true tag
    ReducerLeaf<Int_t>& vtag_true_leaf = reducer.CreateIntLeaf("obsTag_True", 0);
    vtag_true_leaf.AddCondition("B0",    head+"_TRUEID>0", +1);
    vtag_true_leaf.AddCondition("B0bar", head+"_TRUEID<0", -1);

    ReducerLeaf<Int_t>& vtag_true_babar_leaf = reducer.CreateIntLeaf("obsTag_BaBar_True", 0);
    vtag_true_babar_leaf.AddCondition("B0",    head+"_TRUEID>0", -1);
    vtag_true_babar_leaf.AddCondition("B0bar", head+"_TRUEID<0", +1);

    ReducerLeaf<Int_t>& vtag_true_nozero_leaf = reducer.CreateIntLeaf("obsTag_NoZero_True", 1);
    vtag_true_nozero_leaf.AddCondition("B0",    head+"_TRUEID>0", +1);
    vtag_true_nozero_leaf.AddCondition("B0bar", head+"_TRUEID<0", -1);
  }

  // catTagger
  ReducerLeaf<Int_t>& var_tagger_leaf = reducer.CreateIntCopyLeaf("catTagger", reducer.GetInterimLeafByName(head+"_TAGGER"));

  // obsTag
  ReducerLeaf<Int_t>& var_tag_all_leaf = reducer.CreateIntCopyLeaf("obsTagAll", reducer.GetInterimLeafByName(head+"_TAGDECISION"));
  ReducerLeaf<Int_t>& var_tag_all_babar_leaf = reducer.CreateIntCopyLeaf("obsTagAll_BaBar", reducer.GetInterimLeafByName(head+"_TAGDECISION"), -1.0);
  ReducerLeaf<Int_t>& var_tag_os_leaf = reducer.CreateIntCopyLeaf("obsTagOS", reducer.GetInterimLeafByName(head+"_TAGDECISION_OS"));
  ReducerLeaf<Int_t>& var_tag_os_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOS_BaBar", reducer.GetInterimLeafByName(head+"_TAGDECISION_OS"), -1.0);
  ReducerLeaf<Int_t>& var_tag_os_mu_leaf = reducer.CreateIntCopyLeaf("obsTagOSMuon", reducer.GetInterimLeafByName(head+"_OS_Muon_DEC"));
  ReducerLeaf<Int_t>& var_tag_os_mu_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOSMuon_BaBar", reducer.GetInterimLeafByName(head+"_OS_Muon_DEC"), -1.0);
  ReducerLeaf<Int_t>& var_tag_os_electron_leaf = reducer.CreateIntCopyLeaf("obsTagOSElectron", reducer.GetInterimLeafByName(head+"_OS_Electron_DEC"));
  ReducerLeaf<Int_t>& var_tag_os_electron_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOSElectron_BaBar", reducer.GetInterimLeafByName(head+"_OS_Electron_DEC"), -1.0);
  ReducerLeaf<Int_t>& var_tag_os_kaon_leaf = reducer.CreateIntCopyLeaf("obsTagOSKaon", reducer.GetInterimLeafByName(head+"_OS_Kaon_DEC"));
  if (reducer.LeafExists(head+"_OS_nnetKaon_DEC")){
    ReducerLeaf<Int_t>& var_tag_os_kaon_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOSKaon_BaBar", reducer.GetInterimLeafByName(head+"_OS_Kaon_DEC"), -1.0);
    ReducerLeaf<Int_t>& var_tag_os_nnet_kaon_leaf = reducer.CreateIntCopyLeaf("obsTagOSNNKaon", reducer.GetInterimLeafByName(head+"_OS_nnetKaon_DEC"));
    ReducerLeaf<Int_t>& var_tag_os_nnet_kaon_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOSNNKaon_BaBar", reducer.GetInterimLeafByName(head+"_OS_nnetKaon_DEC"), -1.0);
  }
  ReducerLeaf<Int_t>& var_tag_ss_kaon_leaf = reducer.CreateIntCopyLeaf("obsTagSSKaon", reducer.GetInterimLeafByName(head+"_SS_Kaon_DEC"));
  ReducerLeaf<Int_t>& var_tag_ss_kaon_babar_leaf = reducer.CreateIntCopyLeaf("obsTagSSKaon_BaBar", reducer.GetInterimLeafByName(head+"_SS_Kaon_DEC"), -1.0);
  if (reducer.LeafExists(head+"_SS_nnetKaon_DEC")){
    ReducerLeaf<Int_t>& var_tag_ss_nnet_kaon_leaf = reducer.CreateIntCopyLeaf("obsTagSSNNKaon", reducer.GetInterimLeafByName(head+"_SS_nnetKaon_DEC"));
    ReducerLeaf<Int_t>& var_tag_ss_nnet_kaon_babar_leaf = reducer.CreateIntCopyLeaf("obsTagSSNNKaon_BaBar", reducer.GetInterimLeafByName(head+"_SS_nnetKaon_DEC"), -1.0);
  }
  ReducerLeaf<Int_t>& var_tag_ss_pion_leaf = reducer.CreateIntCopyLeaf("obsTagSSPion", reducer.GetInterimLeafByName(head+"_SS_Pion_DEC"));
  ReducerLeaf<Int_t>& var_tag_ss_pion_babar_leaf = reducer.CreateIntCopyLeaf("obsTagSSPion_BaBar", reducer.GetInterimLeafByName(head+"_SS_Pion_DEC"), -1.0);
  ReducerLeaf<Int_t>& var_tag_vtxq_leaf = reducer.CreateIntCopyLeaf("obsTagVtxQ", reducer.GetInterimLeafByName(head+"_VtxCharge_DEC"));
  ReducerLeaf<Int_t>& var_tag_vtxq_babar_leaf = reducer.CreateIntCopyLeaf("obsTagVtxQ_BaBar", reducer.GetInterimLeafByName(head+"_VtxCharge_DEC"), -1.0);
  if (reducer.LeafExists(head+"_OS_Charm_DEC")){
    ReducerLeaf<Int_t>& var_tag_os_charm_leaf = reducer.CreateIntCopyLeaf("obsTagOSCharm", reducer.GetInterimLeafByName(head+"_OS_Charm_DEC"));
    ReducerLeaf<Int_t>& var_tag_os_charm_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOSCharm_BaBar", reducer.GetInterimLeafByName(head+"_OS_Charm_DEC"), -1.0);  
  }
  if (reducer.LeafExists(head+"_SS_Proton_DEC")){
    ReducerLeaf<Int_t>& var_tag_ss_proton_leaf = reducer.CreateIntCopyLeaf("obsTagSSProton", reducer.GetInterimLeafByName(head+"_SS_Proton_DEC"));
    ReducerLeaf<Int_t>& var_tag_ss_proton_babar_leaf = reducer.CreateIntCopyLeaf("obsTagSSProton_BaBar", reducer.GetInterimLeafByName(head+"_SS_Proton_DEC"), -1.0);    
  }

  // Tag comparison
  ReducerLeaf<Int_t>& var_tag_ost_sspi_comp_leaf = reducer.CreateIntLeaf("catTagCompOSvsSSPion");
    var_tag_ost_sspi_comp_leaf.Multiply(var_tag_os_leaf, var_tag_ss_pion_leaf);
  ReducerLeaf<Int_t>& var_tag_ost_sspi_comp_babar_leaf = reducer.CreateIntLeaf("catTagCompOSvsSSPion_BaBar");
    var_tag_ost_sspi_comp_babar_leaf.Multiply(var_tag_os_babar_leaf, var_tag_ss_pion_babar_leaf);

  // obsEta
  ReducerLeaf<Double_t>& var_tag_eta_leaf = reducer.CreateDoubleCopyLeaf("obsEtaAll", reducer.GetInterimLeafByName(head+"_TAGOMEGA"));
  ReducerLeaf<Double_t>& var_tag_eta_os_leaf = reducer.CreateDoubleCopyLeaf("obsEtaOS", reducer.GetInterimLeafByName(head+"_TAGOMEGA_OS"));
  ReducerLeaf<Double_t>& var_tag_eta_os_mu_leaf = reducer.CreateDoubleCopyLeaf("obsEtaOSMuon", reducer.GetInterimLeafByName(head+"_OS_Muon_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_os_electron_leaf = reducer.CreateDoubleCopyLeaf("obsEtaOSElectron", reducer.GetInterimLeafByName(head+"_OS_Electron_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_os_kaon_leaf = reducer.CreateDoubleCopyLeaf("obsEtaOSKaon", reducer.GetInterimLeafByName(head+"_OS_Kaon_PROB"));
  if (reducer.LeafExists(head+"_OS_nnetKaon_DEC")) ReducerLeaf<Double_t>& var_tag_eta_os_nnet_kaon_leaf = reducer.CreateDoubleCopyLeaf("obsEtaOSNNKaon", reducer.GetInterimLeafByName(head+"_OS_nnetKaon_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_ss_kaon_leaf = reducer.CreateDoubleCopyLeaf("obsEtaSSKaon", reducer.GetInterimLeafByName(head+"_SS_Kaon_PROB"));
  if (reducer.LeafExists(head+"_SS_nnetKaon_DEC")) ReducerLeaf<Double_t>& var_tag_eta_ss_nnet_kaon_leaf = reducer.CreateDoubleCopyLeaf("obsEtaSSNNKaon", reducer.GetInterimLeafByName(head+"_SS_nnetKaon_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_ss_pion_leaf = reducer.CreateDoubleCopyLeaf("obsEtaSSPion", reducer.GetInterimLeafByName(head+"_SS_Pion_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_vtxq_leaf = reducer.CreateDoubleCopyLeaf("obsEtaVtxQ", reducer.GetInterimLeafByName(head+"_VtxCharge_PROB"));
  if (reducer.LeafExists(head+"_OS_Charm_PROB")) ReducerLeaf<Double_t>& var_tag_eta_os_charm_leaf = reducer.CreateDoubleCopyLeaf("obsEtaOSCharm", reducer.GetInterimLeafByName(head+"_OS_Charm_PROB"));
  if (reducer.LeafExists(head+"_SS_Proton_PROB")) ReducerLeaf<Double_t>& var_tag_eta_ss_proton_leaf = reducer.CreateDoubleCopyLeaf("obsEtaSSProton", reducer.GetInterimLeafByName(head+"_SS_Proton_PROB"));

  // catEta: somehow this variable does not get written into the tuples anymore
  if (reducer.LeafExists(head+"_TAGCAT")) ReducerLeaf<Int_t>& var_tagcat_leaf = reducer.CreateIntCopyLeaf("catEtaAll", reducer.GetInterimLeafByName(head+"_TAGCAT"));
  if (reducer.LeafExists(head+"_TAGCAT_OS")) ReducerLeaf<Int_t>& var_tagcat_os_leaf = reducer.CreateIntCopyLeaf("catEtaOS", reducer.GetInterimLeafByName(head+"_TAGCAT_OS"));

  // catTagged
  ReducerLeaf<Int_t>& cat_tagged_leaf = reducer.CreateIntLeaf("catTaggedAll", -1);
    cat_tagged_leaf.AddCondition("Tagged",   head+"_TAGDECISION != 0", 1);
    cat_tagged_leaf.AddCondition("Untagged", head+"_TAGDECISION == 0", 0);
  
  ReducerLeaf<Int_t>& cat_tagged_os_leaf = reducer.CreateIntLeaf("catTaggedOS", -1);
    cat_tagged_os_leaf.AddCondition("Tagged",   head+"_TAGDECISION_OS != 0", 1);
    cat_tagged_os_leaf.AddCondition("Untagged", head+"_TAGDECISION_OS == 0", 0);

  ReducerLeaf<Int_t>& cat_tagged_os_muon_leaf = reducer.CreateIntLeaf("catTaggedOSMuon", -1);
    cat_tagged_os_muon_leaf.AddCondition("Tagged",   head+"_OS_Muon_DEC != 0", 1);
    cat_tagged_os_muon_leaf.AddCondition("Untagged", head+"_OS_Muon_DEC == 0", 0);

  ReducerLeaf<Int_t>& cat_tagged_os_electron_leaf = reducer.CreateIntLeaf("catTaggedOSElectron", -1);
    cat_tagged_os_electron_leaf.AddCondition("Tagged",   head+"_OS_Electron_DEC != 0", 1);
    cat_tagged_os_electron_leaf.AddCondition("Untagged", head+"_OS_Electron_DEC == 0", 0);

  ReducerLeaf<Int_t>& cat_tagged_os_kaon_leaf = reducer.CreateIntLeaf("catTaggedOSKaon", -1);
    cat_tagged_os_kaon_leaf.AddCondition("Tagged",   head+"_OS_Kaon_DEC != 0", 1);
    cat_tagged_os_kaon_leaf.AddCondition("Untagged", head+"_OS_Kaon_DEC == 0", 0);
  
  if (reducer.LeafExists(head+"_OS_nnetKaon_DEC")){
    ReducerLeaf<Int_t>& cat_tagged_os_nnet_kaon_leaf = reducer.CreateIntLeaf("catTaggedOSNNKaon", -1);
      cat_tagged_os_nnet_kaon_leaf.AddCondition("Tagged",   head+"_OS_nnetKaon_DEC != 0", 1);
      cat_tagged_os_nnet_kaon_leaf.AddCondition("Untagged", head+"_OS_nnetKaon_DEC == 0", 0);
  }

  ReducerLeaf<Int_t>& cat_tagged_ss_kaon_leaf = reducer.CreateIntLeaf("catTaggedSSKaon", -1);
    cat_tagged_ss_kaon_leaf.AddCondition("Tagged",   head+"_SS_Kaon_DEC != 0", 1);
    cat_tagged_ss_kaon_leaf.AddCondition("Untagged", head+"_SS_Kaon_DEC == 0", 0);

  if (reducer.LeafExists(head+"_SS_nnetKaon_DEC")){
    ReducerLeaf<Int_t>& cat_tagged_ss_nnet_kaon_leaf = reducer.CreateIntLeaf("catTaggedSSNNKaon", -1);
      cat_tagged_ss_nnet_kaon_leaf.AddCondition("Tagged",   head+"_SS_nnetKaon_DEC != 0", 1);
      cat_tagged_ss_nnet_kaon_leaf.AddCondition("Untagged", head+"_SS_nnetKaon_DEC == 0", 0);
  }

  ReducerLeaf<Int_t>& cat_tagged_ss_pion_leaf = reducer.CreateIntLeaf("catTaggedSSPion", -1);
    cat_tagged_ss_pion_leaf.AddCondition("Tagged",   head+"_SS_Pion_DEC != 0", 1);
    cat_tagged_ss_pion_leaf.AddCondition("Untagged", head+"_SS_Pion_DEC == 0", 0);

  ReducerLeaf<Int_t>& cat_tagged_vtxq_leaf = reducer.CreateIntLeaf("catTaggedVtxQ", -1);
    cat_tagged_vtxq_leaf.AddCondition("Tagged",   head+"_VtxCharge_DEC != 0", 1);
    cat_tagged_vtxq_leaf.AddCondition("Untagged", head+"_VtxCharge_DEC == 0", 0);

  if (reducer.LeafExists(head+"_OS_Charm_DEC")){
    ReducerLeaf<Int_t>& cat_tagged_os_charm_leaf = reducer.CreateIntLeaf("catTaggedOSCharm", -1);
      cat_tagged_os_charm_leaf.AddCondition("Tagged",   head+"_OS_Charm_DEC != 0", 1);
      cat_tagged_os_charm_leaf.AddCondition("Untagged", head+"_OS_Charm_DEC == 0", 0);
  }

  if (reducer.LeafExists(head+"_SS_Proton_DEC")){
    ReducerLeaf<Int_t>& cat_tagged_ss_proton_leaf = reducer.CreateIntLeaf("catTaggedSSProton", -1);
      cat_tagged_ss_proton_leaf.AddCondition("Tagged",   head+"_SS_Proton_DEC != 0", 1);
      cat_tagged_ss_proton_leaf.AddCondition("Untagged", head+"_SS_Proton_DEC == 0", 0);
  }

  // catFinalState
  ReducerLeaf<Int_t>& var_finalstate_leaf = reducer.CreateIntLeaf("catFinalState", 0);
    var_finalstate_leaf.AddCondition("B0",    head+"_ID>0", +1);
    var_finalstate_leaf.AddCondition("B0bar", head+"_ID<0", -1);

  // catXi
  ReducerLeaf<Int_t>& xi_leaf = reducer.CreateIntLeaf("catXiAll");
    xi_leaf.Multiply(var_finalstate_leaf, var_tag_all_leaf);
  ReducerLeaf<Int_t>& xi_os = reducer.CreateIntLeaf("catXiOS");
    xi_os.Multiply(var_finalstate_leaf, var_tag_os_leaf);

  reducer.Run();
  reducer.Finalize();
}

