

// from STL

// from BOOST

// from ROOT
#include "TMath.h"
#include "TCut.h"

// from project
#include "dooselection/reducer/Reducer.h"
#include "dooselection/reducer/BkgCategorizerReducer.h"

bool VetoMass(Double_t wrong_mother_mass, Double_t dau_alt_mass, Double_t cut_value,
              Double_t dau1_E, Double_t dau1_p, Double_t dau1_px, Double_t dau1_py, Double_t dau1_pz,
              Double_t dau2_E, Double_t dau2_p, Double_t dau2_px, Double_t dau2_py, Double_t dau2_pz,
              Float_t* momMass1_p, Float_t* momMass2_p){
  // dau1 assumption wrong
  Double_t momE1 = sqrt(dau_alt_mass*dau_alt_mass + dau1_p*dau1_p)+dau2_E;
  // dau2 assumption wrong
  Double_t momE2 = sqrt(dau_alt_mass*dau_alt_mass + dau2_p*dau2_p)+dau1_E;
  
  Double_t momP = sqrt(  (dau1_px+dau2_px)*(dau1_px+dau2_px)
                      + (dau1_py+dau2_py)*(dau1_py+dau2_py)
                      + (dau1_pz+dau2_pz)*(dau1_pz+dau2_pz)
                      );
  
  Double_t momMass1 = sqrt(momE1*momE1-momP*momP);
  Double_t momMass2 = sqrt(momE2*momE2-momP*momP);
  
  Double_t momMassUpper = wrong_mother_mass+cut_value;
  Double_t momMassLower = wrong_mother_mass-cut_value;
  
  bool isOtherPart1 = ( momMassLower < momMass1 ) && ( momMass1 < momMassUpper );
  bool isOtherPart2 = ( momMassLower < momMass2 ) && ( momMass2 < momMassUpper );
  
  *momMass1_p = momMass1;
  *momMass2_p = momMass2;
  
  return (isOtherPart1||isOtherPart2);
}

TCut KSTreeCuts(){
  // Muon cuts
  TCut Mu, Mu_PID, Mu_TrkChi2, Mu_Pt;
  
  Mu_PID        = "min(muplus_PIDmu,muminus_PIDmu)>0";
  Mu_TrkChi2    = "max(muplus_TRACK_CHI2NDOF,muminus_TRACK_CHI2NDOF)<4.";
  Mu_Pt         = "min(muplus_PT,muminus_PT)>500";
  Mu            = Mu_PID&&Mu_TrkChi2&&Mu_Pt;
  Mu.Print();
  
  // Jpsi cuts  
  TCut Jpsi, Jpsi_VtxChi2, Jpsi_MMSigDev, Jpsi_MMWin;
  Jpsi_VtxChi2  = "(J_psi_1S_ENDVERTEX_CHI2/J_psi_1S_ENDVERTEX_NDOF)<11.";
  //Jpsi_MMSigDev   = "(abs(J_psi_1S_MM-3096.916)/J_psi_1S_MMERR)<4.2";
  Jpsi_MMWin = "J_psi_1S_MM>3030.&&J_psi_1S_MM<3165.";
  Jpsi          = Jpsi_VtxChi2&&Jpsi_MMWin;
  Jpsi.Print();
  
  // Pi cuts
  TCut Pi, Pi_TrkChi2, Pi_P;
  Pi_TrkChi2    = "max(piplus_TRACK_CHI2NDOF,piminus_TRACK_CHI2NDOF)<4.";
  Pi_P          = "min(piplus_P,piminus_P)>2000.";
  Pi            = Pi_TrkChi2&&Pi_P;
  Pi.Print();
  
  // KS cuts
  TCut KS, KS_DD_MMWin, KS_LL_MMWin, KS_VtxChi2, KS_TauSig, KS_Pt;
  KS_Pt       = "KS0_PT>1000.";
  KS_VtxChi2  = "(KS0_ENDVERTEX_CHI2/KS0_ENDVERTEX_NDOF)<20.";
  //KS_TauSig   = "abs(B0_LifetimeFit_KS0_ctau/B0_LifetimeFit_KS0_ctauErr)>5.";
  KS_TauSig   = "(KS0_TAU/KS0_TAUERR)>5.";
  KS_DD_MMWin = "(piplus_TRACK_Type==5)&&(piminus_TRACK_Type==5)&&(abs(KS0_MM-497.6)<21.)";
  KS_LL_MMWin = "(piplus_TRACK_Type==3)&&(piminus_TRACK_Type==3)&&(abs(KS0_MM-497.6)<12.)"; // 497.614
  KS          = KS_Pt&&KS_VtxChi2&&(KS_DD_MMWin||KS_LL_MMWin)&&KS_TauSig;
  KS.Print();
  
  // B cuts
  TCut B, B_IPChi2, B_NextIPChi2, B_DTFChi2, B_VtxChi2, B_MassWin;
  B_VtxChi2   = "(B0_ENDVERTEX_CHI2/B0_ENDVERTEX_NDOF)<10.";
  B_IPChi2    = "B0_MINIPCHI2<20";
  B_NextIPChi2= "(B0_MINIPCHI2NEXTBEST>50)||(B0_MINIPCHI2NEXTBEST==-1)";
  B_DTFChi2   = "B0_LOKI_DTF_CHI2NDOF<5.&&B0_LOKI_DTF_CHI2NDOF>=0";
  B_MassWin   = "(B0_LOKI_MASS_JpsiKSConstr>5170.)&&(B0_LOKI_MASS_JpsiKSConstr<5420.)";
//  B_MassWin   = "(B0_LOKI_MASS_JpsiConstr>5170.)&&(B0_LOKI_MASS_JpsiConstr<5420.)";
  B           = B_VtxChi2&&B_IPChi2&&B_NextIPChi2&&B_DTFChi2&&B_MassWin;
  B.Print();
  
  TCut StringCut = Mu&&Pi&&Jpsi&&KS&&B;
  StringCut.Print();
  return StringCut;
}

/*
 * DerivedReducer for KS with veto mass calculation.
 */
class KSDerivedReducer : public BkgCategorizerReducer {
  
  ReducerLeaf<Float_t>* ppi_mass_;
  ReducerLeaf<Float_t>* pip_mass_;
  ReducerLeaf<Int_t>* lambda_veto_;
  
  Float_t* ppi_mass_value_;
  Float_t* pip_mass_value_;
  Int_t*   lambda_veto_value_;
  
  Float_t* piplus_PE_;
  Float_t* piplus_PX_;
  Float_t* piplus_PY_;
  Float_t* piplus_PZ_;
  Float_t* piplus_P_;
  Float_t* piminus_PE_;
  Float_t* piminus_PX_;
  Float_t* piminus_PY_;
  Float_t* piminus_PZ_;
  Float_t* piminus_P_;
  
  virtual void CreateSpecialBranches() {
    piplus_PE_  = (Float_t*)GetInterimLeafByName("piplus_PE").branch_address();
    piplus_PX_  = (Float_t*)GetInterimLeafByName("piplus_PX").branch_address();
    piplus_PY_  = (Float_t*)GetInterimLeafByName("piplus_PY").branch_address();
    piplus_PZ_  = (Float_t*)GetInterimLeafByName("piplus_PZ").branch_address();
    piplus_P_   = (Float_t*)GetInterimLeafByName("piplus_P").branch_address();
    piminus_PE_ = (Float_t*)GetInterimLeafByName("piminus_PE").branch_address();
    piminus_PX_ = (Float_t*)GetInterimLeafByName("piminus_PX").branch_address();
    piminus_PY_ = (Float_t*)GetInterimLeafByName("piminus_PY").branch_address();
    piminus_PZ_ = (Float_t*)GetInterimLeafByName("piminus_PZ").branch_address();
    piminus_P_  = (Float_t*)GetInterimLeafByName("piminus_P").branch_address();
        
    ppi_mass_    = &(CreateFloatLeaf("ppi_M", "ppi_M", "Float_t"));
    pip_mass_    = &(CreateFloatLeaf("pip_M", "pip_M", "Float_t"));
    lambda_veto_ = &(CreateIntLeaf("lambda_veto", "lambda_veto", "Int_t"));
    
    ppi_mass_value_    = (Float_t*)ppi_mass_->branch_address();
    pip_mass_value_    = (Float_t*)pip_mass_->branch_address();
    lambda_veto_value_ = (Int_t*)lambda_veto_->branch_address();
  }
  
  bool EntryPassesSpecialCuts() { 
    //return !
    // *lambda_veto_value_ = VetoMass(1115.68, // 1115.683 => 1111.68..1119.68
    //                                938.27, // 938.272
    //                                4.0,
    //                                *piplus_PE_,  *piplus_P_,  *piplus_PX_,  *piplus_PY_,  *piplus_PZ_,
    //                                *piminus_PE_, *piminus_P_, *piminus_PX_, *piminus_PY_, *piminus_PZ_,
    //                                ppi_mass_value_, pip_mass_value_);
    //std::cout << "ppi = " << *ppi_mass_value_ << ", pip = " << *pip_mass_value_ << std::endl;
    //return !(*lambda_veto_value_);
    
    return true;
  }
  
  virtual void UpdateSpecialLeaves() {
    BkgCategorizerReducer::UpdateSpecialLeaves();
    // is empty because EntryPassesSpecialCuts() already updates the values.
  }
};

int ReduceKS(TString data_type, TString input_file, TString input_tree, TString interim_tree, TString output_file, TString output_tree){
  bool is_MC = false;
  if (data_type == "MC"){
    is_MC = true;
  } 
  
  KSDerivedReducer AReducer;
  
  AReducer.set_input_file_path(input_file);
  AReducer.set_input_tree_path(input_tree);
  AReducer.set_interim_file_path(interim_tree);
  AReducer.set_output_file_path(output_file);
  AReducer.set_output_tree_path(output_tree);
 
  AReducer.set_cut_string(KSTreeCuts().GetTitle());

  AReducer.Initialize();
  
  if (AReducer.LeafExists("B0_DECAYDEPTH")) {
    AReducer.set_decay_matrix_name("B0_DECAYMTRX");
    AReducer.set_decay_depth_leaf<Float_t>(AReducer.GetInterimLeafByName("B0_DECAYDEPTH"));
    AReducer.set_decay_products_leaf<Float_t>(AReducer.GetInterimLeafByName("B0_DECAYPRODUCTS"));
    ReducerLeaf<Int_t>& bkgcat_leaf = AReducer.CreateIntLeaf("bkgCategorizedDecays");
    AReducer.set_background_category_leaf(bkgcat_leaf);    
  }
  
  // TODO: add Reducer member function to add leaves
  const ReducerLeaf<Float_t>& vchi2_leaf = AReducer.GetInterimLeafByName("B0_LOKI_DTF_CHI2NDOF"); 
  AReducer.SetBestCandidateLeaf<Float_t>(vchi2_leaf);
  
  ReducerLeaf<Float_t>& tau_leaf     = AReducer.CreateFloatCopyLeaf("varProptime", AReducer.GetInterimLeafByName("B0_LOKI_DTF_CTAU"), 1.0/0.299792458);
  ReducerLeaf<Float_t>& tau_err_leaf = AReducer.CreateFloatCopyLeaf("varProptimeErr", AReducer.GetInterimLeafByName("B0_LOKI_DTF_CTAUERR"), 1.0/0.299792458);
  
  // the reason, we are getting event and run number as float leaves is simple:
  // by definition all leaves from the interim tree are ReducerLeaf<Float_t>
  // even if they are stored as int in the tree. Nevertheless, they are treated
  // as an int internally in the Reducer.
  AReducer.SetEventNumberLeaf<Float_t>(AReducer.GetInterimLeafByName("eventNumber"));
  AReducer.SetRunNumberLeaf<Float_t>(AReducer.GetInterimLeafByName("runNumber"));
  
  // unbiased trigger
  TCut Trigger_unbiased	    = "B0Hlt1DiMuonHighMassDecision_TOS == 1 && B0Hlt2DiMuonJPsiDecision_TOS == 1";
  ReducerLeaf<Int_t>& unbiased_leaf = AReducer.CreateIntLeaf("triggerUnbiased");
  unbiased_leaf.AddCondition("UnbiasedTrigger",Trigger_unbiased.GetTitle(),1);
  unbiased_leaf.AddCondition("NotUnbiasedTrigger",(!Trigger_unbiased).GetTitle(),0);
  
  // biased trigger
  TCut Trigger_biased = "B0Hlt1DiMuonHighMassDecision_TOS == 1 && B0Hlt2DiMuonDetachedJPsiDecision_TOS == 1";
  ReducerLeaf<Int_t>& biased_leaf = AReducer.CreateIntLeaf("triggerBiased");
  biased_leaf.AddCondition("BiasedTrigger",Trigger_biased.GetTitle(),1);
  biased_leaf.AddCondition("NotBiasedTrigger",(!Trigger_biased).GetTitle(),0);

  // biased trigger
  TCut Trigger_biasedHlt1And2 = "B0Hlt1TrackMuonDecision_TOS == 1 && B0Hlt2DiMuonDetachedJPsiDecision_TOS == 1";
  ReducerLeaf<Int_t>& biased_leaf_Hlt1And2 = AReducer.CreateIntLeaf("triggerBiasedHlt1And2");
  biased_leaf_Hlt1And2.AddCondition("BiasedTriggerHlt1And2",Trigger_biasedHlt1And2.GetTitle(),1);
  biased_leaf_Hlt1And2.AddCondition("NotBiasedTriggerHlt1And2",(!Trigger_biasedHlt1And2).GetTitle(),0);
  
  
  ReducerLeaf<Int_t>& eventtype_leaf = AReducer.CreateIntLeaf("decaytype");
  eventtype_leaf.AddCondition("DD","piminus_TRACK_Type==5",31);
  eventtype_leaf.AddCondition("LL","piminus_TRACK_Type==3",30);
  
  ReducerLeaf<Int_t>& cat_tagged_leaf = AReducer.CreateIntLeaf("catTagged", -1); 
  cat_tagged_leaf.AddCondition("Tagged",   "B0_TAGDECISION != 0", 1);
  cat_tagged_leaf.AddCondition("Untagged", "B0_TAGDECISION == 0", 0);
  
  ReducerLeaf<Int_t>& cat_tagged_OS_leaf = AReducer.CreateIntLeaf("catTagged_OS", -1); 
  cat_tagged_OS_leaf.AddCondition("Tagged",   "B0_TAGDECISION_OS != 0", 1);
  cat_tagged_OS_leaf.AddCondition("Untagged", "B0_TAGDECISION_OS == 0", 0);
  
  // create flipped tagging variable
  ReducerLeaf<Int_t>& vtag_leaf    = AReducer.CreateIntLeaf("varTag", 0); 
  vtag_leaf.AddCondition("B0",    "B0_TAGDECISION == +1", -1);
  vtag_leaf.AddCondition("B0bar", "B0_TAGDECISION == -1", +1);
  ReducerLeaf<Int_t>& vtag_OS_leaf    = AReducer.CreateIntLeaf("varTag_OS", 0); 
  vtag_OS_leaf.AddCondition("B0",    "B0_TAGDECISION_OS == +1", -1);
  vtag_OS_leaf.AddCondition("B0bar", "B0_TAGDECISION_OS == -1", +1);
  
  
  // using default values for (conditional) leaves (last parameter)
  ReducerLeaf<Int_t>& sel_leaf = AReducer.CreateIntLeaf("sel", 1);
  
  // by definition in https://twiki.cern.ch/twiki/bin/view/LHCb/BetasWGFitDataFormat
  // this leaf is *always* 1
  ReducerLeaf<Int_t>& hasBestVtxChi2_leaf = AReducer.CreateIntLeaf("hasBestVtxChi2", 1);
  
  // copying branches for Dortmund's fitter
  ReducerLeaf<Float_t>& vm_leaf    = AReducer.CreateFloatCopyLeaf("varMass",      AReducer.GetInterimLeafByName("B0_LOKI_MASS_JpsiKSConstr"));
  ReducerLeaf<Float_t>& vmd1_leaf  = AReducer.CreateFloatCopyLeaf("varMassDau1",  AReducer.GetInterimLeafByName("J_psi_1S_MM"));
  ReducerLeaf<Float_t>& vomg_leaf  = AReducer.CreateFloatCopyLeaf("varOmega",     AReducer.GetInterimLeafByName("B0_TAGOMEGA"));
    
  
  // copying branches for https://twiki.cern.ch/twiki/bin/view/LHCb/BetasWGFitDataFormat  
  ReducerLeaf<Int_t>&   pid_leaf   = AReducer.CreateIntCopyLeaf(  "pid",          AReducer.GetInterimLeafByName("B0_ID"));
  ReducerLeaf<Int_t>&   pidd1_leaf = AReducer.CreateIntCopyLeaf(  "piddau1",      AReducer.GetInterimLeafByName("J_psi_1S_ID"));
  ReducerLeaf<Int_t>&   pidd2_leaf = AReducer.CreateIntCopyLeaf(  "piddau2",      AReducer.GetInterimLeafByName("KS0_ID"));
  
  ReducerLeaf<Int_t>&   evtnr_leaf = AReducer.CreateIntCopyLeaf(  "evtnr",        AReducer.GetInterimLeafByName("eventNumber"));
  ReducerLeaf<Int_t>&   runnr_leaf = AReducer.CreateIntCopyLeaf(  "runnr",        AReducer.GetInterimLeafByName("runNumber")); 
  
  // mass
  ReducerLeaf<Float_t>& m_leaf     = AReducer.CreateFloatCopyLeaf("m",            AReducer.GetInterimLeafByName("B0_LOKI_MASS_JpsiKSConstr"));
  ReducerLeaf<Float_t>& sm_leaf    = AReducer.CreateFloatCopyLeaf("sigmam",       AReducer.GetInterimLeafByName("B0_LOKI_MASSERR_JpsiKSConstr"));
  ReducerLeaf<Float_t>& m_jpsionly_leaf = AReducer.CreateFloatCopyLeaf("m_jpsiconsonly", AReducer.GetInterimLeafByName("B0_LOKI_MASS_JpsiConstr"));
  ReducerLeaf<Float_t>& merr_jpsionly_leaf = AReducer.CreateFloatCopyLeaf("sigmam_jpsiconsonly", AReducer.GetInterimLeafByName("B0_LOKI_MASSERR_JpsiConstr"));
  ReducerLeaf<Float_t>& md1_leaf   = AReducer.CreateFloatCopyLeaf("mdau1",        AReducer.GetInterimLeafByName("J_psi_1S_MM"));
  ReducerLeaf<Float_t>& md2_leaf   = AReducer.CreateFloatCopyLeaf("mdau2",        AReducer.GetInterimLeafByName("KS0_MM"));
  ReducerLeaf<Float_t>& smd1_leaf  = AReducer.CreateFloatCopyLeaf("sigmamdau1",   AReducer.GetInterimLeafByName("J_psi_1S_MMERR"));
  ReducerLeaf<Float_t>& smd2_leaf  = AReducer.CreateFloatCopyLeaf("sigmamdau2",   AReducer.GetInterimLeafByName("KS0_MMERR"));
  
  // proper decay time
  ReducerLeaf<Float_t>& t_leaf     = AReducer.CreateFloatCopyLeaf("t",            tau_leaf);
  ReducerLeaf<Float_t>& terr_leaf  = AReducer.CreateFloatCopyLeaf("sigmat",       tau_err_leaf);
  ReducerLeaf<Float_t>& chi2_leaf  = AReducer.CreateFloatCopyLeaf("chi2dof",      vchi2_leaf);
  
  // tagging
  ReducerLeaf<Int_t>&   tag_leaf       = AReducer.CreateIntCopyLeaf(  "tagdecision",  AReducer.GetInterimLeafByName("B0_TAGDECISION"));
  ReducerLeaf<Int_t>&   tagc_leaf      = AReducer.CreateIntCopyLeaf(  "tagcat",       AReducer.GetInterimLeafByName("B0_TAGCAT"));
  ReducerLeaf<Float_t>& tago_leaf      = AReducer.CreateFloatCopyLeaf("tagomega",     AReducer.GetInterimLeafByName("B0_TAGOMEGA"));
  ReducerLeaf<Int_t>&   tag_OS_leaf    = AReducer.CreateIntCopyLeaf(  "tagdecision_OS",  AReducer.GetInterimLeafByName("B0_TAGDECISION_OS"));
  ReducerLeaf<Int_t>&   tagc_OS_leaf   = AReducer.CreateIntCopyLeaf(  "tagcat_OS",       AReducer.GetInterimLeafByName("B0_TAGCAT_OS"));
  ReducerLeaf<Float_t>& tago_OS_leaf   = AReducer.CreateFloatCopyLeaf("tagomega_OS",     AReducer.GetInterimLeafByName("B0_TAGOMEGA_OS"));
  ReducerLeaf<Int_t>&   tag_SSPi_leaf  = AReducer.CreateIntCopyLeaf(  "tagdecision_SSPi",  AReducer.GetInterimLeafByName("B0_SS_Pion_DEC"));
  ReducerLeaf<Float_t>& tago_SSPi_leaf = AReducer.CreateFloatCopyLeaf("tagomega_SSPi",     AReducer.GetInterimLeafByName("B0_SS_Pion_PROB"));
  
  
  // MC specific
  if (is_MC) {
    // proptime in ps
    ReducerLeaf<Float_t>& tau_true_leaf     = AReducer.CreateFloatCopyLeaf("varProptimeTrue", AReducer.GetInterimLeafByName("B0_TRUETAU"), 1000.);
    ReducerLeaf<Float_t>& t_true_leaf       = AReducer.CreateFloatCopyLeaf("t_true", tau_true_leaf);    
    // true tag
    ReducerLeaf<Int_t>& vtag_true_leaf    = AReducer.CreateIntLeaf("varTag_True", 0); 
    vtag_true_leaf.AddCondition("B0",    "B0_TRUEID == +511", -1);
    vtag_true_leaf.AddCondition("B0bar", "B0_TRUEID == -511", +1);
    
    ReducerLeaf<Int_t>& tag_true_leaf    = AReducer.CreateIntLeaf("tagdecision_true", 0); 
    tag_true_leaf.AddCondition("B0",    "B0_TRUEID == +511", +1);
    tag_true_leaf.AddCondition("B0bar", "B0_TRUEID == -511", -1);
    
    // bkgcat
    ReducerLeaf<Int_t>&   tag_leaf       = AReducer.CreateIntCopyLeaf(  "bkgcat",  AReducer.GetInterimLeafByName("B0_BKGCAT"));
  
    // for Timon's DTF tests
    ReducerLeaf<Float_t>& ts_truetau     = AReducer.CreateFloatCopyLeaf("ts_truetau", AReducer.GetInterimLeafByName("B0_TRUETAU"), 1000.);
    
    ReducerLeaf<Float_t>& ts_dtf_tau     = AReducer.CreateFloatCopyLeaf("ts_dtf_tau", AReducer.GetInterimLeafByName("B0_LOKI_DTF_CTAU"), 1.0/0.299792458);
    ReducerLeaf<Float_t>& ts_dtf_tauerr  = AReducer.CreateFloatCopyLeaf("ts_dtf_tauerr", AReducer.GetInterimLeafByName("B0_LOKI_DTF_CTAUERR"), 1.0/0.299792458);
    
    ReducerLeaf<Float_t>& ts_ovf_tau     = AReducer.CreateFloatCopyLeaf("ts_ovf_tau", AReducer.GetInterimLeafByName("B0_TAU"), 1000.);
    ReducerLeaf<Float_t>& ts_ovf_tauerr  = AReducer.CreateFloatCopyLeaf("ts_ovf_tauerr", AReducer.GetInterimLeafByName("B0_TAUERR"), 1000.);
    
    ReducerLeaf<Float_t>& ts_dtf_ttrue_resid   = AReducer.CreateFloatLeaf("ts_dtf_ttrue_resid", -99999999.);
    ts_dtf_ttrue_resid.Add(AReducer.GetInterimLeafByName("B0_LOKI_DTF_CTAU"), AReducer.GetInterimLeafByName("B0_TRUETAU"),1./0.299792458,-1000.);
    
    ReducerLeaf<Float_t>& ts_ovf_ttrue_resid   = AReducer.CreateFloatLeaf("ts_ovf_ttrue_resid", -99999999.);
    ts_ovf_ttrue_resid.Add(AReducer.GetInterimLeafByName("B0_TAU"), AReducer.GetInterimLeafByName("B0_TRUETAU"),1000.,-1000.);
    
    ReducerLeaf<Float_t>& ts_dtf_ovf_resid   = AReducer.CreateFloatLeaf("ts_dtf_ovf_resid", -99999999.);
    ts_dtf_ovf_resid.Add(AReducer.GetInterimLeafByName("B0_LOKI_DTF_CTAU"), AReducer.GetInterimLeafByName("B0_TAU"),1./0.299792458,-1000.);
  }

  
  
  AReducer.Run();
  
  AReducer.Finalize();
  
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc != 7){
    std::cout << "Parameters needed: Type InputFile InputTree InterimFile OutputFile OutTreeName" << std::endl;
    std::cout << "Type can be: MC/DATA" << std::endl;
    return 1;
  }
  else{
    //return ReduceKS("RootTest/Test.root", "Bd/DecayTree", "RootTest/InterimTest.root", "RootTest/OutputTest.root", "Bd2JpsiKS");
    return ReduceKS(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
  }

}
