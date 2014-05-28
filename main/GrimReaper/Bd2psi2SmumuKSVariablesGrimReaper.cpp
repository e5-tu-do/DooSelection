/******************************************/
// Bd2psi2SmumuKSVariablesGrimReaper.cpp
//
// Standalone GrimReaper that adds a lot of
// common variables to trees.
//
// Author: Christophe Cauet
// Date: 2014-03-10
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
void MCLeaves(Reducer* _rdcr, cfg_tuple& cfg);
void MassLeaves(Reducer* _rdcr, cfg_tuple& cfg);
void TimeLeaves(Reducer* _rdcr, cfg_tuple& cfg);
void TriggerLeaves(Reducer* _rdcr, cfg_tuple& cfg);
void VetoLeaves(Reducer* _rdcr, cfg_tuple& cfg);
void AuxiliaryLeaves(Reducer* _rdcr, cfg_tuple& cfg);

int main(int argc, char * argv[]){
  sinfo << "-info-  \t" << "Bd2psi2SmumuKSVariablesGrimReaper \t" << "Welcome!" << endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, decay_channel;
  if (argc == 5){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    decay_channel = "Bd2psi2SKS";
  }
  else{
    serr << "-ERROR- \t" << "Bd2psi2SmumuKSVariablesGrimReaper \t" << "Parameters needed:" << endmsg;
    serr << "-ERROR- \t" << "Bd2psi2SmumuKSVariablesGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name" << endmsg;
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
  if (reducer->LeafExists("B0_BKGCAT")) MCLeaves(reducer, cfg);
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
  if (_channel == "Bd2psi2SKS"){
    head = "B0";
    daughters.push_back("psi_2S");
    daughters.push_back("KS0");
    stable_particles.push_back("muminus");
    stable_particles.push_back("muplus");
    stable_particles.push_back("piminus");
    stable_particles.push_back("piplus");
    isMC = _rdcr->LeafExists(head+"_BKGCAT");
    isFlat = (_rdcr->LeafExists("flat_array_index") || _rdcr->LeafExists("idxPV"));
  }
  else{
    serr << "-ERROR- \t" << "Bd2psi2SmumuKSVariablesGrimReaper \t" << "No valid decay channel. Possible decay channels are:" << endmsg;
    serr << "-ERROR- \t" << "Bd2psi2SmumuKSVariablesGrimReaper \t" << "- Bd2psi2SKS" << endmsg;
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

void MCLeaves(Reducer* _rdcr, cfg_tuple& cfg){}

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
    ReducerLeaf<Double_t>& mass_psi2s_ks_pv_leaf     = _rdcr->CreateDoubleCopyLeaf("obsMassPsi2SKSPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_M"+flat_suffix));
    ReducerLeaf<Double_t>& mass_err_psi2s_ks_pv_leaf = _rdcr->CreateDoubleCopyLeaf("obsMassErrPsi2SKSPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDaughtersPVConst_MERR"+flat_suffix));
  }
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_Fitpsi2SPVConst_M"+flat_suffix)) {
    if (main_observable_constraint == "") main_observable_constraint = "Fitpsi2SPVConst_M";
    if (main_observable_constraint_error == "") main_observable_constraint_error = "Fitpsi2SPVConst_MERR";
    ReducerLeaf<Double_t>& mass_pv_leaf     = _rdcr->CreateDoubleCopyLeaf("obsMassPsi2SPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_Fitpsi2SPVConst_M"+flat_suffix));
    ReducerLeaf<Double_t>& mass_err_pv_leaf = _rdcr->CreateDoubleCopyLeaf("obsMassErrpsi2SPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_Fitpsi2SPVConst_MERR"+flat_suffix));
  }
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_M"+flat_suffix)) {
    if (main_observable_constraint == "") main_observable_constraint = "FitPVConst_M";
    if (main_observable_constraint_error == "") main_observable_constraint_error = "FitPVConst_MERR";
    ReducerLeaf<Double_t>& mass_pv_leaf     = _rdcr->CreateDoubleCopyLeaf("obsMassPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_M"+flat_suffix));
    ReducerLeaf<Double_t>& mass_err_pv_leaf = _rdcr->CreateDoubleCopyLeaf("obsMassErrPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_MERR"+flat_suffix));
  }
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_MM")) {
    if (main_observable_constraint == "") main_observable_constraint = "MM";
    if (main_observable_constraint_error == "") main_observable_constraint_error = "MMERR";
    ReducerLeaf<Double_t>& mass_loki_jpsi_pv_leaf     = _rdcr->CreateDoubleCopyLeaf("obsMassLokiJpsiPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_MM"));
    ReducerLeaf<Double_t>& mass_err_loki_jpsi_pv_leaf = _rdcr->CreateDoubleCopyLeaf("obsMassErrLokiJpsiPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_MMERR"));
  }
  ReducerLeaf<Double_t>& mass_jpsi_ks_pv_leaf     = _rdcr->CreateDoubleCopyLeaf("obsMass", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_"+main_observable_constraint+flat_suffix));
  ReducerLeaf<Double_t>& mass_err_jpsi_ks_pv_leaf = _rdcr->CreateDoubleCopyLeaf("obsMassErr", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_"+main_observable_constraint_error+flat_suffix));
  summary.Add("obsMass fit constraints", main_observable_constraint);  
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
  else if (_rdcr->LeafExists(std::get<0>(cfg)+"_TAU")){
    fit_constraints = "NoDTF";
    tau_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsTime", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_TAU"), 1000.0);
    tau_err_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsTimeErr", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_TAUERR"), 1000.0);
  }
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_BKGCAT")){
    tau_true_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsTime_True", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_TRUETAU"), 1000.0);
    // tau_true_err_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsTimeErr_True", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_TRUETAU"), 1000.0); // ???????
  }

  summary.Add("Time fit constraints", fit_constraints);
}

void TriggerLeaves(Reducer* _rdcr, cfg_tuple& cfg){
  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  // trigger categories
  ReducerLeaf<Int_t>& trigger_l0_global_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerL0GlobalTOS", _rdcr->GetInterimLeafByName("psi_2S_L0Global_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt1_global_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerHlt1GlobalTOS", _rdcr->GetInterimLeafByName("psi_2S_Hlt1Global_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt2_global_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerHlt2GlobalTOS", _rdcr->GetInterimLeafByName("psi_2S_Hlt2Global_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt1_trackmuon_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerHlt1TrackMuonTOS", _rdcr->GetInterimLeafByName("psi_2S_Hlt1TrackMuonDecision_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt1_highmass_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerHlt1HighMassTOS", _rdcr->GetInterimLeafByName("psi_2S_Hlt1DiMuonHighMassDecision_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt1_no_ip_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerHlt1NoIPTOS", _rdcr->GetInterimLeafByName("psi_2S_Hlt1SingleMuonNoIPDecision_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt2_detachedjpsi_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerHlt2DetachedJpsiTOS", _rdcr->GetInterimLeafByName("psi_2S_Hlt2DiMuonDetachedJPsiDecision_TOS"));
  ReducerLeaf<Int_t>& trigger_hlt2_jpsi_leaf_ptr = _rdcr->CreateIntCopyLeaf("catTriggerHlt2JpsiTOS", _rdcr->GetInterimLeafByName("psi_2S_Hlt2DiMuonJPsiDecision_TOS"));

  ReducerLeaf<Int_t>& catTriggerSetNoIP = _rdcr->CreateIntLeaf("catTriggerSetNoIP", -10);
      TCut trigger_set_no_ip = "((psi_2S_Hlt1DiMuonHighMassDecision_TOS==1)||(psi_2S_Hlt1TrackMuonDecision_TOS==1)||(psi_2S_Hlt1SingleMuonNoIPDecision_TOS==1))&&((psi_2S_Hlt2DiMuonDetachedJPsiDecision_TOS==1)||(psi_2S_Hlt2DiMuonJPsiDecision_TOS==1))";
      catTriggerSetNoIP.AddCondition("triggered", trigger_set_no_ip.GetTitle(), 1);
      catTriggerSetNoIP.AddCondition("not_triggered", (!trigger_set_no_ip).GetTitle(), 0);

  ReducerLeaf<Int_t>& catTriggerSet = _rdcr->CreateIntLeaf("catTriggerSet", -10);
      TCut trigger_set = "((psi_2S_Hlt1DiMuonHighMassDecision_TOS==1)||(psi_2S_Hlt1TrackMuonDecision_TOS==1))&&((psi_2S_Hlt2DiMuonDetachedJPsiDecision_TOS==1)||(psi_2S_Hlt2DiMuonJPsiDecision_TOS==1))";
      catTriggerSet.AddCondition("triggered", trigger_set.GetTitle(), 1);
      catTriggerSet.AddCondition("not_triggered", (!trigger_set).GetTitle(), 0);
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
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_Fitpsi2SPVConst_KS0_P0_PX")){
    piplus_px  = std::get<0>(cfg)+"_Fitpsi2SPVConst_KS0_P0_PX"+flat_suffix;   
    piplus_py  = std::get<0>(cfg)+"_Fitpsi2SPVConst_KS0_P0_PY"+flat_suffix;   
    piplus_pz  = std::get<0>(cfg)+"_Fitpsi2SPVConst_KS0_P0_PZ"+flat_suffix;   
    piminus_px = std::get<0>(cfg)+"_Fitpsi2SPVConst_KS0_P1_PX"+flat_suffix;     
    piminus_py = std::get<0>(cfg)+"_Fitpsi2SPVConst_KS0_P1_PY"+flat_suffix;     
    piminus_pz = std::get<0>(cfg)+"_Fitpsi2SPVConst_KS0_P1_PZ"+flat_suffix;  
    muplus_px  = std::get<0>(cfg)+"_Fitpsi2SPVConst_J_psi_1S_P0_PX"+flat_suffix;   
    muplus_py  = std::get<0>(cfg)+"_Fitpsi2SPVConst_J_psi_1S_P0_PY"+flat_suffix;   
    muplus_pz  = std::get<0>(cfg)+"_Fitpsi2SPVConst_J_psi_1S_P0_PZ"+flat_suffix;   
    muminus_px = std::get<0>(cfg)+"_Fitpsi2SPVConst_J_psi_1S_P1_PX"+flat_suffix;     
    muminus_py = std::get<0>(cfg)+"_Fitpsi2SPVConst_J_psi_1S_P1_PY"+flat_suffix;     
    muminus_pz = std::get<0>(cfg)+"_Fitpsi2SPVConst_J_psi_1S_P1_PZ"+flat_suffix;
    mass_hypo_constraints = "psi2SPV";
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

    doocore::io::sinfo << "Veto leaves are filled using constrain: " << mass_hypo_constraints << doocore::io::endmsg;
  }
  else{
    doocore::io::sinfo << "No veto leaves created!" << doocore::io::endmsg;
    mass_hypo_constraints = "n/a";
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
  // data taking period
  ReducerLeaf<Int_t>& cat_year_leaf = _rdcr->CreateIntCopyLeaf("catYear", _rdcr->GetInterimLeafByName("GpsYear"));
  // number of tracks
  ReducerLeaf<Int_t>& var_ntrack_leaf = _rdcr->CreateIntCopyLeaf("catNTrack", _rdcr->GetInterimLeafByName("nTracks"));
  // flat array index
  // if (std::get<4>(cfg)){    should be set allready by previous tool
  //   ReducerLeaf<Int_t>& flat_index_leaf_ptr = _rdcr->CreateIntCopyLeaf("idxPV", _rdcr->GetInterimLeafByName("flat_array_index"));
  // }

  // maximal muon track fit chi2ndof
  _rdcr->CreateDoubleLeaf("varMuonMaxTrackFitChi2ndof", -999999.).Maximum(_rdcr->GetInterimLeafByName("muplus_TRACK_CHI2NDOF"), _rdcr->GetInterimLeafByName("muminus_TRACK_CHI2NDOF"));
  // maximal KS0 pion track fit chi2ndof
  _rdcr->CreateDoubleLeaf("varKS0PionMaxTrackFitChi2ndof", -999999.).Maximum(_rdcr->GetInterimLeafByName("piplus_TRACK_CHI2NDOF"), _rdcr->GetInterimLeafByName("piminus_TRACK_CHI2NDOF"));
  // minimal muon PID
  _rdcr->CreateDoubleLeaf("varMuonMinPIDmu", -999999.).Minimum(_rdcr->GetInterimLeafByName("muplus_PIDmu"), _rdcr->GetInterimLeafByName("muminus_PIDmu"));
  // minimal KS0 pion MinIP chi2
  _rdcr->CreateDoubleLeaf("varKS0PionMinMinIPChi2", -999999.).Minimum(_rdcr->GetInterimLeafByName("piplus_MINIPCHI2"), _rdcr->GetInterimLeafByName("piminus_MINIPCHI2"));
  // End vertex chi2/ndof
  _rdcr->CreateDoubleLeaf("varBEndVtxChi2ndof", -99999999.).Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_ENDVERTEX_NDOF"));
  _rdcr->CreateDoubleLeaf("varPsi2SEndVtxChi2ndof", -99999999.).Divide(_rdcr->GetInterimLeafByName("psi_2S_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName("psi_2S_ENDVERTEX_NDOF"));
  _rdcr->CreateDoubleLeaf("varPsi2SEndVtxChi2ndof", -99999999.).Divide(_rdcr->GetInterimLeafByName("psi_2S_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName("psi_2S_ENDVERTEX_NDOF"));
  _rdcr->CreateDoubleLeaf("varKSEndVtxChi2ndof", -99999999.).Divide(_rdcr->GetInterimLeafByName("KS0_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName("KS0_ENDVERTEX_NDOF"));
  // minimal muon transverse momentum
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_J_psi_1S_P0_PT"+flat_suffix) && _rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_J_psi_1S_P1_PT"+flat_suffix)){
    _rdcr->CreateDoubleLeaf("varMuonDTFMinPT", -999999.).Minimum(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_J_psi_1S_P0_PT"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_J_psi_1S_P1_PT"+flat_suffix));
  }
  // minimal KS0 pion momentum
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_KS0_P0_P"+flat_suffix) && _rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_KS0_P1_P"+flat_suffix)){
    _rdcr->CreateDoubleLeaf("varKS0PionDTFMinP", -999999.).Minimum(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_KS0_P0_P"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_KS0_P1_P"+flat_suffix));
  }
  // track type
  ReducerLeaf<Int_t>& catTrackType = _rdcr->CreateIntLeaf("catTrackType", 0);
  if(_rdcr->LeafExists("piplus_TRACK_Type")){
    catTrackType.AddCondition("longtrack", "(piplus_TRACK_Type==3)&&(piminus_TRACK_Type==3)", 33);
    catTrackType.AddCondition("downstream", "(piplus_TRACK_Type==5)&&(piminus_TRACK_Type==5)", 55);
    catTrackType.AddCondition("longdown", "(piplus_TRACK_Type==3)&&(piminus_TRACK_Type==5)", 35);
    catTrackType.AddCondition("downlong", "(piplus_TRACK_Type==5)&&(piminus_TRACK_Type==3)", 53);
  }

  // decay tree fit
  // fit status
  if (_rdcr->LeafExists("B0_FitPVConst_status"+flat_suffix)) ReducerLeaf<Int_t>& dtf_status_pv_constraint = _rdcr->CreateIntCopyLeaf("varDTFStatusPVConst", _rdcr->GetInterimLeafByName("B0_FitPVConst_status"+flat_suffix));
  if (_rdcr->LeafExists("B0_FitDaughtersPVConst_status"+flat_suffix)) ReducerLeaf<Int_t>& dtf_status_psi_2s_ks0_pv_constraint = _rdcr->CreateIntCopyLeaf("varDTFStatusPsi2SKSPVConst", _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_status"+flat_suffix));

  // chi2ndof
  ReducerLeaf<Double_t>* dtf_psi_2s_ks0_chi2ndof_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* dtf_psi_2s_chi2ndof_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* dtf_loki_chi2ndof_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitDaughtersPVConst_chi2"+flat_suffix)) {
    dtf_psi_2s_ks0_chi2ndof_leaf_ptr = &_rdcr->CreateDoubleLeaf("varDTFPsi2SKSPVConstChi2ndof", -1.0);
    dtf_psi_2s_ks0_chi2ndof_leaf_ptr->Divide(_rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_chi2"+flat_suffix),
                                  _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_nDOF"+flat_suffix));
  } 
  if (_rdcr->LeafExists("B0_Fitpsi2SPVConst_chi2"+flat_suffix)) {
    dtf_psi_2s_chi2ndof_leaf_ptr = &_rdcr->CreateDoubleLeaf("varDTFPsi2SPVConstChi2ndof", -1.0);
    dtf_psi_2s_chi2ndof_leaf_ptr->Divide(_rdcr->GetInterimLeafByName("B0_Fitpsi2SPVConst_chi2"+flat_suffix),
                                  _rdcr->GetInterimLeafByName("B0_Fitpsi2SPVConst_nDOF"+flat_suffix));
  } 
  if (_rdcr->LeafExists("B0_LOKI_DTF_CHI2NDOF")) {
    dtf_loki_chi2ndof_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDTFLokiChi2ndof", _rdcr->GetInterimLeafByName("B0_LOKI_DTF_CHI2NDOF"));
  }

  // IP chi2
  ReducerLeaf<Double_t>* ip_chi2_psi_2s_ks0_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* ip_chi2_psi_2s_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* ip_chi2_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitDaughtersPVConst_IPCHI2"+flat_suffix)) {
    ip_chi2_psi_2s_ks0_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDTFPsi2SKSPVIPChi2", _rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_IPCHI2"+flat_suffix));
  }
  if (_rdcr->LeafExists("B0_Fitpsi2SPVConst_IPCHI2"+flat_suffix)) {
    ip_chi2_psi_2s_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDTFPsi2SPVIPChi2", _rdcr->GetInterimLeafByName("B0_Fitpsi2SPVConst_IPCHI2"+flat_suffix));
  }
  if (_rdcr->LeafExists("B0_IPCHI2_OWNPV")) {
    ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varIPChi2OwnPV", _rdcr->GetInterimLeafByName("B0_IPCHI2_OWNPV"));
  }
}

