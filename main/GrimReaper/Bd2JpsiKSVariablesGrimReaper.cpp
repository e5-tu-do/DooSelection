/******************************************/
// Bd2JpsiKSVariablesGrimReaper.cpp
//
// Standalone GrimReaper that adds a lot of
// common variables to trees.
//
// Author: Christophe Cauet
// Date: 2014-02-04
/******************************************/

// from STL
#include <tuple>
#include <list>

// from ROOT
#include "TRandom3.h"
#include "TCut.h"

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/config/Summary.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"
#include "dooselection/reducer/ReducerLeaf.h"
#include "dooselection/reducer/KinematicReducerLeaf.h"

using namespace dooselection::reducer;
using namespace doocore::io;

// forward declarations
// typedef tuple: head, daughters, stable particles, isMC, isFlat
typedef std::tuple<std::string, std::list<std::string>, std::list<std::string>, bool, bool> cfg_tuple;
cfg_tuple Configure(Reducer* _rdcr, std::string& _channel);
void MassLeaves(Reducer* _rdcr, cfg_tuple& cfg);
void TimeLeaves(Reducer* _rdcr, cfg_tuple& cfg);
void TriggerLeaves(Reducer* _rdcr, cfg_tuple& cfg);
void VetoLeaves(Reducer* _rdcr, cfg_tuple& cfg);
void AuxiliaryLeaves(Reducer* _rdcr, cfg_tuple& cfg);

int main(int argc, char * argv[]){
  sinfo << "-info-  \t" << "Bd2JpsiKSVariablesGrimReaper \t" << "Welcome!" << endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, decay_channel;
  if (argc == 5){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    decay_channel = "Bd2JpsiKS";
  }
  else{
    serr << "-ERROR- \t" << "Bd2JpsiKSVariablesGrimReaper \t" << "Parameters needed:" << endmsg;
    serr << "-ERROR- \t" << "Bd2JpsiKSVariablesGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name" << endmsg;
    return 1;
  }

  Reducer* reducer = new Reducer();
  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  summary.AddSection("I/O");
  summary.Add("Input file", inputfile);
  summary.Add("Input tree", inputtree);
  summary.Add("Output file", outputfile);
  summary.Add("Output file", outputtree);

  // reducer part
  reducer->set_input_file_path(inputfile);
  reducer->set_input_tree_path(inputtree);
  reducer->set_output_file_path(outputfile);
  reducer->set_output_tree_path(outputtree);

  reducer->Initialize();

  // config
  cfg_tuple cfg = Configure(reducer, decay_channel);

  // add leaves  
  summary.AddSection("Added leaves");
  MassLeaves(reducer, cfg);
  TimeLeaves(reducer, cfg);
  TriggerLeaves(reducer, cfg);
  VetoLeaves(reducer, cfg);
  AuxiliaryLeaves(reducer, cfg);

  reducer->Run();
  reducer->Finalize();
}

cfg_tuple Configure(Reducer* _rdcr, std::string& _channel){
  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  summary.AddSection("Channel");
  // typedef tuple: head, daughters, stable particles, isMC, isFlat
  std::string head ="";
  std::list<std::string> daughters;
  std::list<std::string> stable_particles;
  bool isMC = false;
  bool isFlat = false;
  if (_channel == "Bd2JpsiKS"){
    head = "B0";
    daughters.push_back("J_psi_1S");
    daughters.push_back("KS0");
    stable_particles.push_back("muminus");
    stable_particles.push_back("muplus");
    stable_particles.push_back("piminus");
    stable_particles.push_back("piplus");
    isMC = _rdcr->LeafExists(head+"_BKGCAT");
    isFlat = (_rdcr->LeafExists("flat_array_index") || _rdcr->LeafExists("idxPV"));
  }
  else{
    serr << "-ERROR- \t" << "Bd2JpsiKSVariablesGrimReaper \t" << "No valid decay channel. Possible decay channels are:" << endmsg;
    serr << "-ERROR- \t" << "Bd2JpsiKSVariablesGrimReaper \t" << "- Bd2JspiKS" << endmsg;
  }
  summary.Add("Name", _channel);
  summary.Add("Head", head);
  for (std::list<std::string>::iterator it = daughters.begin(); it != daughters.end(); ++it){
    summary.Add("Daughter", *it);
  }
  for (std::list<std::string>::iterator it = stable_particles.begin(); it != stable_particles.end(); ++it){
    summary.Add("Stable", *it);
  }
  summary.AddSection("Data Type");
  summary.Add("MC", isMC);
  summary.Add("Flat", isFlat);

  if (isFlat) sinfo << "-info-  \t" << "You are running the reducer over a flat tuple!" << endmsg;
  if (isMC) sinfo << "-info-  \t" << "You are running the reducer over a MC tuple!" << endmsg;

  return std::make_tuple(head, daughters, stable_particles, isMC, isFlat);
}

void MassLeaves(Reducer* _rdcr, cfg_tuple& cfg){
  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  // handle flattened tuples
  std::string flat_suffix = "";
  if (std::get<4>(cfg)) flat_suffix = "_flat";
  sinfo << "-info-  \t" << "Setting flat suffix to " << flat_suffix << endmsg;

  // create copies of mass observables for different fit constraints
  // plus the 'nominal' mass observable depending on the following hierarchy
  std::string main_observable_constraint = "";
  std::string main_observable_constraint_error = "";
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitDaughtersPVConst_M"+flat_suffix)) {
    if (main_observable_constraint == "") main_observable_constraint = "FitDaughtersPVConst_M";
    if (main_observable_constraint_error == "") main_observable_constraint_error = "FitDaughtersPVConst_MERR";
    ReducerLeaf<Double_t>& mass_jpsi_ks_pv_leaf     = _rdcr->CreateDoubleCopyLeaf("obsMassDaughtersPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_M"+flat_suffix));
    ReducerLeaf<Double_t>& mass_err_jpsi_ks_pv_leaf = _rdcr->CreateDoubleCopyLeaf("obsMassErrDaughtersPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_MERR"+flat_suffix));
  }
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitJpsiPVConst_M"+flat_suffix)) {
    if (main_observable_constraint == "") main_observable_constraint = "FitJpsiPVConst_M";
    if (main_observable_constraint_error == "") main_observable_constraint_error = "FitJpsiPVConst_MERR";
    ReducerLeaf<Double_t>& mass_ks_pv_leaf     = _rdcr->CreateDoubleCopyLeaf("obsMassJpsiPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitJpsiPVConst_M"+flat_suffix));
    ReducerLeaf<Double_t>& mass_err_ks_pv_leaf = _rdcr->CreateDoubleCopyLeaf("obsMassErrJpsiPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitJpsiPVConst_MERR"+flat_suffix));
  } 
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_M"+flat_suffix)) {
    if (main_observable_constraint == "") main_observable_constraint = "FitPVConst_M";
    if (main_observable_constraint_error == "") main_observable_constraint_error = "FitPVConst_MERR";
    ReducerLeaf<Double_t>& mass_pv_leaf     = _rdcr->CreateDoubleCopyLeaf("obsMassPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_M"+flat_suffix));
    ReducerLeaf<Double_t>& mass_err_pv_leaf = _rdcr->CreateDoubleCopyLeaf("obsMassErrPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_MERR"+flat_suffix));
  } 
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_LOKI_MASS_JpsiKSConstr")) {
    if (main_observable_constraint == "") main_observable_constraint = "LOKI_MASS_JpsiKSConstr";
    if (main_observable_constraint_error == "") main_observable_constraint_error = "LOKI_MASSERR_JpsiKSConstr";
    ReducerLeaf<Double_t>& mass_loki_jpsi_ks_pv_leaf     = _rdcr->CreateDoubleCopyLeaf("obsMassLokiDaughtersPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_LOKI_MASS_JpsiKSConstr"));
    ReducerLeaf<Double_t>& mass_err_loki_jpsi_ks_pv_leaf = _rdcr->CreateDoubleCopyLeaf("obsMassErrLokiDaughtersPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_LOKI_MASSERR_JpsiKSConstr"));
  } 
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_LOKI_MASS_JpsiConstr")) {
    if (main_observable_constraint == "") main_observable_constraint = "LOKI_MASS_JpsiConstr";
    if (main_observable_constraint_error == "") main_observable_constraint_error = "LOKI_MASSERR_JpsiConstr";
    ReducerLeaf<Double_t>& mass_loki_jpsi_pv_leaf     = _rdcr->CreateDoubleCopyLeaf("obsMassLokiJpsiPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_LOKI_MASS_JpsiConstr"));
    ReducerLeaf<Double_t>& mass_err_loki_jpsi_pv_leaf = _rdcr->CreateDoubleCopyLeaf("obsMassErrLokiJpsiPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_LOKI_MASSERR_JpsiConstr"));
  }
  ReducerLeaf<Double_t>& mass_jpsi_ks_pv_leaf     = _rdcr->CreateDoubleCopyLeaf("obsMass", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_"+main_observable_constraint+flat_suffix));
  ReducerLeaf<Double_t>& mass_err_jpsi_ks_pv_leaf = _rdcr->CreateDoubleCopyLeaf("obsMassErr", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_"+main_observable_constraint_error+flat_suffix));
  summary.Add("obsMass fit constraints", main_observable_constraint);

  // old
  std::string fit_constraints;
  ReducerLeaf<Double_t>* jpsi_mass_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* ks0_mass_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* jpsi_mass_err_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* ks0_mass_err_leaf_ptr = NULL;

  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitDaughtersPVConst_M")) {
    fit_constraints = "PVJpsiKSConst";
    jpsi_mass_leaf_ptr     = &_rdcr->CreateDoubleCopyLeaf("obsMassDauOne", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_J_psi_1S_M"+flat_suffix));
    jpsi_mass_err_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsMassErrDauOne", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_J_psi_1S_MERR"+flat_suffix));
    if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_M") && _rdcr->LeafExists(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_MERR")) {
      ks0_mass_leaf_ptr     = &_rdcr->CreateDoubleCopyLeaf("obsMassDauTwo", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_M"+flat_suffix));
      ks0_mass_err_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsMassErrDauTwo", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_MERR"+flat_suffix));
    }
  } else if (_rdcr->LeafExists(std::get<0>(cfg)+"_LOKI_MASS_JpsiKSConstr")) {
    fit_constraints = "JpsiKSConst";
    jpsi_mass_leaf_ptr     = &_rdcr->CreateDoubleCopyLeaf("obsMassDauOne", _rdcr->GetInterimLeafByName("J_psi_1S_MM"));
    jpsi_mass_err_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsMassErrDauOne", _rdcr->GetInterimLeafByName("J_psi_1S_MMERR"));
    if (_rdcr->LeafExists("KS0_MM")) {
      ks0_mass_leaf_ptr      = &_rdcr->CreateDoubleCopyLeaf("obsMassDauTwo", _rdcr->GetInterimLeafByName("KS0_MM"));
      jpsi_mass_err_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsMassErrDauTwo", _rdcr->GetInterimLeafByName("KS0_MMERR"));
    }
  } else if (_rdcr->LeafExists(std::get<0>(cfg)+"_LOKI_MASS_JpsiConstr")) {
    fit_constraints = "JpsiConst";
    jpsi_mass_leaf_ptr     = &_rdcr->CreateDoubleCopyLeaf("obsMassDauOne", _rdcr->GetInterimLeafByName("J_psi_1S_MM"));
    jpsi_mass_err_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsMassErrDauOne", _rdcr->GetInterimLeafByName("J_psi_1S_MMERR"));
    if (_rdcr->LeafExists("KS0_MM")) {
      ks0_mass_leaf_ptr      = &_rdcr->CreateDoubleCopyLeaf("obsMassDauTwo", _rdcr->GetInterimLeafByName("KS0_MM"));
      jpsi_mass_err_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsMassErrDauTwo", _rdcr->GetInterimLeafByName("KS0_MMERR"));
    }
  }
}

void TimeLeaves(Reducer* _rdcr, cfg_tuple& cfg){
  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  // handle flattened tuples
  std::string flat_suffix = "";
  std::string fit_constraints = "";
  if (std::get<4>(cfg)) flat_suffix = "_flat";

  ReducerLeaf<Double_t>* tau_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* tau_true_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* tau_err_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* tau_true_err_leaf_ptr = NULL;

  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_tau")) {
    fit_constraints = "PVConst";
    tau_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsTime", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_tau"+flat_suffix), 1000.0);
    tau_err_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsTimeErr", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_tauErr"+flat_suffix), 1000.0);
    if(_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_KS0_tau")){
      _rdcr->CreateDoubleLeaf("varKS0TauSignificance", -99999999.).Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_KS0_tau"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_KS0_tauErr"+flat_suffix));
    }
  }
  else if (_rdcr->LeafExists(std::get<0>(cfg)+"_LOKI_DTF_CTAU")){
    fit_constraints = "LOKI";
    tau_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsTime", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_LOKI_DTF_CTAU"), 1.0/0.299792458);
    tau_err_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsTimeErr", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_LOKI_DTF_CTAUERR"), 1.0/0.299792458);
  }

  if (_rdcr->LeafExists(std::get<0>(cfg)+"_BKGCAT")){
    tau_true_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsTime_True", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_TRUETAU"), 1000.0);
    
    // the "true" time error is defined as "reconstructed decay time" - "true decay time"
    tau_true_err_leaf_ptr = &_rdcr->CreateDoubleLeaf("obsTimeErr_True", 1000.0);
    if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_tau")) {
      tau_true_err_leaf_ptr->Add(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_tau"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_TRUETAU"), 1.0, -1.0);
    }
    else if (_rdcr->LeafExists(std::get<0>(cfg)+"_LOKI_DTF_CTAU")){
      tau_true_err_leaf_ptr->Add(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_LOKI_DTF_CTAU"), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_TRUETAU"), 1.0/0.299792458, -1.0);      
    }
  }

  summary.Add("Time fit constraints", fit_constraints);
}

void TriggerLeaves(Reducer* _rdcr, cfg_tuple& cfg){
  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  // trigger categories
  ReducerLeaf<Int_t>& trigger_l0_global_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerL0GlobalTOS", _rdcr->GetInterimLeafByName("J_psi_1S_L0Global_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt1_global_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerHlt1GlobalTOS", _rdcr->GetInterimLeafByName("J_psi_1S_Hlt1Global_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt2_global_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerHlt2GlobalTOS", _rdcr->GetInterimLeafByName("J_psi_1S_Hlt2Global_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt1_trackmuon_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerHlt1TrackMuonTOS", _rdcr->GetInterimLeafByName("J_psi_1S_Hlt1TrackMuonDecision_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt1_highmass_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerHlt1HighMassTOS", _rdcr->GetInterimLeafByName("J_psi_1S_Hlt1DiMuonHighMassDecision_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt2_detachedjpsi_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerHlt2DetachedJpsiTOS", _rdcr->GetInterimLeafByName("J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt2_jpsi_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerHlt2JpsiTOS", _rdcr->GetInterimLeafByName("J_psi_1S_Hlt2DiMuonJPsiDecision_TOS"));

  ReducerLeaf<Int_t>& catTriggerSetAlmostUnbiased = _rdcr->CreateIntLeaf("catTriggerSetAlmostUnbiased", -10);
      TCut trigger_set_almost_unbiased = "J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS==1&&J_psi_1S_Hlt1DiMuonHighMassDecision_TOS==1";
      TCut trigger_set_almost_unbiased_and_jpsi = "J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS==1&&J_psi_1S_Hlt1DiMuonHighMassDecision_TOS==1&&J_psi_1S_Hlt2DiMuonJPsiDecision_TOS==1";
      catTriggerSetAlmostUnbiased.AddCondition("triggered", trigger_set_almost_unbiased.GetTitle(), 1);
      catTriggerSetAlmostUnbiased.AddCondition("not_triggered", (!trigger_set_almost_unbiased).GetTitle(), 0);

  ReducerLeaf<Int_t>& catTriggerSetExclBiased = _rdcr->CreateIntLeaf("catTriggerSetExclBiased", -10);
      TCut trigger_set_excl_biased = "J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS==1&&J_psi_1S_Hlt1TrackMuonDecision_TOS==1&&J_psi_1S_Hlt1DiMuonHighMassDecision_TOS==0";
      catTriggerSetExclBiased.AddCondition("triggered", trigger_set_excl_biased.GetTitle(), 1);
      catTriggerSetExclBiased.AddCondition("not_triggered", (!trigger_set_excl_biased).GetTitle(), 0);

  ReducerLeaf<Int_t>& catTriggerHlt1TrackMuonAndHlt2DetachedJpsi = _rdcr->CreateIntLeaf("catTriggerHlt1TrackMuonAndHlt2DetachedJpsi", -10);
      TCut trigger_hlt1trackmuon_hlt2detached = "J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS==1&&J_psi_1S_Hlt1TrackMuonDecision_TOS==1";
      catTriggerHlt1TrackMuonAndHlt2DetachedJpsi.AddCondition("triggered", trigger_hlt1trackmuon_hlt2detached.GetTitle(), 1);
      catTriggerHlt1TrackMuonAndHlt2DetachedJpsi.AddCondition("not_triggered", (!trigger_hlt1trackmuon_hlt2detached).GetTitle(), 0);

  ReducerLeaf<Int_t>& catTriggerHlt1HighMassAndHlt2Jpsi = _rdcr->CreateIntLeaf("catTriggerHlt1HighMassAndHlt2Jpsi", -10);
      TCut trigger_hlt1highmass_hlt2jpsi = "J_psi_1S_Hlt1DiMuonHighMassDecision_TOS==1&&J_psi_1S_Hlt2DiMuonJPsiDecision_TOS==1";
      catTriggerHlt1HighMassAndHlt2Jpsi.AddCondition("triggered", trigger_hlt1highmass_hlt2jpsi.GetTitle(), 1);
      catTriggerHlt1HighMassAndHlt2Jpsi.AddCondition("not_triggered", (!trigger_hlt1highmass_hlt2jpsi).GetTitle(), 0);

  ReducerLeaf<Int_t>& catTriggerEfficiencySet = _rdcr->CreateIntLeaf("catTriggerEfficiencySet", -10);
      TCut trigger_efficiency_set = "(J_psi_1S_Hlt1DiMuonHighMassDecision_TOS==1)||(J_psi_1S_Hlt1TrackMuonDecision_TOS==1)||(J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS==1)||(J_psi_1S_Hlt2DiMuonJPsiDecision_TOS==1)";
      catTriggerEfficiencySet.AddCondition("triggered", trigger_efficiency_set.GetTitle(), 1);
      catTriggerEfficiencySet.AddCondition("not_triggered", (!trigger_efficiency_set).GetTitle(), 0);

  ReducerLeaf<Int_t>& catTriggerSet = _rdcr->CreateIntLeaf("catTriggerSet", -10);
      TCut trigger_set = "((J_psi_1S_Hlt1DiMuonHighMassDecision_TOS==1)||(J_psi_1S_Hlt1TrackMuonDecision_TOS==1))&&(J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS==1)";
      catTriggerSet.AddCondition("triggered", trigger_set.GetTitle(), 1);
      catTriggerSet.AddCondition("not_triggered", (!trigger_set).GetTitle(), 0);

  ReducerLeaf<Int_t>& catTriggerEfficiency = _rdcr->CreateIntLeaf("catTriggerEfficiency", -10);
      catTriggerEfficiency.AddCondition("almost_unbiased", trigger_set_almost_unbiased_and_jpsi.GetTitle(), 0);
      catTriggerEfficiency.AddCondition("excl_biased", trigger_set_excl_biased.GetTitle(), 1);

  ReducerLeaf<Int_t>& catTrigger = _rdcr->CreateIntLeaf("catTrigger", -10);
      catTrigger.AddCondition("almost_unbiased", trigger_set_almost_unbiased.GetTitle(), 0);
      catTrigger.AddCondition("excl_biased", trigger_set_excl_biased.GetTitle(), 1);
  
  // Hlt2DiMuonJpsi prescale categories
  TCut hlt2_jpsi_prescale_tcks = "HLTTCK==7667767 || HLTTCK==7929912 || HLTTCK==7733303 || HLTTCK==7798839 || HLTTCK==10027074 || HLTTCK==10027075 || HLTTCK==10682436 || HLTTCK==10420293 || HLTTCK==11075654 || HLTTCK==10092610 || HLTTCK==10092611 || HLTTCK==10027076 || HLTTCK==10551365 || HLTTCK==11272262 || HLTTCK==11206726";
  ReducerLeaf<Int_t>& catTriggerJpsiPrescale = _rdcr->CreateIntLeaf("catTriggerJpsiPrescale", -1);
    catTriggerJpsiPrescale.AddCondition("prescale", hlt2_jpsi_prescale_tcks.GetTitle(), 1);
    catTriggerJpsiPrescale.AddCondition("no-prescale", (!hlt2_jpsi_prescale_tcks).GetTitle(), 0);
}

void VetoLeaves(Reducer* _rdcr, cfg_tuple& cfg){
  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  // handle flattened tuples
  std::string flat_suffix = "";
  if (std::get<4>(cfg)) flat_suffix = "_flat";

  // veto leafs
  std::string piplus_px, piplus_py, piplus_pz;
  std::string piminus_px, piminus_py, piminus_pz;
  std::string muplus_px, muplus_py, muplus_pz;
  std::string muminus_px, muminus_py, muminus_pz;

  std::string mass_hypo_constraints = "";
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitJpsiPVConst_KS0_P0_PX")){
    piplus_px  = std::get<0>(cfg)+"_FitJpsiPVConst_KS0_P0_PX"+flat_suffix;   
    piplus_py  = std::get<0>(cfg)+"_FitJpsiPVConst_KS0_P0_PY"+flat_suffix;   
    piplus_pz  = std::get<0>(cfg)+"_FitJpsiPVConst_KS0_P0_PZ"+flat_suffix;   
    piminus_px = std::get<0>(cfg)+"_FitJpsiPVConst_KS0_P1_PX"+flat_suffix;     
    piminus_py = std::get<0>(cfg)+"_FitJpsiPVConst_KS0_P1_PY"+flat_suffix;     
    piminus_pz = std::get<0>(cfg)+"_FitJpsiPVConst_KS0_P1_PZ"+flat_suffix;  
    muplus_px  = std::get<0>(cfg)+"_FitJpsiPVConst_J_psi_1S_P0_PX"+flat_suffix;   
    muplus_py  = std::get<0>(cfg)+"_FitJpsiPVConst_J_psi_1S_P0_PY"+flat_suffix;   
    muplus_pz  = std::get<0>(cfg)+"_FitJpsiPVConst_J_psi_1S_P0_PZ"+flat_suffix;   
    muminus_px = std::get<0>(cfg)+"_FitJpsiPVConst_J_psi_1S_P1_PX"+flat_suffix;     
    muminus_py = std::get<0>(cfg)+"_FitJpsiPVConst_J_psi_1S_P1_PY"+flat_suffix;     
    muminus_pz = std::get<0>(cfg)+"_FitJpsiPVConst_J_psi_1S_P1_PZ"+flat_suffix;
    mass_hypo_constraints = "JpsiPV";
  }
  else if (_rdcr->LeafExists("piplus_PX")){
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
    mass_hypo_constraints = "NoConstr";
  }
  
  if (mass_hypo_constraints!=""){
    // mass hypotheses
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_pipi = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_pipi", "varKS0MassHypo_pipi", "Double_t", NULL);
    varKS0MassHypo_pipi->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varKS0MassHypo_pipi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Jpsipipi = new KinematicReducerLeaf<Double_t>("varBMassHypo_Jpsipipi", "varBMassHypo_Jpsipipi", "Double_t", NULL);
    varBMassHypo_Jpsipipi->FixedMassDaughtersFourBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(muplus_px),
        _rdcr->GetInterimLeafByName(muplus_py),
        _rdcr->GetInterimLeafByName(muplus_pz),
        105.6583715,
        _rdcr->GetInterimLeafByName(muminus_px),
        _rdcr->GetInterimLeafByName(muminus_py),
        _rdcr->GetInterimLeafByName(muminus_pz),
        105.6583715);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_Jpsipipi);

    
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_piK = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_piK", "varKS0MassHypo_piK", "Double_t", NULL);
    varKS0MassHypo_piK->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varKS0MassHypo_piK);

    KinematicReducerLeaf<Double_t>* varBMassHypo_JpsipiK = new KinematicReducerLeaf<Double_t>("varBMassHypo_JpsipiK", "varBMassHypo_JpsipiK", "Double_t", NULL);
    varBMassHypo_JpsipiK->FixedMassDaughtersFourBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(muplus_px),
        _rdcr->GetInterimLeafByName(muplus_py),
        _rdcr->GetInterimLeafByName(muplus_pz),
        105.6583715,
        _rdcr->GetInterimLeafByName(muminus_px),
        _rdcr->GetInterimLeafByName(muminus_py),
        _rdcr->GetInterimLeafByName(muminus_pz),
        105.6583715);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_JpsipiK);
    
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_Kpi = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_Kpi", "varKS0MassHypo_Kpi", "Double_t", NULL);
    varKS0MassHypo_Kpi->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varKS0MassHypo_Kpi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_JpsiKpi = new KinematicReducerLeaf<Double_t>("varBMassHypo_JpsiKpi", "varBMassHypo_JpsiKpi", "Double_t", NULL);
    varBMassHypo_JpsiKpi->FixedMassDaughtersFourBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(muplus_px),
        _rdcr->GetInterimLeafByName(muplus_py),
        _rdcr->GetInterimLeafByName(muplus_pz),
        105.6583715,
        _rdcr->GetInterimLeafByName(muminus_px),
        _rdcr->GetInterimLeafByName(muminus_py),
        _rdcr->GetInterimLeafByName(muminus_pz),
        105.6583715);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_JpsiKpi);
    
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_pip = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_pip", "varKS0MassHypo_pip", "Double_t", NULL);
    varKS0MassHypo_pip->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        938.272046);
    _rdcr->RegisterDoubleLeaf(varKS0MassHypo_pip);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Jpsipip = new KinematicReducerLeaf<Double_t>("varBMassHypo_Jpsipip", "varBMassHypo_Jpsipip", "Double_t", NULL);
    varBMassHypo_Jpsipip->FixedMassDaughtersFourBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(muplus_px),
        _rdcr->GetInterimLeafByName(muplus_py),
        _rdcr->GetInterimLeafByName(muplus_pz),
        105.6583715,
        _rdcr->GetInterimLeafByName(muminus_px),
        _rdcr->GetInterimLeafByName(muminus_py),
        _rdcr->GetInterimLeafByName(muminus_pz),
        105.6583715);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_Jpsipip);
    
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_ppi = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_ppi", "varKS0MassHypo_ppi", "Double_t", NULL);
    varKS0MassHypo_ppi->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varKS0MassHypo_ppi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Jpsippi = new KinematicReducerLeaf<Double_t>("varBMassHypo_Jpsippi", "varBMassHypo_Jpsippi", "Double_t", NULL);
    varBMassHypo_Jpsippi->FixedMassDaughtersFourBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(muplus_px),
        _rdcr->GetInterimLeafByName(muplus_py),
        _rdcr->GetInterimLeafByName(muplus_pz),
        105.6583715,
        _rdcr->GetInterimLeafByName(muminus_px),
        _rdcr->GetInterimLeafByName(muminus_py),
        _rdcr->GetInterimLeafByName(muminus_pz),
        105.6583715);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_Jpsippi);

    doocore::io::sinfo << "Veto leaves are filled using constrain: " << mass_hypo_constraints << doocore::io::endmsg;

    if (_rdcr->LeafExists("varKS0MassHypoDaughtersPVConst_pipi")){
      // mass hypotheses using the maximal constrained fit (WIP)
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_pipi = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_pipi", "varKS0MassHypoDaughtersPVConst_pipi", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_pipi->FixedMassDaughtersTwoBodyDecayMotherMass(
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          139.57018,
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          139.57018);
      _rdcr->RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_pipi);
      
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_piK = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_piK", "varKS0MassHypoDaughtersPVConst_piK", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_piK->FixedMassDaughtersTwoBodyDecayMotherMass(
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          139.57018,
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          493.677);
      _rdcr->RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_piK);
      
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_Kpi = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_Kpi", "varKS0MassHypoDaughtersPVConst_Kpi", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_Kpi->FixedMassDaughtersTwoBodyDecayMotherMass(
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          493.677,
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          139.57018);
      _rdcr->RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_Kpi);
      
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_pip = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_pip", "varKS0MassHypoDaughtersPVConst_pip", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_pip->FixedMassDaughtersTwoBodyDecayMotherMass(
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          139.57018,
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          938.272046);
      _rdcr->RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_pip);
      
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_ppi = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_ppi", "varKS0MassHypoDaughtersPVConst_ppi", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_ppi->FixedMassDaughtersTwoBodyDecayMotherMass(
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          938.272046,
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          139.57018);
      _rdcr->RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_ppi);
    }
  }
  summary.Add("Veto fit constraints", mass_hypo_constraints);
}

void AuxiliaryLeaves(Reducer* _rdcr, cfg_tuple& cfg){
  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  // handle flattened tuples
  std::string flat_suffix = "";
  if (std::get<4>(cfg)) flat_suffix = "_flat";

  // random leaf
  TRandom3* random_generator_ = new TRandom3(42);
  ReducerLeaf<Int_t>& random_leaf = _rdcr->CreateIntLeaf("idxRandom", -1);
  random_leaf.Randomize(random_generator_);

  // background category
  if (std::get<3>(cfg)){
    ReducerLeaf<Int_t>& bkgcat_leaf = _rdcr->CreateIntCopyLeaf("catBkg", _rdcr->GetInterimLeafByName("B0_BKGCAT"));
  }
  // event and run number
  ReducerLeaf<Int_t>& event_number_leaf_ptr = _rdcr->CreateIntCopyLeaf("idxEventNumber", _rdcr->GetInterimLeafByName("eventNumber"));
  ReducerLeaf<Int_t>& run_number_leaf_ptr = _rdcr->CreateIntCopyLeaf("idxRunNumber", _rdcr->GetInterimLeafByName("runNumber"));
  // number of PVs
  ReducerLeaf<Int_t>& var_npv_leaf = _rdcr->CreateIntCopyLeaf("catNPV", _rdcr->GetInterimLeafByName("nPV"));
  // magnet direction
  ReducerLeaf<Int_t>& var_mag_leaf = _rdcr->CreateIntCopyLeaf("catMag", _rdcr->GetInterimLeafByName("Polarity"));
  // number of tracks
  ReducerLeaf<Int_t>& var_ntrack_leaf = _rdcr->CreateIntCopyLeaf("catNTrack", _rdcr->GetInterimLeafByName("nTracks"));
  // flat array index
  // if (std::get<4>(cfg)){    should be set allready by previous tool
  //   ReducerLeaf<Int_t>& flat_index_leaf_ptr = _rdcr->CreateIntCopyLeaf("idxPV", _rdcr->GetInterimLeafByName("flat_array_index"));
  // }
  // track ghost probability (only if pions or muons are available)
  if (_rdcr->LeafExists("piplus_TRACK_GhostProb")){
    ReducerLeaf<Double_t>* pip_track_ghost_prob_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPiPTrackGhostProb", _rdcr->GetInterimLeafByName("piplus_TRACK_GhostProb"));
    ReducerLeaf<Double_t>* pim_track_ghost_prob_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPiMTrackGhostProb", _rdcr->GetInterimLeafByName("piminus_TRACK_GhostProb"));
  }
  if (_rdcr->LeafExists("muplus_TRACK_GhostProb")){
    ReducerLeaf<Double_t>* mup_track_ghost_prob_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varMuPTrackGhostProb", _rdcr->GetInterimLeafByName("muplus_TRACK_GhostProb"));
    ReducerLeaf<Double_t>* mum_track_ghost_prob_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varMuMTrackGhostProb", _rdcr->GetInterimLeafByName("muminus_TRACK_GhostProb"));
  }

  // maximal muon track fit chi2ndof
  _rdcr->CreateDoubleLeaf("varMuonMaxTrackFitChi2ndof", -999999.).Maximum(_rdcr->GetInterimLeafByName("muplus_TRACK_CHI2NDOF"), _rdcr->GetInterimLeafByName("muminus_TRACK_CHI2NDOF"));
  // maximal pion track fit chi2ndof
  _rdcr->CreateDoubleLeaf("varPionMaxTrackFitChi2ndof", -999999.).Maximum(_rdcr->GetInterimLeafByName("piplus_TRACK_CHI2NDOF"), _rdcr->GetInterimLeafByName("piminus_TRACK_CHI2NDOF"));
  // minimal muon PID
  _rdcr->CreateDoubleLeaf("varMuonMinPIDmu", -999999.).Minimum(_rdcr->GetInterimLeafByName("muplus_PIDmu"), _rdcr->GetInterimLeafByName("muminus_PIDmu"));
  // minimal pion MinIP chi2
  _rdcr->CreateDoubleLeaf("varPionMinMinIPChi2", -999999.).Minimum(_rdcr->GetInterimLeafByName("piplus_MINIPCHI2"), _rdcr->GetInterimLeafByName("piminus_MINIPCHI2"));
  // End vertex chi2/ndof
  _rdcr->CreateDoubleLeaf("varBEndVtxChi2ndof", -99999999.).Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_ENDVERTEX_NDOF"));
  _rdcr->CreateDoubleLeaf("varJPsiEndVtxChi2ndof", -99999999.).Divide(_rdcr->GetInterimLeafByName("J_psi_1S_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName("J_psi_1S_ENDVERTEX_NDOF"));
  _rdcr->CreateDoubleLeaf("varKSEndVtxChi2ndof", -99999999.).Divide(_rdcr->GetInterimLeafByName("KS0_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName("KS0_ENDVERTEX_NDOF"));
  // minimal muon transverse momentum
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_J_psi_1S_P0_PT"+flat_suffix) && _rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_J_psi_1S_P1_PT"+flat_suffix)){
    _rdcr->CreateDoubleLeaf("varMuonDTFMinPT", -999999.).Minimum(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_J_psi_1S_P0_PT"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_J_psi_1S_P1_PT"+flat_suffix));
  }
  // minimal pion momentum
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_KS0_P0_P"+flat_suffix) && _rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_KS0_P1_P"+flat_suffix)){
    _rdcr->CreateDoubleLeaf("varPionDTFMinP", -999999.).Minimum(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_KS0_P0_P"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_KS0_P1_P"+flat_suffix));
  }
  // track type
  ReducerLeaf<Int_t>& catTrackType = _rdcr->CreateIntLeaf("catTrackType", 0);
  if(_rdcr->LeafExists("piplus_TRACK_Type")){
    catTrackType.AddCondition("longtrack", "(piplus_TRACK_Type==3)&&(piminus_TRACK_Type==3)", 33);
    catTrackType.AddCondition("downstream", "(piplus_TRACK_Type==5)&&(piminus_TRACK_Type==5)", 55);
    catTrackType.AddCondition("longdown", "(piplus_TRACK_Type==3)&&(piminus_TRACK_Type==5)", 35);
    catTrackType.AddCondition("downlong", "(piplus_TRACK_Type==5)&&(piminus_TRACK_Type==3)", 53);
  }
  // data taking period
  ReducerLeaf<Int_t>& cat_year_leaf = _rdcr->CreateIntLeaf("catYear", 0);
    cat_year_leaf.AddCondition("2011", "GpsTime < 1.325376e+15",  2011);
    cat_year_leaf.AddCondition("2012", "GpsTime >= 1.325376e+15", 2012);

  // decay tree fit
  // fit status
  if (_rdcr->LeafExists("B0_FitPVConst_status"+flat_suffix)) ReducerLeaf<Int_t>& dtf_status_pv_constraint = _rdcr->CreateIntCopyLeaf("varDTFStatusPVConst", _rdcr->GetInterimLeafByName("B0_FitPVConst_status"+flat_suffix));
  if (_rdcr->LeafExists("B0_FitDaughtersPVConst_status"+flat_suffix)) ReducerLeaf<Int_t>& dtf_status_daughters_pv_constraint = _rdcr->CreateIntCopyLeaf("varDTFStatusDaughtersPVConst", _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_status"+flat_suffix));

  // chi2ndof
  ReducerLeaf<Double_t>* dtf_chi2ndof_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitDaughtersPVConst_chi2"+flat_suffix)) {
    dtf_chi2ndof_leaf_ptr = &_rdcr->CreateDoubleLeaf("varDTFChi2ndof", -1.0);
    dtf_chi2ndof_leaf_ptr->Divide(_rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_chi2"+flat_suffix),
                                  _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_nDOF"+flat_suffix));
  } else if (_rdcr->LeafExists("B0_LOKI_DTF_CHI2NDOF")) {
    dtf_chi2ndof_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDTFChi2ndof", _rdcr->GetInterimLeafByName("B0_LOKI_DTF_CHI2NDOF"));
  }

  // IP chi2
  ReducerLeaf<Double_t>* ip_chi2_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitDaughtersPVConst_IPCHI2"+flat_suffix)) {
    ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDTFIPChi2", _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_IPCHI2"+flat_suffix));
  } else if (_rdcr->LeafExists("B0_IPCHI2_OWNPV")) {
    ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varIPChi2OwnPV", _rdcr->GetInterimLeafByName("B0_IPCHI2_OWNPV"));
  }

  // momentum leaves for CPT violation analysis
  // standard leaves
  ReducerLeaf<Double_t>& var_b0_momentum_x_leaf = _rdcr->CreateDoubleCopyLeaf("obsMomentumX", _rdcr->GetInterimLeafByName("B0_PX"));
  ReducerLeaf<Double_t>& var_b0_momentum_y_leaf = _rdcr->CreateDoubleCopyLeaf("obsMomentumY", _rdcr->GetInterimLeafByName("B0_PY"));
  ReducerLeaf<Double_t>& var_b0_momentum_z_leaf = _rdcr->CreateDoubleCopyLeaf("obsMomentumZ", _rdcr->GetInterimLeafByName("B0_PZ"));
  // DTF leaves
  if (_rdcr->LeafExists("B0_FitDaughtersPVConst_P"+flat_suffix)){
    ReducerLeaf<Double_t>& var_b0_dtf_momentum_p_leaf =      _rdcr->CreateDoubleCopyLeaf("obsDTFMomentum", _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_P"+flat_suffix));
    ReducerLeaf<Double_t>& var_b0_dtf_momentum_p_err_leaf =  _rdcr->CreateDoubleCopyLeaf("obsDTFMomentumError", _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_PERR"+flat_suffix));
    ReducerLeaf<Double_t>& var_b0_dtf_momentum_pt_leaf =     _rdcr->CreateDoubleCopyLeaf("obsDTFTransverseMomentum", _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_PT"+flat_suffix));
    ReducerLeaf<Double_t>& var_b0_dtf_momentum_pt_err_leaf = _rdcr->CreateDoubleCopyLeaf("obsDTFTransverseMomentumError", _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_PTERR"+flat_suffix));
  }

  // alternative daughter masses with different constraints
  if (_rdcr->LeafExists("B0_FitPVConst_KS0_M"+flat_suffix)) ReducerLeaf<Double_t>& dtf_kaon_mass_pv_constraint = _rdcr->CreateDoubleCopyLeaf("varDTFKS0MassPVConst", _rdcr->GetInterimLeafByName("B0_FitPVConst_KS0_M"+flat_suffix));
  if (_rdcr->LeafExists("B0_FitDaughtersPVConst_KS0_M"+flat_suffix)) ReducerLeaf<Double_t>& dtf_kaon_mass_daughters_pv_constraint = _rdcr->CreateDoubleCopyLeaf("varDTFKS0MassDaughtersPVConst", _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_M"+flat_suffix));
  if (_rdcr->LeafExists("B0_FitPVConst_J_psi_1S_M"+flat_suffix)) ReducerLeaf<Double_t>& dtf_kaon_mass_pv_constraint = _rdcr->CreateDoubleCopyLeaf("varDTFJpsiMassPVConst", _rdcr->GetInterimLeafByName("B0_FitPVConst_J_psi_1S_M"+flat_suffix));
  if (_rdcr->LeafExists("B0_FitDaughtersPVConst_J_psi_1S_M"+flat_suffix)) ReducerLeaf<Double_t>& dtf_kaon_mass_daughters_pv_constraint = _rdcr->CreateDoubleCopyLeaf("varDTFJpsiMassDaughtersPVConst", _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_J_psi_1S_M"+flat_suffix));

  // DTF PV position
  if (_rdcr->LeafExists("B0_FitDaughtersPVConst_PV_X"+flat_suffix)) ReducerLeaf<Double_t>& dtf_pv_position_x = _rdcr->CreateDoubleCopyLeaf("varDTFPVPosX", _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_PV_X"+flat_suffix));
  if (_rdcr->LeafExists("B0_FitDaughtersPVConst_PV_Y"+flat_suffix)) ReducerLeaf<Double_t>& dtf_pv_position_y = _rdcr->CreateDoubleCopyLeaf("varDTFPVPosY", _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_PV_Y"+flat_suffix));
  if (_rdcr->LeafExists("B0_FitDaughtersPVConst_PV_Z"+flat_suffix)) ReducerLeaf<Double_t>& dtf_pv_position_z = _rdcr->CreateDoubleCopyLeaf("varDTFPVPosZ", _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_PV_Z"+flat_suffix));
}

