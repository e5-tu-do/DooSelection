/******************************************/
// DooVariablesGrimReaper.cpp
//
// Standalone GrimReaper that adds a lot of
// common variables to trees.
//
// Author: Christophe Cauet
// Date: 2013-10-14 
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
    cat_tagged_os_or_ss_pion_leaf_(NULL),
    cat_tagged_os_xor_ss_pion_leaf_(NULL),
    var_tag_eta_os_ss_pion_leaf_(NULL),
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
  dooselection::reducer::ReducerLeaf<Int_t>* cat_tagged_os_or_ss_pion_leaf_; 
  dooselection::reducer::ReducerLeaf<Int_t>* cat_tagged_os_xor_ss_pion_leaf_; 
  dooselection::reducer::ReducerLeaf<Double_t>* var_tag_eta_os_ss_pion_leaf_; 
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
  Int_t* cat_tagged_os_or_ss_pion_value_;  
  Int_t* cat_tagged_os_xor_ss_pion_value_; 
  Double_t* var_tag_eta_os_ss_pion_value_;
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
  var_tag_os_nnet_kaon_      = (Int_t*)GetInterimLeafByName("B0_OS_nnetKaon_DEC").branch_address();
  var_tag_os_vtx_charge_     = (Int_t*)GetInterimLeafByName("B0_VtxCharge_DEC").branch_address();      
  var_tag_eta_os_muon_       = (Double_t*)GetInterimLeafByName("B0_OS_Muon_PROB").branch_address();      
  var_tag_eta_os_electron_   = (Double_t*)GetInterimLeafByName("B0_OS_Electron_PROB").branch_address();     
  var_tag_eta_os_nnet_kaon_  = (Double_t*)GetInterimLeafByName("B0_OS_nnetKaon_PROB").branch_address();     
  var_tag_eta_os_vtx_charge_ = (Double_t*)GetInterimLeafByName("B0_VtxCharge_PROB").branch_address();       

  var_tag_os_comb_with_nnet_kaon_value_       = (Int_t*)var_tag_os_comb_with_nnet_kaon_leaf_->branch_address();
  var_tag_os_comb_with_nnet_kaon_babar_value_ = (Int_t*)var_tag_os_comb_with_nnet_kaon_babar_leaf_->branch_address();
  cat_tagged_os_comb_with_nnet_kaon_value_    = (Int_t*)cat_tagged_os_comb_with_nnet_kaon_leaf_->branch_address();
  var_tag_eta_os_comb_with_nnet_kaon_value_   = (Double_t*)var_tag_eta_os_comb_with_nnet_kaon_leaf_->branch_address();

  /////// sin2beta OS + SSPion combination (both OS combinations (with cut based and neural net kaon))
  // create special combination leaves
  var_tag_os_sspion_leaf_         = &CreateIntLeaf("obsTagOSSSPion");
  var_tag_os_sspion_babar_leaf_   = &CreateIntLeaf("obsTagOSSSPion_BaBar");
  cat_tagged_os_or_ss_pion_leaf_  = &CreateIntLeaf("catTaggedOSorSSPion");     // 0 for untagged, 1 for tagged
  cat_tagged_os_xor_ss_pion_leaf_ = &CreateIntLeaf("catTaggedOSxorSSPion");   // 0 for untagged, 1 for OS tag, -1 for SSPion tag
  var_tag_eta_os_ss_pion_leaf_    = &CreateDoubleLeaf("obsEtaOSSSPion");
  var_tag_os_with_nnet_kaon_sspion_leaf_         = &CreateIntLeaf("obsTagOSwNNKaonSSPion");
  var_tag_os_with_nnet_kaon_sspion_babar_leaf_   = &CreateIntLeaf("obsTagOSwNNKaonSSPion_BaBar");
  cat_tagged_os_with_nnet_kaon_or_ss_pion_leaf_  = &CreateIntLeaf("catTaggedOSwNNKaonorSSPion");     // 0 for untagged, 1 for tagged
  cat_tagged_os_with_nnet_kaon_xor_ss_pion_leaf_ = &CreateIntLeaf("catTaggedOSwNNKaonxorSSPion");   // 0 for untagged, 1 for OS tag, -1 for SSPion tag
  var_tag_eta_os_with_nnet_kaon_ss_pion_leaf_    = &CreateDoubleLeaf("obsEtaOSwNNKaonSSPion");

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
  cat_tagged_os_or_ss_pion_value_  = (Int_t*)cat_tagged_os_or_ss_pion_leaf_->branch_address();
  cat_tagged_os_xor_ss_pion_value_ = (Int_t*)cat_tagged_os_xor_ss_pion_leaf_->branch_address();  
  var_tag_eta_os_ss_pion_value_    = (Double_t*)var_tag_eta_os_ss_pion_leaf_->branch_address();  
  var_tag_os_with_nnet_kaon_sspion_value_         = (Int_t*)var_tag_os_with_nnet_kaon_sspion_leaf_->branch_address();  
  var_tag_os_with_nnet_kaon_sspion_babar_value_   = (Int_t*)var_tag_os_with_nnet_kaon_sspion_babar_leaf_->branch_address();
  cat_tagged_os_with_nnet_kaon_or_ss_pion_value_  = (Int_t*)cat_tagged_os_with_nnet_kaon_or_ss_pion_leaf_->branch_address();
  cat_tagged_os_with_nnet_kaon_xor_ss_pion_value_ = (Int_t*)cat_tagged_os_with_nnet_kaon_xor_ss_pion_leaf_->branch_address();  
  var_tag_eta_os_with_nnet_kaon_ss_pion_value_    = (Double_t*)var_tag_eta_os_with_nnet_kaon_ss_pion_leaf_->branch_address(); 
}

bool TaggingRdcr::EntryPassesSpecialCuts(){return true;}

void TaggingRdcr::UpdateSpecialLeaves(){
  // OS with neural net kaon combination

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
  double tagdecision=0; 
  double pnsum_a= 0.50; //hypothesis of truetag=+1 
  double pnsum_b= 0.50; //hypothesis of truetag=-1 
  double pnsum= 0.; 

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

  // sin2beta OS (with cut based OS kaon) + SSPion combination
  if (*var_tag_os_!=0){ // if OS tagger has tag, write OS tag to combination and 
    *var_tag_os_sspion_value_ = *var_tag_os_;
    *var_tag_os_sspion_babar_value_ = -(*var_tag_os_);
    *cat_tagged_os_or_ss_pion_value_ = 1;
    *cat_tagged_os_xor_ss_pion_value_ = 1;
    *var_tag_eta_os_ss_pion_value_ = *var_tag_eta_os_;
  }
  else if ((*var_tag_os_==0) && (*var_tag_ss_pion_!=0)){
    *var_tag_os_sspion_value_ = *var_tag_ss_pion_;
    *var_tag_os_sspion_babar_value_ = -(*var_tag_ss_pion_);
    *cat_tagged_os_or_ss_pion_value_ = 1;
    *cat_tagged_os_xor_ss_pion_value_ = -1;
    *var_tag_eta_os_ss_pion_value_ = *var_tag_eta_ss_pion_;
  }
  else{
    *var_tag_os_sspion_value_ = 0;
    *var_tag_os_sspion_babar_value_ = 0;
    *cat_tagged_os_or_ss_pion_value_ = 0;
    *cat_tagged_os_xor_ss_pion_value_ = 0;
    *var_tag_eta_os_ss_pion_value_ = 0.5;
  }

  // sin2beta OS (with neural net kaon) + SSPion combination
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

int Bd2JpsiKS(const std::string& inputfile, const std::string& inputtree, const std::string& outputfile, const std::string& outputtree, const std::string& decay_channel);

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "DooVariablesGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, decay_channel;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    decay_channel = argv[5];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "DooVariablesGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "DooVariablesGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name decay_channel" << doocore::io::endmsg;
    return 1;
  }

  if (decay_channel=="Bd2JpsiKS"){
    doocore::io::sinfo << "-info-  \t" << "DooVariablesGrimReaper \t" << "Decay: Bd2JpsiKS" << doocore::io::endmsg;
    Bd2JpsiKS(inputfile, inputtree, outputfile, outputtree, decay_channel);
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "DooVariablesGrimReaper \t" << "No valid decay channel. Possible decay channels are:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "DooVariablesGrimReaper \t" << "- Bd2JspiKS" << doocore::io::endmsg;
  }
  doocore::io::sinfo << "-info- \t" << "DooVariablesGrimReaper \t" << "Done!" << doocore::io::endmsg;
}

int Bd2JpsiKS(const std::string& inputfile, const std::string& inputtree, const std::string& outputfile, const std::string& outputtree, const std::string& decay_channel){
  TaggingRdcr reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  reducer.Initialize();

  // random leaf
  TRandom3* random_generator_ = new TRandom3(42);
  ReducerLeaf<Int_t>& random_leaf = reducer.CreateIntLeaf("idxRandom", -1);
  random_leaf.Randomize(random_generator_);

  // TODO: substitute this
  // BkgCategorizerReducer::CreateSpecialBranches();

  // handle MC specific stuff
  if (reducer.LeafExists("B0_BKGCAT")){
    ReducerLeaf<Int_t>& bkgcat_leaf = reducer.CreateIntCopyLeaf("catBkg", reducer.GetInterimLeafByName("B0_BKGCAT"));

  //   if (reducer.LeafExists("B0_DECAYDEPTH")) {
  //     reducer.set_decay_matrix_name("B0_DECAYMTRX");
  //     reducer.set_decay_matrix_length_leaf<Float_t>(reducer.GetInterimLeafByName("B0_DECAYMTRX_LENGTH"));
  //     reducer.set_decay_depth_leaf<Float_t>(reducer.GetInterimLeafByName("B0_DECAYDEPTH"));
  //     reducer.set_decay_products_leaf<Float_t>(reducer.GetInterimLeafByName("B0_DECAYPRODUCTS"));
  //     dooselection::reducer::ReducerLeaf<Int_t>& bkgcat_leaf = reducer.CreateIntLeaf("catBkgCategorizedDecays");
  //     reducer.set_background_category_leaf(bkgcat_leaf);
  //   }

    // flavour tagging true tag
    ReducerLeaf<Int_t>& vtag_true_leaf = reducer.CreateIntLeaf("obsTag_True", 0);
    vtag_true_leaf.AddCondition("B0",    "B0_TRUEID>0", +1);
    vtag_true_leaf.AddCondition("B0bar", "B0_TRUEID<0", -1);

    ReducerLeaf<Int_t>& vtag_true_babar_leaf = reducer.CreateIntLeaf("obsTag_BaBar_True", 0);
    vtag_true_babar_leaf.AddCondition("B0",    "B0_TRUEID>0", -1);
    vtag_true_babar_leaf.AddCondition("B0bar", "B0_TRUEID<0", +1);
  }

  // handle flattened tuples
  std::string flat_suffix = "";
  if (reducer.LeafExists("flat_array_index") || reducer.LeafExists("idxPV")){
    doocore::io::sinfo << "You are running the reducer over a flat tuple!" << doocore::io::endmsg;
    if (!(reducer.LeafExists("idxPV"))) ReducerLeaf<Int_t>& flat_index_leaf_ptr = reducer.CreateIntCopyLeaf("idxPV", reducer.GetInterimLeafByName("flat_array_index"));
    flat_suffix = "_flat";
  }
  ReducerLeaf<Int_t>& event_number_leaf_ptr = reducer.CreateIntCopyLeaf("idxEventNumber", reducer.GetInterimLeafByName("eventNumber"));
  ReducerLeaf<Int_t>& run_number_leaf_ptr = reducer.CreateIntCopyLeaf("idxRunNumber", reducer.GetInterimLeafByName("runNumber"));

  // time leaves
  ReducerLeaf<Double_t>* tau_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* tau_true_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* tau_err_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* tau_true_err_leaf_ptr = NULL;

  if (reducer.LeafExists("B0_FitPVConst_tau")) {
    tau_leaf_ptr = &reducer.CreateDoubleCopyLeaf("obsTime", reducer.GetInterimLeafByName("B0_FitPVConst_tau"+flat_suffix), 1000.0);
    tau_err_leaf_ptr = &reducer.CreateDoubleCopyLeaf("obsTimeErr", reducer.GetInterimLeafByName("B0_FitPVConst_tauErr"+flat_suffix), 1000.0);
    if(reducer.LeafExists("B0_FitPVConst_KS0_tau")){
      reducer.CreateDoubleLeaf("varKS0TauSignificance", -99999999.).Divide(reducer.GetInterimLeafByName("B0_FitPVConst_KS0_tau"+flat_suffix), reducer.GetInterimLeafByName("B0_FitPVConst_KS0_tauErr"+flat_suffix));
    }
  }
  else if (reducer.LeafExists("B0_LOKI_DTF_CTAU")){
    tau_leaf_ptr = &reducer.CreateDoubleCopyLeaf("obsTime", reducer.GetInterimLeafByName("B0_LOKI_DTF_CTAU"), 1.0/0.299792458);
    tau_err_leaf_ptr = &reducer.CreateDoubleCopyLeaf("obsTimeErr", reducer.GetInterimLeafByName("B0_LOKI_DTF_CTAUERR"), 1.0/0.299792458);
  }
  if (reducer.LeafExists("B0_BKGCAT")){
    tau_true_leaf_ptr = &reducer.CreateDoubleCopyLeaf("obsTime_True", reducer.GetInterimLeafByName("B0_TRUETAU"), 1000.0);
    tau_true_err_leaf_ptr = &reducer.CreateDoubleCopyLeaf("obsTimeErr_True", reducer.GetInterimLeafByName("B0_TRUETAU"), 1000.0); // ???????
  }

  // mass leaves
  std::string fit_constrains;
  ReducerLeaf<Double_t>* mass_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* mass_err_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* jpsi_mass_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* ks0_mass_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* jpsi_mass_err_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* ks0_mass_err_leaf_ptr = NULL;
  if (reducer.LeafExists("B0_FitDaughtersPVConst_M")) {
    fit_constrains = "PVJpsiKSConst";
    mass_leaf_ptr          = &reducer.CreateDoubleCopyLeaf("obsMass", reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_M"+flat_suffix));
    mass_err_leaf_ptr      = &reducer.CreateDoubleCopyLeaf("obsMassErr", reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_MERR"+flat_suffix));
    jpsi_mass_leaf_ptr     = &reducer.CreateDoubleCopyLeaf("obsMassDauOne", reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_J_psi_1S_M"+flat_suffix));
    jpsi_mass_err_leaf_ptr = &reducer.CreateDoubleCopyLeaf("obsMassErrDauOne", reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_J_psi_1S_MERR"+flat_suffix));
    if (reducer.LeafExists("B0_FitDaughtersPVConst_KS0_M") && reducer.LeafExists("B0_FitDaughtersPVConst_KS0_MERR")) {
      ks0_mass_leaf_ptr     = &reducer.CreateDoubleCopyLeaf("obsMassDauTwo", reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_M"+flat_suffix));
      ks0_mass_err_leaf_ptr = &reducer.CreateDoubleCopyLeaf("obsMassErrDauTwo", reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_MERR"+flat_suffix));
    }
  } else if (reducer.LeafExists("B0_LOKI_MASS_JpsiKSConstr")) {
    fit_constrains = "JpsiKSConst";
    mass_leaf_ptr          = &reducer.CreateDoubleCopyLeaf("obsMass", reducer.GetInterimLeafByName("B0_LOKI_MASS_JpsiKSConstr"));
    mass_err_leaf_ptr      = &reducer.CreateDoubleCopyLeaf("obsMassErr", reducer.GetInterimLeafByName("B0_LOKI_MASSERR_JpsiKSConstr"));
    jpsi_mass_leaf_ptr     = &reducer.CreateDoubleCopyLeaf("obsMassDauOne", reducer.GetInterimLeafByName("J_psi_1S_MM"));
    jpsi_mass_err_leaf_ptr = &reducer.CreateDoubleCopyLeaf("obsMassErrDauOne", reducer.GetInterimLeafByName("J_psi_1S_MMERR"));
    if (reducer.LeafExists("KS0_MM")) {
      ks0_mass_leaf_ptr      = &reducer.CreateDoubleCopyLeaf("obsMassDauTwo", reducer.GetInterimLeafByName("KS0_MM"));
      jpsi_mass_err_leaf_ptr = &reducer.CreateDoubleCopyLeaf("obsMassErrDauTwo", reducer.GetInterimLeafByName("KS0_MMERR"));
    }
  } else if (reducer.LeafExists("B0_LOKI_MASS_JpsiConstr")) {
    fit_constrains = "JpsiConst";
    mass_leaf_ptr          = &reducer.CreateDoubleCopyLeaf("obsMass", reducer.GetInterimLeafByName("B0_LOKI_MASS_JpsiConstr"));
    mass_err_leaf_ptr      = &reducer.CreateDoubleCopyLeaf("obsMassErr", reducer.GetInterimLeafByName("B0_LOKI_MASSERR_JpsiConstr"));
    jpsi_mass_leaf_ptr     = &reducer.CreateDoubleCopyLeaf("obsMassDauOne", reducer.GetInterimLeafByName("J_psi_1S_MM"));
    jpsi_mass_err_leaf_ptr = &reducer.CreateDoubleCopyLeaf("obsMassErrDauOne", reducer.GetInterimLeafByName("J_psi_1S_MMERR"));
    if (reducer.LeafExists("KS0_MM")) {
      ks0_mass_leaf_ptr      = &reducer.CreateDoubleCopyLeaf("obsMassDauTwo", reducer.GetInterimLeafByName("KS0_MM"));
      jpsi_mass_err_leaf_ptr = &reducer.CreateDoubleCopyLeaf("obsMassErrDauTwo", reducer.GetInterimLeafByName("KS0_MMERR"));
    }
  }
  doocore::io::sinfo << "Fit constrain is: " << fit_constrains << doocore::io::endmsg;

  // number of PVs
  ReducerLeaf<Int_t>& var_npv_leaf = reducer.CreateIntCopyLeaf("catNPV", reducer.GetInterimLeafByName("nPV"));

  // magnet direction
  ReducerLeaf<Int_t>& var_mag_leaf = reducer.CreateIntCopyLeaf("catMag", reducer.GetInterimLeafByName("Polarity"));
  
  // number of tracks
  ReducerLeaf<Int_t>& var_ntrack_leaf = reducer.CreateIntCopyLeaf("catNTrack", reducer.GetInterimLeafByName("nTracks"));

  // data taking period
  ReducerLeaf<Int_t>& cat_year_leaf = reducer.CreateIntLeaf("catYear", 0);
    cat_year_leaf.AddCondition("2011", "GpsTime < 1.325376e+15",  2011);
    cat_year_leaf.AddCondition("2012", "GpsTime >= 1.325376e+15", 2012);

  // maximal muon track fit chi2ndof
  reducer.CreateDoubleLeaf("varMuonMaxTrackFitChi2ndof", -999999.).Maximum(reducer.GetInterimLeafByName("muplus_TRACK_CHI2NDOF"), reducer.GetInterimLeafByName("muminus_TRACK_CHI2NDOF"));

  // maximal pion track fit chi2ndof
  reducer.CreateDoubleLeaf("varPionMaxTrackFitChi2ndof", -999999.).Maximum(reducer.GetInterimLeafByName("piplus_TRACK_CHI2NDOF"), reducer.GetInterimLeafByName("piminus_TRACK_CHI2NDOF"));

  // minimal muon PID
  reducer.CreateDoubleLeaf("varMuonMinPIDmu", -999999.).Minimum(reducer.GetInterimLeafByName("muplus_PIDmu"), reducer.GetInterimLeafByName("muminus_PIDmu"));

  // minimal pion MinIP chi2
  reducer.CreateDoubleLeaf("varPionMinMinIPChi2", -999999.).Minimum(reducer.GetInterimLeafByName("piplus_MINIPCHI2"), reducer.GetInterimLeafByName("piminus_MINIPCHI2"));

  // decay tree fit
  // fit status
  ReducerLeaf<Int_t>& dtf_status_pv_constraint = reducer.CreateIntCopyLeaf("varDTFStatusPVConst", reducer.GetInterimLeafByName("B0_FitPVConst_status"+flat_suffix));
  ReducerLeaf<Int_t>& dtf_status_daughters_pv_constraint = reducer.CreateIntCopyLeaf("varDTFStatusDaughtersPVConst", reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_status"+flat_suffix));

  // chi2ndof
  ReducerLeaf<Double_t>* dtf_chi2ndof_leaf_ptr = NULL;
  if (reducer.LeafExists("B0_FitDaughtersPVConst_chi2")) {
    dtf_chi2ndof_leaf_ptr = &reducer.CreateDoubleLeaf("varDTFChi2ndof", -1.0);
    dtf_chi2ndof_leaf_ptr->Divide(reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_chi2"+flat_suffix),
                                  reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_nDOF"+flat_suffix));
  } else if (reducer.LeafExists("B0_LOKI_DTF_CHI2NDOF")) {
    dtf_chi2ndof_leaf_ptr = &reducer.CreateDoubleCopyLeaf("varDTFChi2ndof", reducer.GetInterimLeafByName("B0_LOKI_DTF_CHI2NDOF"));
  }

  // alternative masses with different constraints
  if (reducer.LeafExists("B0_FitPVConst_KS0_M"+flat_suffix)) ReducerLeaf<Double_t>& dtf_kaon_mass_pv_constraint = reducer.CreateDoubleCopyLeaf("varDTFKS0MassPVConst", reducer.GetInterimLeafByName("B0_FitPVConst_KS0_M"+flat_suffix));
  if (reducer.LeafExists("B0_FitDaughtersPVConst_KS0_M"+flat_suffix)) ReducerLeaf<Double_t>& dtf_kaon_mass_daughters_pv_constraint = reducer.CreateDoubleCopyLeaf("varDTFKS0MassDaughtersPVConst", reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_M"+flat_suffix));
  if (reducer.LeafExists("B0_FitPVConst_J_psi_1S_M"+flat_suffix)) ReducerLeaf<Double_t>& dtf_kaon_mass_pv_constraint = reducer.CreateDoubleCopyLeaf("varDTFJpsiMassPVConst", reducer.GetInterimLeafByName("B0_FitPVConst_J_psi_1S_M"+flat_suffix));
  if (reducer.LeafExists("B0_FitDaughtersPVConst_J_psi_1S_M"+flat_suffix)) ReducerLeaf<Double_t>& dtf_kaon_mass_daughters_pv_constraint = reducer.CreateDoubleCopyLeaf("varDTFJpsiMassDaughtersPVConst", reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_J_psi_1S_M"+flat_suffix));

  // DTF PV position
  if (reducer.LeafExists("B0_FitDaughtersPVConst_PV_X"+flat_suffix)) ReducerLeaf<Double_t>& dtf_pv_position_x = reducer.CreateDoubleCopyLeaf("varDTFPVPosX", reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_PV_X"+flat_suffix));
  if (reducer.LeafExists("B0_FitDaughtersPVConst_PV_Y"+flat_suffix)) ReducerLeaf<Double_t>& dtf_pv_position_y = reducer.CreateDoubleCopyLeaf("varDTFPVPosY", reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_PV_Y"+flat_suffix));
  if (reducer.LeafExists("B0_FitDaughtersPVConst_PV_Z"+flat_suffix)) ReducerLeaf<Double_t>& dtf_pv_position_z = reducer.CreateDoubleCopyLeaf("varDTFPVPosZ", reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_PV_Z"+flat_suffix));

  // minimal muon transverse momentum
  if (reducer.LeafExists("B0_FitPVConst_J_psi_1S_P0_PT_flat") && reducer.LeafExists("B0_FitPVConst_J_psi_1S_P1_PT_flat")){
    reducer.CreateDoubleLeaf("varMuonDTFMinPT", -999999.).Minimum(reducer.GetInterimLeafByName("B0_FitPVConst_J_psi_1S_P0_PT_flat"), reducer.GetInterimLeafByName("B0_FitPVConst_J_psi_1S_P1_PT_flat"));
  }

  // minimal pion momentum
  if (reducer.LeafExists("B0_FitPVConst_KS0_P0_P_flat") && reducer.LeafExists("B0_FitPVConst_KS0_P1_P_flat")){
    reducer.CreateDoubleLeaf("varPionDTFMinP", -999999.).Minimum(reducer.GetInterimLeafByName("B0_FitPVConst_KS0_P0_P_flat"), reducer.GetInterimLeafByName("B0_FitPVConst_KS0_P1_P_flat"));
  }

  // End vertex chi2/ndof
  reducer.CreateDoubleLeaf("varBEndVtxChi2ndof", -99999999.).Divide(reducer.GetInterimLeafByName("B0_ENDVERTEX_CHI2"), reducer.GetInterimLeafByName("B0_ENDVERTEX_NDOF"));
  reducer.CreateDoubleLeaf("varJPsiEndVtxChi2ndof", -99999999.).Divide(reducer.GetInterimLeafByName("J_psi_1S_ENDVERTEX_CHI2"), reducer.GetInterimLeafByName("J_psi_1S_ENDVERTEX_NDOF"));
  reducer.CreateDoubleLeaf("varKSEndVtxChi2ndof", -99999999.).Divide(reducer.GetInterimLeafByName("KS0_ENDVERTEX_CHI2"), reducer.GetInterimLeafByName("KS0_ENDVERTEX_NDOF"));

  // track ghost probability
  ReducerLeaf<Double_t>* pip_track_ghost_prob_leaf_ptr = &reducer.CreateDoubleCopyLeaf("varPiPTrackGhostProb", reducer.GetInterimLeafByName("piplus_TRACK_GhostProb"));
  ReducerLeaf<Double_t>* pim_track_ghost_prob_leaf_ptr = &reducer.CreateDoubleCopyLeaf("varPiMTrackGhostProb", reducer.GetInterimLeafByName("piminus_TRACK_GhostProb"));
  ReducerLeaf<Double_t>* mup_track_ghost_prob_leaf_ptr = &reducer.CreateDoubleCopyLeaf("varMuPTrackGhostProb", reducer.GetInterimLeafByName("muplus_TRACK_GhostProb"));
  ReducerLeaf<Double_t>* mum_track_ghost_prob_leaf_ptr = &reducer.CreateDoubleCopyLeaf("varMuMTrackGhostProb", reducer.GetInterimLeafByName("muminus_TRACK_GhostProb"));

  // track type category
  ReducerLeaf<Int_t>& catTrackType = reducer.CreateIntLeaf("catTrackType", 0);
  if(reducer.LeafExists("piplus_TRACK_Type")){
    catTrackType.AddCondition("longtrack", "(piplus_TRACK_Type==3)&&(piminus_TRACK_Type==3)", 33);
    catTrackType.AddCondition("downstream", "(piplus_TRACK_Type==5)&&(piminus_TRACK_Type==5)", 55);
    catTrackType.AddCondition("longdown", "(piplus_TRACK_Type==3)&&(piminus_TRACK_Type==5)", 35);
    catTrackType.AddCondition("downlong", "(piplus_TRACK_Type==5)&&(piminus_TRACK_Type==3)", 53);
  }

  // Flavour Tagging
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
  ReducerLeaf<Int_t>& var_tag_os_kaon_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOSKaon_BaBar", reducer.GetInterimLeafByName("B0_OS_Kaon_DEC"), -1.0);
  ReducerLeaf<Int_t>& var_tag_os_nnet_kaon_leaf = reducer.CreateIntCopyLeaf("obsTagOSNNKaon", reducer.GetInterimLeafByName("B0_OS_nnetKaon_DEC"));
  ReducerLeaf<Int_t>& var_tag_os_nnet_kaon_babar_leaf = reducer.CreateIntCopyLeaf("obsTagOSNNKaon_BaBar", reducer.GetInterimLeafByName("B0_OS_nnetKaon_DEC"), -1.0);
  ReducerLeaf<Int_t>& var_tag_ss_kaon_leaf = reducer.CreateIntCopyLeaf("obsTagSSKaon", reducer.GetInterimLeafByName("B0_SS_Kaon_DEC"));
  ReducerLeaf<Int_t>& var_tag_ss_kaon_babar_leaf = reducer.CreateIntCopyLeaf("obsTagSSKaon_BaBar", reducer.GetInterimLeafByName("B0_SS_Kaon_DEC"), -1.0);
  ReducerLeaf<Int_t>& var_tag_ss_nnet_kaon_leaf = reducer.CreateIntCopyLeaf("obsTagSSNNKaon", reducer.GetInterimLeafByName("B0_SS_nnetKaon_DEC"));
  ReducerLeaf<Int_t>& var_tag_ss_nnet_kaon_babar_leaf = reducer.CreateIntCopyLeaf("obsTagSSNNKaon_BaBar", reducer.GetInterimLeafByName("B0_SS_nnetKaon_DEC"), -1.0);
  ReducerLeaf<Int_t>& var_tag_ss_pion_leaf = reducer.CreateIntCopyLeaf("obsTagSSPion", reducer.GetInterimLeafByName("B0_SS_Pion_DEC"));
  ReducerLeaf<Int_t>& var_tag_ss_pion_babar_leaf = reducer.CreateIntCopyLeaf("obsTagSSPion_BaBar", reducer.GetInterimLeafByName("B0_SS_Pion_DEC"), -1.0);
  ReducerLeaf<Int_t>& var_tag_vtxq_leaf = reducer.CreateIntCopyLeaf("obsTagVtxQ", reducer.GetInterimLeafByName("B0_VtxCharge_DEC"));
  ReducerLeaf<Int_t>& var_tag_vtxq_babar_leaf = reducer.CreateIntCopyLeaf("obsTagVtxQ_BaBar", reducer.GetInterimLeafByName("B0_VtxCharge_DEC"), -1.0);

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
  ReducerLeaf<Double_t>& var_tag_eta_os_nnet_kaon_leaf = reducer.CreateDoubleCopyLeaf("obsEtaOSNNKaon", reducer.GetInterimLeafByName("B0_OS_nnetKaon_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_ss_kaon_leaf = reducer.CreateDoubleCopyLeaf("obsEtaSSKaon", reducer.GetInterimLeafByName("B0_SS_Kaon_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_ss_nnet_kaon_leaf = reducer.CreateDoubleCopyLeaf("obsEtaSSNNKaon", reducer.GetInterimLeafByName("B0_SS_nnetKaon_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_ss_pion_leaf = reducer.CreateDoubleCopyLeaf("obsEtaSSPion", reducer.GetInterimLeafByName("B0_SS_Pion_PROB"));
  ReducerLeaf<Double_t>& var_tag_eta_vtxq_leaf = reducer.CreateDoubleCopyLeaf("obsEtaVtxQ", reducer.GetInterimLeafByName("B0_VtxCharge_PROB"));

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
  
  ReducerLeaf<Int_t>& cat_tagged_os_nnet_kaon_leaf = reducer.CreateIntLeaf("catTaggedOSNNKaon", -1);
    cat_tagged_os_nnet_kaon_leaf.AddCondition("Tagged",   "B0_OS_nnetKaon_DEC != 0", 1);
    cat_tagged_os_nnet_kaon_leaf.AddCondition("Untagged", "B0_OS_nnetKaon_DEC == 0", 0);

  ReducerLeaf<Int_t>& cat_tagged_ss_kaon_leaf = reducer.CreateIntLeaf("catTaggedSSKaon", -1);
    cat_tagged_ss_kaon_leaf.AddCondition("Tagged",   "B0_SS_Kaon_DEC != 0", 1);
    cat_tagged_ss_kaon_leaf.AddCondition("Untagged", "B0_SS_Kaon_DEC == 0", 0);

  ReducerLeaf<Int_t>& cat_tagged_ss_nnet_kaon_leaf = reducer.CreateIntLeaf("catTaggedSSNNKaon", -1);
    cat_tagged_ss_nnet_kaon_leaf.AddCondition("Tagged",   "B0_SS_nnetKaon_DEC != 0", 1);
    cat_tagged_ss_nnet_kaon_leaf.AddCondition("Untagged", "B0_SS_nnetKaon_DEC == 0", 0);

  ReducerLeaf<Int_t>& cat_tagged_ss_pion_leaf = reducer.CreateIntLeaf("catTaggedSSPion", -1);
    cat_tagged_ss_pion_leaf.AddCondition("Tagged",   "B0_SS_Pion_DEC != 0", 1);
    cat_tagged_ss_pion_leaf.AddCondition("Untagged", "B0_SS_Pion_DEC == 0", 0);

  ReducerLeaf<Int_t>& cat_tagged_vtxq_leaf = reducer.CreateIntLeaf("catTaggedVtxQ", -1);
    cat_tagged_vtxq_leaf.AddCondition("Tagged",   "B0_VtxCharge_DEC != 0", 1);
    cat_tagged_vtxq_leaf.AddCondition("Untagged", "B0_VtxCharge_DEC == 0", 0);

  // catFinalState
  ReducerLeaf<Int_t>& var_finalstate_leaf = reducer.CreateIntLeaf("catFinalState", 0);
    var_finalstate_leaf.AddCondition("B0",    "B0_ID>0", +1);
    var_finalstate_leaf.AddCondition("B0bar", "B0_ID<0", -1);

  // catXi
  ReducerLeaf<Int_t>& xi_leaf = reducer.CreateIntLeaf("catXiAll");
    xi_leaf.Multiply(var_finalstate_leaf, var_tag_all_leaf);
  ReducerLeaf<Int_t>& xi_os = reducer.CreateIntLeaf("catXiOS");
    xi_os.Multiply(var_finalstate_leaf, var_tag_os_leaf);

  // trigger categories
  ReducerLeaf<Int_t>& trigger_l0_global_leaf_ptr = reducer.CreateIntCopyLeaf("catTriggerL0GlobalTOS", reducer.GetInterimLeafByName("J_psi_1S_L0Global_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt1_global_leaf_ptr = reducer.CreateIntCopyLeaf("catTriggerHlt1GlobalTOS", reducer.GetInterimLeafByName("J_psi_1S_Hlt1Global_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt2_global_leaf_ptr = reducer.CreateIntCopyLeaf("catTriggerHlt2GlobalTOS", reducer.GetInterimLeafByName("J_psi_1S_Hlt2Global_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt1_trackmuon_leaf_ptr = reducer.CreateIntCopyLeaf("catTriggerHlt1TrackMuonTOS", reducer.GetInterimLeafByName("J_psi_1S_Hlt1TrackMuonDecision_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt1_highmass_leaf_ptr = reducer.CreateIntCopyLeaf("catTriggerHlt1HighMassTOS", reducer.GetInterimLeafByName("J_psi_1S_Hlt1DiMuonHighMassDecision_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt2_detachedjpsi_leaf_ptr = reducer.CreateIntCopyLeaf("catTriggerHlt2DetachedJpsiTOS", reducer.GetInterimLeafByName("J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt2_jpsi_leaf_ptr = reducer.CreateIntCopyLeaf("catTriggerHlt2JpsiTOS", reducer.GetInterimLeafByName("J_psi_1S_Hlt2DiMuonJPsiDecision_TOS"));
  
  ReducerLeaf<Int_t>& catTriggerSetFull = reducer.CreateIntLeaf("catTriggerSetFull", -10);
      TCut trigger_set_full = "((J_psi_1S_Hlt1DiMuonHighMassDecision_TOS==1)||(J_psi_1S_Hlt1TrackMuonDecision_TOS==1))&&(J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS==1)";
      catTriggerSetFull.AddCondition("triggered", trigger_set_full.GetTitle(), 1);
      catTriggerSetFull.AddCondition("not_triggered", (!trigger_set_full).GetTitle(), 0);

  ReducerLeaf<Int_t>& catTriggerSetAlmostUnbiased = reducer.CreateIntLeaf("catTriggerSetAlmostUnbiased", -10);
      TCut trigger_set_almost_unbiased = "J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS==1&&J_psi_1S_Hlt1DiMuonHighMassDecision_TOS==1";
      TCut trigger_set_almost_unbiased_and_jpsi = "J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS==1&&J_psi_1S_Hlt1DiMuonHighMassDecision_TOS==1&&J_psi_1S_Hlt2DiMuonJPsiDecision_TOS==1";
      catTriggerSetAlmostUnbiased.AddCondition("triggered", trigger_set_almost_unbiased.GetTitle(), 1);
      catTriggerSetAlmostUnbiased.AddCondition("not_triggered", (!trigger_set_almost_unbiased).GetTitle(), 0);

  ReducerLeaf<Int_t>& catTriggerSetExclBiased = reducer.CreateIntLeaf("catTriggerSetExclBiased", -10);
      TCut trigger_set_excl_biased = "J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS==1&&J_psi_1S_Hlt1TrackMuonDecision_TOS==1&&J_psi_1S_Hlt1DiMuonHighMassDecision_TOS==0";
      catTriggerSetExclBiased.AddCondition("triggered", trigger_set_excl_biased.GetTitle(), 1);
      catTriggerSetExclBiased.AddCondition("not_triggered", (!trigger_set_excl_biased).GetTitle(), 0);

  ReducerLeaf<Int_t>& catTriggerHlt1TrackMuonAndHlt2DetachedJpsi = reducer.CreateIntLeaf("catTriggerHlt1TrackMuonAndHlt2DetachedJpsi", -10);
      TCut trigger_hlt1trackmuon_hlt2detached = "J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS==1&&J_psi_1S_Hlt1TrackMuonDecision_TOS==1";
      catTriggerHlt1TrackMuonAndHlt2DetachedJpsi.AddCondition("triggered", trigger_hlt1trackmuon_hlt2detached.GetTitle(), 1);
      catTriggerHlt1TrackMuonAndHlt2DetachedJpsi.AddCondition("not_triggered", (!trigger_hlt1trackmuon_hlt2detached).GetTitle(), 0);

  ReducerLeaf<Int_t>& catTriggerHlt1HighMassAndHlt2Jpsi = reducer.CreateIntLeaf("catTriggerHlt1HighMassAndHlt2Jpsi", -10);
      TCut trigger_hlt1highmass_hlt2jpsi = "J_psi_1S_Hlt1DiMuonHighMassDecision_TOS==1&&J_psi_1S_Hlt2DiMuonJPsiDecision_TOS==1";
      catTriggerHlt1HighMassAndHlt2Jpsi.AddCondition("triggered", trigger_hlt1highmass_hlt2jpsi.GetTitle(), 1);
      catTriggerHlt1HighMassAndHlt2Jpsi.AddCondition("not_triggered", (!trigger_hlt1highmass_hlt2jpsi).GetTitle(), 0);

  ReducerLeaf<Int_t>& catTriggerSets = reducer.CreateIntLeaf("catTriggerSets", -10);
      catTriggerSets.AddCondition("almost_unbiased", trigger_set_almost_unbiased_and_jpsi.GetTitle(), 0);
      catTriggerSets.AddCondition("excl_biased", trigger_set_excl_biased.GetTitle(), 1);

  ReducerLeaf<Int_t>& catTrigger = reducer.CreateIntLeaf("catTrigger", -10);
      catTrigger.AddCondition("almost_unbiased", trigger_set_almost_unbiased.GetTitle(), 0);
      catTrigger.AddCondition("excl_biased", trigger_set_excl_biased.GetTitle(), 1);

  // veto leafs
  std::string piplus_px, piplus_py, piplus_pz;
  std::string piminus_px, piminus_py, piminus_pz;
  std::string muplus_px, muplus_py, muplus_pz;
  std::string muminus_px, muminus_py, muminus_pz;

  std::string mass_hypo_constrains = "";
  if (reducer.LeafExists("B0_FitJpsiPVConst_KS0_P0_PX")){
    piplus_px  = "B0_FitJpsiPVConst_KS0_P0_PX"+flat_suffix;   
    piplus_py  = "B0_FitJpsiPVConst_KS0_P0_PY"+flat_suffix;   
    piplus_pz  = "B0_FitJpsiPVConst_KS0_P0_PZ"+flat_suffix;   
    piminus_px = "B0_FitJpsiPVConst_KS0_P1_PX"+flat_suffix;     
    piminus_py = "B0_FitJpsiPVConst_KS0_P1_PY"+flat_suffix;     
    piminus_pz = "B0_FitJpsiPVConst_KS0_P1_PZ"+flat_suffix;  
    muplus_px  = "B0_FitJpsiPVConst_J_psi_1S_P0_PX"+flat_suffix;   
    muplus_py  = "B0_FitJpsiPVConst_J_psi_1S_P0_PY"+flat_suffix;   
    muplus_pz  = "B0_FitJpsiPVConst_J_psi_1S_P0_PZ"+flat_suffix;   
    muminus_px = "B0_FitJpsiPVConst_J_psi_1S_P1_PX"+flat_suffix;     
    muminus_py = "B0_FitJpsiPVConst_J_psi_1S_P1_PY"+flat_suffix;     
    muminus_pz = "B0_FitJpsiPVConst_J_psi_1S_P1_PZ"+flat_suffix;
    mass_hypo_constrains = "JpsiPV";
  }
  else if (reducer.LeafExists("piplus_PX")){
    piplus_px  = "piplus_PX";
    piplus_py  = "piplus_PY";
    piplus_pz  = "piplus_PZ";
    piminus_px = "piminus_PX";
    piminus_py = "piminus_PY";
    piminus_pz = "piminus_PZ";
    muplus_px  = "muplus_PX";   
    muplus_py  = "muplus_PY";   
    muplus_pz  = "muplus_PZ";   
    muminus_px = "muminus_PX";     
    muminus_py = "muminus_PY";     
    muminus_pz = "muminus_PZ";
    mass_hypo_constrains = "NoConstr";
  }
  
  if (mass_hypo_constrains!=""){
    // mass hypotheses
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_pipi = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_pipi", "varKS0MassHypo_pipi", "Double_t", NULL);
    varKS0MassHypo_pipi->FixedMassDaughtersTwoBodyDecayMotherMass(
        reducer.GetInterimLeafByName(piplus_px),
        reducer.GetInterimLeafByName(piplus_py),
        reducer.GetInterimLeafByName(piplus_pz),
        139.57018,
        reducer.GetInterimLeafByName(piminus_px),
        reducer.GetInterimLeafByName(piminus_py),
        reducer.GetInterimLeafByName(piminus_pz),
        139.57018);
    reducer.RegisterDoubleLeaf(varKS0MassHypo_pipi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Jpsipipi = new KinematicReducerLeaf<Double_t>("varBMassHypo_Jpsipipi", "varBMassHypo_Jpsipipi", "Double_t", NULL);
    varBMassHypo_Jpsipipi->FixedMassDaughtersFourBodyDecayMotherMass(
        reducer.GetInterimLeafByName(piplus_px),
        reducer.GetInterimLeafByName(piplus_py),
        reducer.GetInterimLeafByName(piplus_pz),
        139.57018,
        reducer.GetInterimLeafByName(piminus_px),
        reducer.GetInterimLeafByName(piminus_py),
        reducer.GetInterimLeafByName(piminus_pz),
        139.57018,
        reducer.GetInterimLeafByName(muplus_px),
        reducer.GetInterimLeafByName(muplus_py),
        reducer.GetInterimLeafByName(muplus_pz),
        105.6583715,
        reducer.GetInterimLeafByName(muminus_px),
        reducer.GetInterimLeafByName(muminus_py),
        reducer.GetInterimLeafByName(muminus_pz),
        105.6583715);
    reducer.RegisterDoubleLeaf(varBMassHypo_Jpsipipi);

    
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_piK = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_piK", "varKS0MassHypo_piK", "Double_t", NULL);
    varKS0MassHypo_piK->FixedMassDaughtersTwoBodyDecayMotherMass(
        reducer.GetInterimLeafByName(piplus_px),
        reducer.GetInterimLeafByName(piplus_py),
        reducer.GetInterimLeafByName(piplus_pz),
        139.57018,
        reducer.GetInterimLeafByName(piminus_px),
        reducer.GetInterimLeafByName(piminus_py),
        reducer.GetInterimLeafByName(piminus_pz),
        493.677);
    reducer.RegisterDoubleLeaf(varKS0MassHypo_piK);

    KinematicReducerLeaf<Double_t>* varBMassHypo_JpsipiK = new KinematicReducerLeaf<Double_t>("varBMassHypo_JpsipiK", "varBMassHypo_JpsipiK", "Double_t", NULL);
    varBMassHypo_JpsipiK->FixedMassDaughtersFourBodyDecayMotherMass(
        reducer.GetInterimLeafByName(piplus_px),
        reducer.GetInterimLeafByName(piplus_py),
        reducer.GetInterimLeafByName(piplus_pz),
        139.57018,
        reducer.GetInterimLeafByName(piminus_px),
        reducer.GetInterimLeafByName(piminus_py),
        reducer.GetInterimLeafByName(piminus_pz),
        493.677,
        reducer.GetInterimLeafByName(muplus_px),
        reducer.GetInterimLeafByName(muplus_py),
        reducer.GetInterimLeafByName(muplus_pz),
        105.6583715,
        reducer.GetInterimLeafByName(muminus_px),
        reducer.GetInterimLeafByName(muminus_py),
        reducer.GetInterimLeafByName(muminus_pz),
        105.6583715);
    reducer.RegisterDoubleLeaf(varBMassHypo_JpsipiK);
    
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_Kpi = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_Kpi", "varKS0MassHypo_Kpi", "Double_t", NULL);
    varKS0MassHypo_Kpi->FixedMassDaughtersTwoBodyDecayMotherMass(
        reducer.GetInterimLeafByName(piplus_px),
        reducer.GetInterimLeafByName(piplus_py),
        reducer.GetInterimLeafByName(piplus_pz),
        493.677,
        reducer.GetInterimLeafByName(piminus_px),
        reducer.GetInterimLeafByName(piminus_py),
        reducer.GetInterimLeafByName(piminus_pz),
        139.57018);
    reducer.RegisterDoubleLeaf(varKS0MassHypo_Kpi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_JpsiKpi = new KinematicReducerLeaf<Double_t>("varBMassHypo_JpsiKpi", "varBMassHypo_JpsiKpi", "Double_t", NULL);
    varBMassHypo_JpsiKpi->FixedMassDaughtersFourBodyDecayMotherMass(
        reducer.GetInterimLeafByName(piplus_px),
        reducer.GetInterimLeafByName(piplus_py),
        reducer.GetInterimLeafByName(piplus_pz),
        493.677,
        reducer.GetInterimLeafByName(piminus_px),
        reducer.GetInterimLeafByName(piminus_py),
        reducer.GetInterimLeafByName(piminus_pz),
        139.57018,
        reducer.GetInterimLeafByName(muplus_px),
        reducer.GetInterimLeafByName(muplus_py),
        reducer.GetInterimLeafByName(muplus_pz),
        105.6583715,
        reducer.GetInterimLeafByName(muminus_px),
        reducer.GetInterimLeafByName(muminus_py),
        reducer.GetInterimLeafByName(muminus_pz),
        105.6583715);
    reducer.RegisterDoubleLeaf(varBMassHypo_JpsiKpi);
    
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_pip = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_pip", "varKS0MassHypo_pip", "Double_t", NULL);
    varKS0MassHypo_pip->FixedMassDaughtersTwoBodyDecayMotherMass(
        reducer.GetInterimLeafByName(piplus_px),
        reducer.GetInterimLeafByName(piplus_py),
        reducer.GetInterimLeafByName(piplus_pz),
        139.57018,
        reducer.GetInterimLeafByName(piminus_px),
        reducer.GetInterimLeafByName(piminus_py),
        reducer.GetInterimLeafByName(piminus_pz),
        938.272046);
    reducer.RegisterDoubleLeaf(varKS0MassHypo_pip);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Jpsipip = new KinematicReducerLeaf<Double_t>("varBMassHypo_Jpsipip", "varBMassHypo_Jpsipip", "Double_t", NULL);
    varBMassHypo_Jpsipip->FixedMassDaughtersFourBodyDecayMotherMass(
        reducer.GetInterimLeafByName(piplus_px),
        reducer.GetInterimLeafByName(piplus_py),
        reducer.GetInterimLeafByName(piplus_pz),
        139.57018,
        reducer.GetInterimLeafByName(piminus_px),
        reducer.GetInterimLeafByName(piminus_py),
        reducer.GetInterimLeafByName(piminus_pz),
        938.272046,
        reducer.GetInterimLeafByName(muplus_px),
        reducer.GetInterimLeafByName(muplus_py),
        reducer.GetInterimLeafByName(muplus_pz),
        105.6583715,
        reducer.GetInterimLeafByName(muminus_px),
        reducer.GetInterimLeafByName(muminus_py),
        reducer.GetInterimLeafByName(muminus_pz),
        105.6583715);
    reducer.RegisterDoubleLeaf(varBMassHypo_Jpsipip);
    
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_ppi = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_ppi", "varKS0MassHypo_ppi", "Double_t", NULL);
    varKS0MassHypo_ppi->FixedMassDaughtersTwoBodyDecayMotherMass(
        reducer.GetInterimLeafByName(piplus_px),
        reducer.GetInterimLeafByName(piplus_py),
        reducer.GetInterimLeafByName(piplus_pz),
        938.272046,
        reducer.GetInterimLeafByName(piminus_px),
        reducer.GetInterimLeafByName(piminus_py),
        reducer.GetInterimLeafByName(piminus_pz),
        139.57018);
    reducer.RegisterDoubleLeaf(varKS0MassHypo_ppi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Jpsippi = new KinematicReducerLeaf<Double_t>("varBMassHypo_Jpsippi", "varBMassHypo_Jpsippi", "Double_t", NULL);
    varBMassHypo_Jpsippi->FixedMassDaughtersFourBodyDecayMotherMass(
        reducer.GetInterimLeafByName(piplus_px),
        reducer.GetInterimLeafByName(piplus_py),
        reducer.GetInterimLeafByName(piplus_pz),
        938.272046,
        reducer.GetInterimLeafByName(piminus_px),
        reducer.GetInterimLeafByName(piminus_py),
        reducer.GetInterimLeafByName(piminus_pz),
        139.57018,
        reducer.GetInterimLeafByName(muplus_px),
        reducer.GetInterimLeafByName(muplus_py),
        reducer.GetInterimLeafByName(muplus_pz),
        105.6583715,
        reducer.GetInterimLeafByName(muminus_px),
        reducer.GetInterimLeafByName(muminus_py),
        reducer.GetInterimLeafByName(muminus_pz),
        105.6583715);
    reducer.RegisterDoubleLeaf(varBMassHypo_Jpsippi);

    doocore::io::sinfo << "Veto leaves are filled using constrain: " << mass_hypo_constrains << doocore::io::endmsg;

    if (reducer.LeafExists("varKS0MassHypoDaughtersPVConst_pipi")){
      // mass hypotheses using the maximal constrained fit (WIP)
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_pipi = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_pipi", "varKS0MassHypoDaughtersPVConst_pipi", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_pipi->FixedMassDaughtersTwoBodyDecayMotherMass(
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          139.57018,
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          139.57018);
      reducer.RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_pipi);
      
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_piK = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_piK", "varKS0MassHypoDaughtersPVConst_piK", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_piK->FixedMassDaughtersTwoBodyDecayMotherMass(
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          139.57018,
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          493.677);
      reducer.RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_piK);
      
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_Kpi = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_Kpi", "varKS0MassHypoDaughtersPVConst_Kpi", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_Kpi->FixedMassDaughtersTwoBodyDecayMotherMass(
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          493.677,
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          139.57018);
      reducer.RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_Kpi);
      
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_pip = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_pip", "varKS0MassHypoDaughtersPVConst_pip", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_pip->FixedMassDaughtersTwoBodyDecayMotherMass(
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          139.57018,
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          938.272046);
      reducer.RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_pip);
      
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_ppi = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_ppi", "varKS0MassHypoDaughtersPVConst_ppi", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_ppi->FixedMassDaughtersTwoBodyDecayMotherMass(
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          938.272046,
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          reducer.GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          139.57018);
      reducer.RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_ppi);
    }
  }

  reducer.Run();
  reducer.Finalize();
}

