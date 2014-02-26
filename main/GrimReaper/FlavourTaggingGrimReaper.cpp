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
    // OS with neural net kaon combination
    var_tag_os_comb_with_nnet_kaon_leaf_(NULL),
    var_tag_os_comb_with_nnet_kaon_babar_leaf_(NULL),
    cat_tagged_os_comb_with_nnet_kaon_leaf_(NULL),
    var_tag_eta_os_comb_with_nnet_kaon_leaf_(NULL),
    var_tag_os_muon_(NULL),
    var_tag_os_electron_(NULL),
    var_tag_os_nnet_kaon_(NULL),
    var_tag_os_vtx_charge_(NULL),
    var_tag_eta_os_muon_(NULL),
    var_tag_eta_os_electron_(NULL),
    var_tag_eta_os_nnet_kaon_(NULL),
    var_tag_eta_os_vtx_charge_(NULL),
    var_tag_os_comb_with_nnet_kaon_value_(NULL),
    var_tag_os_comb_with_nnet_kaon_babar_value_(NULL),
    cat_tagged_os_comb_with_nnet_kaon_value_(NULL),
    var_tag_eta_os_comb_with_nnet_kaon_value_(NULL),
    // sin2beta OS + SSPion combination
    var_tag_os_sspion_leaf_(NULL),
    var_tag_os_sspion_babar_leaf_(NULL),
    var_tag_os_sspion_exclusive_leaf_(NULL),
    var_tag_os_sspion_exclusive_babar_leaf_(NULL),
    cat_tagged_os_or_ss_pion_leaf_(NULL),
    cat_tagged_os_xor_ss_pion_leaf_(NULL),
    cat_tagged_os_ss_pion_leaf_(NULL),
    var_tag_eta_os_ss_pion_leaf_(NULL),
    var_tag_eta_os_ss_pion_exclusive_leaf_(NULL),
    var_tag_os_(NULL),
    var_tag_ss_pion_(NULL),
    cat_tagged_os_(NULL),
    cat_tagged_ss_pion_(NULL),
    var_tag_eta_os_(NULL),
    var_tag_eta_ss_pion_(NULL),
    var_tag_os_sspion_value_(NULL),
    var_tag_os_sspion_babar_value_(NULL),
    cat_tagged_os_or_ss_pion_value_(NULL),
    cat_tagged_os_xor_ss_pion_value_(NULL),
    var_tag_eta_os_ss_pion_value_(NULL)
  {}
  virtual ~TaggingRdcr(){}
 protected:
  virtual void CreateSpecialBranches();
  virtual bool EntryPassesSpecialCuts();
  virtual void UpdateSpecialLeaves();
 private:
  /////// OS with neural net kaon combination
  // create leaves
  dooselection::reducer::ReducerLeaf<Int_t>* var_tag_os_comb_with_nnet_kaon_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>* var_tag_os_comb_with_nnet_kaon_babar_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>* cat_tagged_os_comb_with_nnet_kaon_leaf_; 
  dooselection::reducer::ReducerLeaf<Double_t>* var_tag_eta_os_comb_with_nnet_kaon_leaf_; 

  // leaves to read
  Int_t* var_tag_os_muon_;         
  Int_t* var_tag_os_electron_; 
  Int_t* var_tag_os_nnet_kaon_; 
  Int_t* var_tag_os_vtx_charge_;    
  Double_t* var_tag_eta_os_muon_;      
  Double_t* var_tag_eta_os_electron_; 
  Double_t* var_tag_eta_os_nnet_kaon_; 
  Double_t* var_tag_eta_os_vtx_charge_; 

  // leaves to write
  Int_t* var_tag_os_comb_with_nnet_kaon_value_;         
  Int_t* var_tag_os_comb_with_nnet_kaon_babar_value_;   
  Int_t* cat_tagged_os_comb_with_nnet_kaon_value_;  
  Double_t* var_tag_eta_os_comb_with_nnet_kaon_value_; 

  /////// sin2beta OS + SSPion combination
  // create leaves
  dooselection::reducer::ReducerLeaf<Int_t>* var_tag_os_sspion_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>* var_tag_os_sspion_babar_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>* var_tag_os_sspion_exclusive_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>* var_tag_os_sspion_exclusive_babar_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>* cat_tagged_os_or_ss_pion_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>* cat_tagged_os_xor_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>* cat_tagged_os_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* var_tag_eta_os_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* var_tag_eta_os_ss_pion_exclusive_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>* var_tag_os_with_nnet_kaon_sspion_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>* var_tag_os_with_nnet_kaon_sspion_babar_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>* cat_tagged_os_with_nnet_kaon_or_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Int_t>* cat_tagged_os_with_nnet_kaon_xor_ss_pion_leaf_;
  dooselection::reducer::ReducerLeaf<Double_t>* var_tag_eta_os_with_nnet_kaon_ss_pion_leaf_;

  // leaves to read
  Int_t* var_tag_os_;         
  Int_t* var_tag_ss_pion_;    
  Int_t* cat_tagged_os_;      
  Int_t* cat_tagged_ss_pion_; 
  Double_t* var_tag_eta_os_;      
  Double_t* var_tag_eta_ss_pion_; 

  // leaves to write
  Int_t* var_tag_os_sspion_value_;         
  Int_t* var_tag_os_sspion_babar_value_;
  Int_t* var_tag_os_sspion_exclusive_value_;
  Int_t* var_tag_os_sspion_exclusive_babar_value_;
  Int_t* cat_tagged_os_or_ss_pion_value_;  
  Int_t* cat_tagged_os_xor_ss_pion_value_; 
  Int_t* cat_tagged_os_ss_pion_value_;
  Double_t* var_tag_eta_os_ss_pion_value_;
  Double_t* var_tag_eta_os_ss_pion_exclusive_value_;
  Int_t* var_tag_os_with_nnet_kaon_sspion_value_;
  Int_t* var_tag_os_with_nnet_kaon_sspion_babar_value_;
  Int_t* cat_tagged_os_with_nnet_kaon_or_ss_pion_value_;
  Int_t* cat_tagged_os_with_nnet_kaon_xor_ss_pion_value_;
  Double_t* var_tag_eta_os_with_nnet_kaon_ss_pion_value_;
};

void TaggingRdcr::CreateSpecialBranches(){
  /////// OS with neural net kaon combination
  var_tag_os_comb_with_nnet_kaon_leaf_       = &CreateIntLeaf("obsTagOSwNNKaon");
  var_tag_os_comb_with_nnet_kaon_babar_leaf_ = &CreateIntLeaf("obsTagOSwNNKaon_BaBar");  
  cat_tagged_os_comb_with_nnet_kaon_leaf_    = &CreateIntLeaf("catTaggedOSwNNKaon");
  var_tag_eta_os_comb_with_nnet_kaon_leaf_   = &CreateDoubleLeaf("obsEtaOSwNNKaon");

  var_tag_os_muon_           = (Int_t*)GetInterimLeafByName("B0_OS_Muon_DEC").branch_address();     
  var_tag_os_electron_       = (Int_t*)GetInterimLeafByName("B0_OS_Electron_DEC").branch_address();
  if (LeafExists("B0_OS_nnetKaon_DEC")) var_tag_os_nnet_kaon_ = (Int_t*)GetInterimLeafByName("B0_OS_nnetKaon_DEC").branch_address();
  var_tag_os_vtx_charge_     = (Int_t*)GetInterimLeafByName("B0_VtxCharge_DEC").branch_address();      
  var_tag_eta_os_muon_       = (Double_t*)GetInterimLeafByName("B0_OS_Muon_PROB").branch_address();      
  var_tag_eta_os_electron_   = (Double_t*)GetInterimLeafByName("B0_OS_Electron_PROB").branch_address();     
  if (LeafExists("B0_OS_nnetKaon_DEC")) var_tag_eta_os_nnet_kaon_ = (Double_t*)GetInterimLeafByName("B0_OS_nnetKaon_PROB").branch_address();     
  var_tag_eta_os_vtx_charge_ = (Double_t*)GetInterimLeafByName("B0_VtxCharge_PROB").branch_address();       

  var_tag_os_comb_with_nnet_kaon_value_       = (Int_t*)var_tag_os_comb_with_nnet_kaon_leaf_->branch_address();
  var_tag_os_comb_with_nnet_kaon_babar_value_ = (Int_t*)var_tag_os_comb_with_nnet_kaon_babar_leaf_->branch_address();
  cat_tagged_os_comb_with_nnet_kaon_value_    = (Int_t*)cat_tagged_os_comb_with_nnet_kaon_leaf_->branch_address();
  var_tag_eta_os_comb_with_nnet_kaon_value_   = (Double_t*)var_tag_eta_os_comb_with_nnet_kaon_leaf_->branch_address();

  /////// sin2beta OS + SSPion combination (both OS combinations (with cut based and neural net kaon))
  // create special combination leaves
  var_tag_os_sspion_leaf_                 = &CreateIntLeaf("obsTagOSSSPion");
  var_tag_os_sspion_babar_leaf_           = &CreateIntLeaf("obsTagOSSSPion_BaBar");
  var_tag_os_sspion_exclusive_leaf_       = &CreateIntLeaf("obsTagOSSSPionExcl");
  var_tag_os_sspion_exclusive_babar_leaf_ = &CreateIntLeaf("obsTagOSSSPionExcl_BaBar");
  cat_tagged_os_or_ss_pion_leaf_          = &CreateIntLeaf("catTaggedOSorSSPion");        // 0 for untagged, 1 for tagged
  cat_tagged_os_xor_ss_pion_leaf_         = &CreateIntLeaf("catTaggedOSxorSSPion");       // 0 for untagged, 1 for OS tag, -1 for SSPion tag
  cat_tagged_os_ss_pion_leaf_             = &CreateIntLeaf("catTaggedOSSSPion");          // 0 for untagged, 1 for excl. OS tag, -1 for excl. SSPion tag, 10 for combination of OS and SSPion
  var_tag_eta_os_ss_pion_leaf_            = &CreateDoubleLeaf("obsEtaOSSSPion");
  var_tag_eta_os_ss_pion_exclusive_leaf_  = &CreateDoubleLeaf("obsEtaOSSSPionExcl");

  if (LeafExists("B0_OS_nnetKaon_DEC")){
    var_tag_os_with_nnet_kaon_sspion_leaf_         = &CreateIntLeaf("obsTagOSwNNKaonSSPion");
    var_tag_os_with_nnet_kaon_sspion_babar_leaf_   = &CreateIntLeaf("obsTagOSwNNKaonSSPion_BaBar");
    cat_tagged_os_with_nnet_kaon_or_ss_pion_leaf_  = &CreateIntLeaf("catTaggedOSwNNKaonorSSPion");     // 0 for untagged, 1 for tagged
    cat_tagged_os_with_nnet_kaon_xor_ss_pion_leaf_ = &CreateIntLeaf("catTaggedOSwNNKaonxorSSPion");   // 0 for untagged, 1 for OS tag, -1 for SSPion tag
    var_tag_eta_os_with_nnet_kaon_ss_pion_leaf_    = &CreateDoubleLeaf("obsEtaOSwNNKaonSSPion");
  }

  // leaves to read
  var_tag_os_          = (Int_t*)GetInterimLeafByName("B0_TAGDECISION_OS").branch_address();
  var_tag_ss_pion_     = (Int_t*)GetInterimLeafByName("B0_SS_Pion_DEC").branch_address();
  cat_tagged_os_       = (Int_t*)GetInterimLeafByName("B0_TAGDECISION_OS").branch_address();
  cat_tagged_ss_pion_  = (Int_t*)GetInterimLeafByName("B0_SS_Pion_DEC").branch_address();
  var_tag_eta_os_      = (Double_t*)GetInterimLeafByName("B0_TAGOMEGA_OS").branch_address();
  var_tag_eta_ss_pion_ = (Double_t*)GetInterimLeafByName("B0_SS_Pion_PROB").branch_address();

  // leaves to write
  var_tag_os_sspion_value_         = (Int_t*)var_tag_os_sspion_leaf_->branch_address();  
  var_tag_os_sspion_babar_value_   = (Int_t*)var_tag_os_sspion_babar_leaf_->branch_address();
  var_tag_os_sspion_exclusive_value_         = (Int_t*)var_tag_os_sspion_exclusive_leaf_->branch_address();  
  var_tag_os_sspion_exclusive_babar_value_   = (Int_t*)var_tag_os_sspion_exclusive_babar_leaf_->branch_address();
  cat_tagged_os_or_ss_pion_value_  = (Int_t*)cat_tagged_os_or_ss_pion_leaf_->branch_address();
  cat_tagged_os_xor_ss_pion_value_ = (Int_t*)cat_tagged_os_xor_ss_pion_leaf_->branch_address();
  cat_tagged_os_ss_pion_value_ = (Int_t*)cat_tagged_os_ss_pion_leaf_->branch_address();
  var_tag_eta_os_ss_pion_value_    = (Double_t*)var_tag_eta_os_ss_pion_leaf_->branch_address();
  var_tag_eta_os_ss_pion_exclusive_value_    = (Double_t*)var_tag_eta_os_ss_pion_exclusive_leaf_->branch_address();

  if (LeafExists("B0_OS_nnetKaon_DEC")){
    var_tag_os_with_nnet_kaon_sspion_value_         = (Int_t*)var_tag_os_with_nnet_kaon_sspion_leaf_->branch_address();  
    var_tag_os_with_nnet_kaon_sspion_babar_value_   = (Int_t*)var_tag_os_with_nnet_kaon_sspion_babar_leaf_->branch_address();
    cat_tagged_os_with_nnet_kaon_or_ss_pion_value_  = (Int_t*)cat_tagged_os_with_nnet_kaon_or_ss_pion_leaf_->branch_address();
    cat_tagged_os_with_nnet_kaon_xor_ss_pion_value_ = (Int_t*)cat_tagged_os_with_nnet_kaon_xor_ss_pion_leaf_->branch_address();  
    var_tag_eta_os_with_nnet_kaon_ss_pion_value_    = (Double_t*)var_tag_eta_os_with_nnet_kaon_ss_pion_leaf_->branch_address(); 
  } 
}

bool TaggingRdcr::EntryPassesSpecialCuts(){return true;}

void TaggingRdcr::UpdateSpecialLeaves(){
  double tagdecision=0; 
  double pnsum_a= 0.50; //hypothesis of truetag=+1 
  double pnsum_b= 0.50; //hypothesis of truetag=-1 
  double pnsum= 0.; 

  // OS with neural net kaon combination
  if (LeafExists("B0_OS_nnetKaon_DEC")){
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
  }

  // sin2beta OS (with cut based OS kaon) + SSPion combination
  if ((*var_tag_os_==0) && (*var_tag_ss_pion_==0)){           // if OS and SSPion tags are 0, set everything to untagged
    *var_tag_os_sspion_value_ = 0;
    *var_tag_os_sspion_babar_value_ = 0;
    *cat_tagged_os_ss_pion_value_ = 0;
    *var_tag_eta_os_ss_pion_value_ = 0.5;
  }
  else if ((*var_tag_os_!=0) && (*var_tag_ss_pion_==0)){      // if OS tagger exclusively has tag, write OS tag to combination
    *var_tag_os_sspion_value_ = *var_tag_os_;
    *var_tag_os_sspion_babar_value_ = -(*var_tag_os_);
    *cat_tagged_os_ss_pion_value_ = 1;
    *var_tag_eta_os_ss_pion_value_ = *var_tag_eta_os_;
  }
  else if ((*var_tag_os_==0) && (*var_tag_ss_pion_!=0)){      // if SSPion tagger exclusively has tag, write OS tag to combination
    *var_tag_os_sspion_value_ = *var_tag_ss_pion_;
    *var_tag_os_sspion_babar_value_ = -(*var_tag_ss_pion_);
    *cat_tagged_os_ss_pion_value_ = -1;
    *var_tag_eta_os_ss_pion_value_ = *var_tag_eta_ss_pion_;
  }
  else{                                                       // else, combine OS and SSPion tag decision and eta
    // this is the FT standard combination
    // see page 106 of: https://cds.cern.ch/record/1456804/files/CERN-THESIS-2012-075.pdf
    // as a reference.
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
    if (comb_prob_b > comb_prob_bbar) *var_tag_os_sspion_value_ = -1;   // probability for b quark content is larger than for bbar quark
    if (comb_prob_b < comb_prob_bbar) *var_tag_os_sspion_value_ = +1;   // probability for b quark content is smaller than for bbar quark
    if (comb_prob_b > comb_prob_bbar) *var_tag_os_sspion_babar_value_ = +1;   // vice versa for babar notation
    if (comb_prob_b < comb_prob_bbar) *var_tag_os_sspion_babar_value_ = -1;   // 

    // mistag
    double comb_prob = std::max(comb_prob_b,comb_prob_bbar) /(comb_prob_b + comb_prob_bbar);
    *var_tag_eta_os_ss_pion_value_ = 1. - comb_prob;

    // tag category
    *cat_tagged_os_ss_pion_value_ = 10;

    // debug output
    // std::cout << "" << std::endl;
    // std::cout << "using oss ss pion combintation" << std::endl;
    // std::cout << "OS tag / mistag " << os_dec << " / " << os_mistag << std::endl;
    // std::cout << "SS tag / mistag " << ss_dec << " / " << ss_mistag << std::endl;
    // std::cout << "Comb tag / mistag " << *var_tag_os_sspion_value_ << " / " << *var_tag_eta_os_ss_pion_value_ << std::endl;
  }

  // sin2beta OS (with cut based OS kaon) + exclusive SSPion combination
  if (*var_tag_os_!=0){ // if OS tagger has tag, write OS tag to combination and 
    *var_tag_os_sspion_exclusive_value_ = *var_tag_os_;
    *var_tag_os_sspion_exclusive_babar_value_ = -(*var_tag_os_);
    *cat_tagged_os_or_ss_pion_value_ = 1;
    *cat_tagged_os_xor_ss_pion_value_ = 1;
    *var_tag_eta_os_ss_pion_exclusive_value_ = *var_tag_eta_os_;
  }
  else if ((*var_tag_os_==0) && (*var_tag_ss_pion_!=0)){
    *var_tag_os_sspion_exclusive_value_ = *var_tag_ss_pion_;
    *var_tag_os_sspion_exclusive_babar_value_ = -(*var_tag_ss_pion_);
    *cat_tagged_os_or_ss_pion_value_ = 1;
    *cat_tagged_os_xor_ss_pion_value_ = -1;
    *var_tag_eta_os_ss_pion_exclusive_value_ = *var_tag_eta_ss_pion_;
  }
  else{
    *var_tag_os_sspion_exclusive_value_ = 0;
    *var_tag_os_sspion_exclusive_babar_value_ = 0;
    *cat_tagged_os_or_ss_pion_value_ = 0;
    *cat_tagged_os_xor_ss_pion_value_ = 0;
    *var_tag_eta_os_ss_pion_exclusive_value_ = 0.5;
  }

  // sin2beta OS (with neural net kaon) + SSPion combination
  if (LeafExists("B0_OS_nnetKaon_DEC")){
    if (tagdecision!=0){ // if OS tagger has tag, write OS tag to combination and 
      *var_tag_os_with_nnet_kaon_sspion_value_ = tagdecision;
      *var_tag_os_with_nnet_kaon_sspion_babar_value_ = -1.*tagdecision;
      *cat_tagged_os_with_nnet_kaon_or_ss_pion_value_ = 1;
      *cat_tagged_os_with_nnet_kaon_xor_ss_pion_value_ = 1;
      *var_tag_eta_os_with_nnet_kaon_ss_pion_value_ = 1-pnsum;
    }
    else if ((tagdecision==0) && (*var_tag_ss_pion_!=0)){
      *var_tag_os_with_nnet_kaon_sspion_value_ = *var_tag_ss_pion_;
      *var_tag_os_with_nnet_kaon_sspion_babar_value_ = -(*var_tag_ss_pion_);
      *cat_tagged_os_with_nnet_kaon_or_ss_pion_value_ = 1;
      *cat_tagged_os_with_nnet_kaon_xor_ss_pion_value_ = -1;
      *var_tag_eta_os_with_nnet_kaon_ss_pion_value_ = *var_tag_eta_ss_pion_;
    }
    else{
      *var_tag_os_with_nnet_kaon_sspion_value_ = 0;
      *var_tag_os_with_nnet_kaon_sspion_babar_value_ = 0;
      *cat_tagged_os_with_nnet_kaon_or_ss_pion_value_ = 0;
      *cat_tagged_os_with_nnet_kaon_xor_ss_pion_value_ = 0;
      *var_tag_eta_os_with_nnet_kaon_ss_pion_value_ = 0.5;
    }
  }
}

int Bd2JpsiKS(const std::string& inputfile, const std::string& inputtree, const std::string& outputfile, const std::string& outputtree, const std::string& decay_channel);

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "FlavourTaggingGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, decay_channel;
  if (argc == 5){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    // decay_channel = argv[5];
    decay_channel = "Bd2JpsiKS"; // always use this channel for now
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "FlavourTaggingGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "FlavourTaggingGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name" << doocore::io::endmsg;
    return 1;
  }

  if (decay_channel=="Bd2JpsiKS"){
    doocore::io::sinfo << "-info-  \t" << "FlavourTaggingGrimReaper \t" << "Decay: Bd2JpsiKS" << doocore::io::endmsg;
    Bd2JpsiKS(inputfile, inputtree, outputfile, outputtree, decay_channel);
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "FlavourTaggingGrimReaper \t" << "No valid decay channel. Possible decay channels are:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "FlavourTaggingGrimReaper \t" << "- Bd2JspiKS" << doocore::io::endmsg;
  }
  doocore::io::sinfo << "-info- \t" << "FlavourTaggingGrimReaper \t" << "Done!" << doocore::io::endmsg;
}

int Bd2JpsiKS(const std::string& inputfile, const std::string& inputtree, const std::string& outputfile, const std::string& outputtree, const std::string& decay_channel){
  TaggingRdcr reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  reducer.Initialize();

  // handle MC specific stuff
  if (reducer.LeafExists("B0_BKGCAT") && reducer.LeafExists("B0_TRUEID")){
    // flavour tagging true tag
    ReducerLeaf<Int_t>& vtag_true_leaf = reducer.CreateIntLeaf("obsTag_True", 0);
    vtag_true_leaf.AddCondition("B0",    "B0_TRUEID>0", +1);
    vtag_true_leaf.AddCondition("B0bar", "B0_TRUEID<0", -1);

    ReducerLeaf<Int_t>& vtag_true_babar_leaf = reducer.CreateIntLeaf("obsTag_BaBar_True", 0);
    vtag_true_babar_leaf.AddCondition("B0",    "B0_TRUEID>0", -1);
    vtag_true_babar_leaf.AddCondition("B0bar", "B0_TRUEID<0", +1);
  }

  // catTagger
  ReducerLeaf<Int_t>& var_tagger_leaf = reducer.CreateIntCopyLeaf("catTagger", reducer.GetInterimLeafByName("B0_TAGGER"));

  // obsTag
  ReducerLeaf<Int_t>& var_tag_all_leaf = reducer.CreateIntCopyLeaf("obsTagAll", reducer.GetInterimLeafByName("B0_TAGDECISION"));
  ReducerLeaf<Int_t>& var_tag_all_babar_leaf = reducer.CreateIntCopyLeaf("obsTagAll_BaBar", reducer.GetInterimLeafByName("B0_TAGDECISION"), -1.0);
  ReducerLeaf<Int_t>& var_tag_os_leaf = reducer.CreateIntCopyLeaf("obsTagOS", reducer.GetInterimLeafByName("B0_TAGDECISION_OS"));
  ReducerLeaf<Int_t>& var_tag_os_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOS_BaBar", reducer.GetInterimLeafByName("B0_TAGDECISION_OS"), -1.0);
  ReducerLeaf<Int_t>& var_tag_os_mu_leaf = reducer.CreateIntCopyLeaf("obsTagOSMuon", reducer.GetInterimLeafByName("B0_OS_Muon_DEC"));
  ReducerLeaf<Int_t>& var_tag_os_mu_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOSMuon_BaBar", reducer.GetInterimLeafByName("B0_OS_Muon_DEC"), -1.0);
  ReducerLeaf<Int_t>& var_tag_os_electron_leaf = reducer.CreateIntCopyLeaf("obsTagOSElectron", reducer.GetInterimLeafByName("B0_OS_Electron_DEC"));
  ReducerLeaf<Int_t>& var_tag_os_electron_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOSElectron_BaBar", reducer.GetInterimLeafByName("B0_OS_Electron_DEC"), -1.0);
  ReducerLeaf<Int_t>& var_tag_os_kaon_leaf = reducer.CreateIntCopyLeaf("obsTagOSKaon", reducer.GetInterimLeafByName("B0_OS_Kaon_DEC"));
  if (reducer.LeafExists("B0_OS_nnetKaon_DEC")){
    ReducerLeaf<Int_t>& var_tag_os_kaon_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOSKaon_BaBar", reducer.GetInterimLeafByName("B0_OS_Kaon_DEC"), -1.0);
    ReducerLeaf<Int_t>& var_tag_os_nnet_kaon_leaf = reducer.CreateIntCopyLeaf("obsTagOSNNKaon", reducer.GetInterimLeafByName("B0_OS_nnetKaon_DEC"));
    ReducerLeaf<Int_t>& var_tag_os_nnet_kaon_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOSNNKaon_BaBar", reducer.GetInterimLeafByName("B0_OS_nnetKaon_DEC"), -1.0);
  }
  ReducerLeaf<Int_t>& var_tag_ss_kaon_leaf = reducer.CreateIntCopyLeaf("obsTagSSKaon", reducer.GetInterimLeafByName("B0_SS_Kaon_DEC"));
  ReducerLeaf<Int_t>& var_tag_ss_kaon_babar_leaf = reducer.CreateIntCopyLeaf("obsTagSSKaon_BaBar", reducer.GetInterimLeafByName("B0_SS_Kaon_DEC"), -1.0);
  if (reducer.LeafExists("B0_SS_nnetKaon_DEC")){
    ReducerLeaf<Int_t>& var_tag_ss_nnet_kaon_leaf = reducer.CreateIntCopyLeaf("obsTagSSNNKaon", reducer.GetInterimLeafByName("B0_SS_nnetKaon_DEC"));
    ReducerLeaf<Int_t>& var_tag_ss_nnet_kaon_babar_leaf = reducer.CreateIntCopyLeaf("obsTagSSNNKaon_BaBar", reducer.GetInterimLeafByName("B0_SS_nnetKaon_DEC"), -1.0);
  }
  ReducerLeaf<Int_t>& var_tag_ss_pion_leaf = reducer.CreateIntCopyLeaf("obsTagSSPion", reducer.GetInterimLeafByName("B0_SS_Pion_DEC"));
  ReducerLeaf<Int_t>& var_tag_ss_pion_babar_leaf = reducer.CreateIntCopyLeaf("obsTagSSPion_BaBar", reducer.GetInterimLeafByName("B0_SS_Pion_DEC"), -1.0);
  ReducerLeaf<Int_t>& var_tag_vtxq_leaf = reducer.CreateIntCopyLeaf("obsTagVtxQ", reducer.GetInterimLeafByName("B0_VtxCharge_DEC"));
  ReducerLeaf<Int_t>& var_tag_vtxq_babar_leaf = reducer.CreateIntCopyLeaf("obsTagVtxQ_BaBar", reducer.GetInterimLeafByName("B0_VtxCharge_DEC"), -1.0);
  if (reducer.LeafExists("B0_OS_Charm_DEC")){
    ReducerLeaf<Int_t>& var_tag_os_charm_leaf = reducer.CreateIntCopyLeaf("obsTagOSCharm", reducer.GetInterimLeafByName("B0_OS_Charm_DEC"));
    ReducerLeaf<Int_t>& var_tag_os_charm_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOSCharm_BaBar", reducer.GetInterimLeafByName("B0_OS_Charm_DEC"), -1.0);  
  }
  if (reducer.LeafExists("B0_SS_Proton_DEC")){
    ReducerLeaf<Int_t>& var_tag_ss_proton_leaf = reducer.CreateIntCopyLeaf("obsTagSSProton", reducer.GetInterimLeafByName("B0_SS_Proton_DEC"));
    ReducerLeaf<Int_t>& var_tag_ss_proton_babar_leaf = reducer.CreateIntCopyLeaf("obsTagSSProton_BaBar", reducer.GetInterimLeafByName("B0_SS_Proton_DEC"), -1.0);    
  }

  // Tag comparison
  ReducerLeaf<Int_t>& var_tag_ost_sspi_comp_leaf = reducer.CreateIntLeaf("catTagCompOSvsSSPion");
    var_tag_ost_sspi_comp_leaf.Multiply(var_tag_os_leaf, var_tag_ss_pion_leaf);
  ReducerLeaf<Int_t>& var_tag_ost_sspi_comp_babar_leaf = reducer.CreateIntLeaf("catTagCompOSvsSSPion_BaBar");
    var_tag_ost_sspi_comp_babar_leaf.Multiply(var_tag_os_babar_leaf, var_tag_ss_pion_babar_leaf);

  // obsEta
  ReducerLeaf<Double_t>& var_tag_eta_leaf = reducer.CreateDoubleCopyLeaf("obsEtaAll", reducer.GetInterimLeafByName("B0_TAGOMEGA"));
  ReducerLeaf<Double_t>& var_tag_eta_os_leaf = reducer.CreateDoubleCopyLeaf("obsEtaOS", reducer.GetInterimLeafByName("B0_TAGOMEGA_OS"));
  ReducerLeaf<Double_t>& var_tag_eta_os_mu_leaf = reducer.CreateDoubleCopyLeaf("obsEtaOSMuon", reducer.GetInterimLeafByName("B0_OS_Muon_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_os_electron_leaf = reducer.CreateDoubleCopyLeaf("obsEtaOSElectron", reducer.GetInterimLeafByName("B0_OS_Electron_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_os_kaon_leaf = reducer.CreateDoubleCopyLeaf("obsEtaOSKaon", reducer.GetInterimLeafByName("B0_OS_Kaon_PROB"));
  if (reducer.LeafExists("B0_OS_nnetKaon_DEC")) ReducerLeaf<Double_t>& var_tag_eta_os_nnet_kaon_leaf = reducer.CreateDoubleCopyLeaf("obsEtaOSNNKaon", reducer.GetInterimLeafByName("B0_OS_nnetKaon_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_ss_kaon_leaf = reducer.CreateDoubleCopyLeaf("obsEtaSSKaon", reducer.GetInterimLeafByName("B0_SS_Kaon_PROB"));
  if (reducer.LeafExists("B0_SS_nnetKaon_DEC")) ReducerLeaf<Double_t>& var_tag_eta_ss_nnet_kaon_leaf = reducer.CreateDoubleCopyLeaf("obsEtaSSNNKaon", reducer.GetInterimLeafByName("B0_SS_nnetKaon_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_ss_pion_leaf = reducer.CreateDoubleCopyLeaf("obsEtaSSPion", reducer.GetInterimLeafByName("B0_SS_Pion_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_vtxq_leaf = reducer.CreateDoubleCopyLeaf("obsEtaVtxQ", reducer.GetInterimLeafByName("B0_VtxCharge_PROB"));
  if (reducer.LeafExists("B0_OS_Charm_PROB")) ReducerLeaf<Double_t>& var_tag_eta_os_charm_leaf = reducer.CreateDoubleCopyLeaf("obsEtaOSCharm", reducer.GetInterimLeafByName("B0_OS_Charm_PROB"));
  if (reducer.LeafExists("B0_SS_Proton_PROB")) ReducerLeaf<Double_t>& var_tag_eta_ss_proton_leaf = reducer.CreateDoubleCopyLeaf("obsEtaSSProton", reducer.GetInterimLeafByName("B0_SS_Proton_PROB"));

  // catEta: somehow this variable does not get written into the tuples anymore
  if (reducer.LeafExists("B0_TAGCAT")) ReducerLeaf<Int_t>& var_tagcat_leaf = reducer.CreateIntCopyLeaf("catEtaAll", reducer.GetInterimLeafByName("B0_TAGCAT"));
  if (reducer.LeafExists("B0_TAGCAT_OS")) ReducerLeaf<Int_t>& var_tagcat_os_leaf = reducer.CreateIntCopyLeaf("catEtaOS", reducer.GetInterimLeafByName("B0_TAGCAT_OS"));

  // catTagged
  ReducerLeaf<Int_t>& cat_tagged_leaf = reducer.CreateIntLeaf("catTaggedAll", -1);
    cat_tagged_leaf.AddCondition("Tagged",   "B0_TAGDECISION != 0", 1);
    cat_tagged_leaf.AddCondition("Untagged", "B0_TAGDECISION == 0", 0);
  
  ReducerLeaf<Int_t>& cat_tagged_os_leaf = reducer.CreateIntLeaf("catTaggedOS", -1);
    cat_tagged_os_leaf.AddCondition("Tagged",   "B0_TAGDECISION_OS != 0", 1);
    cat_tagged_os_leaf.AddCondition("Untagged", "B0_TAGDECISION_OS == 0", 0);

  ReducerLeaf<Int_t>& cat_tagged_os_muon_leaf = reducer.CreateIntLeaf("catTaggedOSMuon", -1);
    cat_tagged_os_muon_leaf.AddCondition("Tagged",   "B0_OS_Muon_DEC != 0", 1);
    cat_tagged_os_muon_leaf.AddCondition("Untagged", "B0_OS_Muon_DEC == 0", 0);

  ReducerLeaf<Int_t>& cat_tagged_os_electron_leaf = reducer.CreateIntLeaf("catTaggedOSElectron", -1);
    cat_tagged_os_electron_leaf.AddCondition("Tagged",   "B0_OS_Electron_DEC != 0", 1);
    cat_tagged_os_electron_leaf.AddCondition("Untagged", "B0_OS_Electron_DEC == 0", 0);

  ReducerLeaf<Int_t>& cat_tagged_os_kaon_leaf = reducer.CreateIntLeaf("catTaggedOSKaon", -1);
    cat_tagged_os_kaon_leaf.AddCondition("Tagged",   "B0_OS_Kaon_DEC != 0", 1);
    cat_tagged_os_kaon_leaf.AddCondition("Untagged", "B0_OS_Kaon_DEC == 0", 0);
  
  if (reducer.LeafExists("B0_OS_nnetKaon_DEC")){
    ReducerLeaf<Int_t>& cat_tagged_os_nnet_kaon_leaf = reducer.CreateIntLeaf("catTaggedOSNNKaon", -1);
      cat_tagged_os_nnet_kaon_leaf.AddCondition("Tagged",   "B0_OS_nnetKaon_DEC != 0", 1);
      cat_tagged_os_nnet_kaon_leaf.AddCondition("Untagged", "B0_OS_nnetKaon_DEC == 0", 0);
  }

  ReducerLeaf<Int_t>& cat_tagged_ss_kaon_leaf = reducer.CreateIntLeaf("catTaggedSSKaon", -1);
    cat_tagged_ss_kaon_leaf.AddCondition("Tagged",   "B0_SS_Kaon_DEC != 0", 1);
    cat_tagged_ss_kaon_leaf.AddCondition("Untagged", "B0_SS_Kaon_DEC == 0", 0);

  if (reducer.LeafExists("B0_SS_nnetKaon_DEC")){
    ReducerLeaf<Int_t>& cat_tagged_ss_nnet_kaon_leaf = reducer.CreateIntLeaf("catTaggedSSNNKaon", -1);
      cat_tagged_ss_nnet_kaon_leaf.AddCondition("Tagged",   "B0_SS_nnetKaon_DEC != 0", 1);
      cat_tagged_ss_nnet_kaon_leaf.AddCondition("Untagged", "B0_SS_nnetKaon_DEC == 0", 0);
  }

  ReducerLeaf<Int_t>& cat_tagged_ss_pion_leaf = reducer.CreateIntLeaf("catTaggedSSPion", -1);
    cat_tagged_ss_pion_leaf.AddCondition("Tagged",   "B0_SS_Pion_DEC != 0", 1);
    cat_tagged_ss_pion_leaf.AddCondition("Untagged", "B0_SS_Pion_DEC == 0", 0);

  ReducerLeaf<Int_t>& cat_tagged_vtxq_leaf = reducer.CreateIntLeaf("catTaggedVtxQ", -1);
    cat_tagged_vtxq_leaf.AddCondition("Tagged",   "B0_VtxCharge_DEC != 0", 1);
    cat_tagged_vtxq_leaf.AddCondition("Untagged", "B0_VtxCharge_DEC == 0", 0);

  if (reducer.LeafExists("B0_OS_Charm_DEC")){
    ReducerLeaf<Int_t>& cat_tagged_os_charm_leaf = reducer.CreateIntLeaf("catTaggedOSCharm", -1);
      cat_tagged_os_charm_leaf.AddCondition("Tagged",   "B0_OS_Charm_DEC != 0", 1);
      cat_tagged_os_charm_leaf.AddCondition("Untagged", "B0_OS_Charm_DEC == 0", 0);
  }

  if (reducer.LeafExists("B0_SS_Proton_DEC")){
    ReducerLeaf<Int_t>& cat_tagged_ss_proton_leaf = reducer.CreateIntLeaf("catTaggedSSProton", -1);
      cat_tagged_ss_proton_leaf.AddCondition("Tagged",   "B0_SS_Proton_DEC != 0", 1);
      cat_tagged_ss_proton_leaf.AddCondition("Untagged", "B0_SS_Proton_DEC == 0", 0);
  }

  // catFinalState
  ReducerLeaf<Int_t>& var_finalstate_leaf = reducer.CreateIntLeaf("catFinalState", 0);
    var_finalstate_leaf.AddCondition("B0",    "B0_ID>0", +1);
    var_finalstate_leaf.AddCondition("B0bar", "B0_ID<0", -1);

  // catXi
  ReducerLeaf<Int_t>& xi_leaf = reducer.CreateIntLeaf("catXiAll");
    xi_leaf.Multiply(var_finalstate_leaf, var_tag_all_leaf);
  ReducerLeaf<Int_t>& xi_os = reducer.CreateIntLeaf("catXiOS");
    xi_os.Multiply(var_finalstate_leaf, var_tag_os_leaf);

  reducer.Run();
  reducer.Finalize();
}

