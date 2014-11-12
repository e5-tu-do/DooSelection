/******************************************/
// B02DDVariablesGrimReaper.cpp
//
// Standalone GrimReaper that adds a lot of
// common variables to trees.
//
// Author: Frank Meier
// Date: 2014-11-07
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
void TriggerLeaves(Reducer* _rdcr);
void VetoLeaves(Reducer* _rdcr, cfg_tuple& cfg);
void AuxiliaryLeaves(Reducer* _rdcr, cfg_tuple& cfg);

int main(int argc, char * argv[]){
  sinfo << "-info-  \t" << "B02DDVariablesGrimReaper \t" << "Welcome!" << endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, decay_channel;
  if (argc == 5){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    decay_channel = "B02DD";
  }
  else{
    serr << "-ERROR- \t" << "B02DDVariablesGrimReaper \t" << "Parameters needed:" << endmsg;
    serr << "-ERROR- \t" << "B02DDVariablesGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name" << endmsg;
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
  TriggerLeaves(reducer);
  // VetoLeaves(reducer, cfg);
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
  if (_channel == "B02DD"){
    head = "B0";
    daughters.push_back("Dplus");
    daughters.push_back("Dminus");
    stable_particles.push_back("Kminus");
    stable_particles.push_back("Kplus");
    stable_particles.push_back("piminus");
    stable_particles.push_back("piminus0");
    stable_particles.push_back("piplus");
    stable_particles.push_back("piplus0");
    isMC = _rdcr->LeafExists(head+"_BKGCAT");
    isFlat = (_rdcr->LeafExists("flat_array_index") || _rdcr->LeafExists("idxPV"));
  }
  else{
    serr << "-ERROR- \t" << "B02DDVariablesGrimReaper \t" << "No valid decay channel. Possible decay channels are:" << endmsg;
    serr << "-ERROR- \t" << "B02DDVariablesGrimReaper \t" << "- B02DD" << endmsg;
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
  std::string main_observable_constraint = "PV";
  std::string main_observable_constraint_error = "PV";
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitDDPVConst_M"+flat_suffix)) {
    if (main_observable_constraint == "DDPV") main_observable_constraint = "FitDDPVConst_M";
    if (main_observable_constraint_error == "DDPV") main_observable_constraint_error = "FitDDPVConst_MERR";
    _rdcr->CreateDoubleCopyLeaf("obsMassDDPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDDPVConst_M"+flat_suffix));
    _rdcr->CreateDoubleCopyLeaf("obsMassErrDDPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitDDPVConst_MERR"+flat_suffix));
  } 
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_M"+flat_suffix)) {
    if (main_observable_constraint == "PV") main_observable_constraint = "FitPVConst_M";
    if (main_observable_constraint_error == "PV") main_observable_constraint_error = "FitPVConst_MERR";
    _rdcr->CreateDoubleCopyLeaf("obsMassPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_M"+flat_suffix));
    _rdcr->CreateDoubleCopyLeaf("obsMassErrPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_MERR"+flat_suffix));
    _rdcr->CreateDoubleCopyLeaf("obsMassDauOne", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_M"+flat_suffix));
    _rdcr->CreateDoubleCopyLeaf("obsMassErrDauOne", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_MERR"+flat_suffix));
    _rdcr->CreateDoubleCopyLeaf("obsMassDauTwo", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_M"+flat_suffix));
    _rdcr->CreateDoubleCopyLeaf("obsMassErrDauTwo", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_MERR"+flat_suffix));
  } 
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_LOKI_MASS_DDConstr")) {
    if (main_observable_constraint == "LOKI") main_observable_constraint = "LOKI_MASS_DDConstr";
    if (main_observable_constraint_error == "LOKI") main_observable_constraint_error = "LOKI_MASSERR_DDConstr";
    _rdcr->CreateDoubleCopyLeaf("obsMassLokiDDPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_LOKI_MASS_DDConstr"));
    _rdcr->CreateDoubleCopyLeaf("obsMassErrLokiDDPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_LOKI_MASSERR_DDConstr"));
    _rdcr->CreateDoubleCopyLeaf("obsMassLokiDauOne", _rdcr->GetInterimLeafByName("Dplus_MM"));
    _rdcr->CreateDoubleCopyLeaf("obsMassErrLokiDauOne", _rdcr->GetInterimLeafByName("Dplus_MMERR"));
    _rdcr->CreateDoubleCopyLeaf("obsMassLokiDauTwo", _rdcr->GetInterimLeafByName("Dminus_MM"));
    _rdcr->CreateDoubleCopyLeaf("obsMassErrLokiDauTwo", _rdcr->GetInterimLeafByName("Dminus_MMERR"));
  } 
  _rdcr->CreateDoubleCopyLeaf("obsMass", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_"+main_observable_constraint+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("obsMassErr", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_"+main_observable_constraint_error+flat_suffix));
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
    if(_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_Dplus_tau")){
      _rdcr->CreateDoubleLeaf("varDplusTauSignificance", -99999999.).Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_tau"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_tauErr"+flat_suffix));
    }
    if(_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_Dminus_tau")){
      _rdcr->CreateDoubleLeaf("varDminusTauSignificance", -99999999.).Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_tau"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_tauErr"+flat_suffix));
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

void TriggerLeaves(Reducer* _rdcr){
  // trigger categories
  _rdcr->CreateIntCopyLeaf("catTriggerL0GlobalTOS", _rdcr->GetInterimLeafByName("B0_L0Global_TOS"));
  _rdcr->CreateIntCopyLeaf("catTriggerHlt1GlobalTOS", _rdcr->GetInterimLeafByName("B0_Hlt1Global_TOS"));
  _rdcr->CreateIntCopyLeaf("catTriggerHlt2GlobalTOS", _rdcr->GetInterimLeafByName("B0_Hlt2Global_TOS"));
  _rdcr->CreateIntCopyLeaf("catTriggerHlt2Topo2BodyBBDTTOS", _rdcr->GetInterimLeafByName("B0_Hlt2Topo2BodyBBDTDecision_TOS"));
  _rdcr->CreateIntCopyLeaf("catTriggerHlt2Topo3BodyBBDTTOS", _rdcr->GetInterimLeafByName("B0_Hlt2Topo3BodyBBDTDecision_TOS"));
  _rdcr->CreateIntCopyLeaf("catTriggerHlt2Topo4BodyBBDTTOS", _rdcr->GetInterimLeafByName("B0_Hlt2Topo4BodyBBDTDecision_TOS"));

  ReducerLeaf<Int_t>& catTriggerSetTopo234bodybbdt = _rdcr->CreateIntLeaf("catTriggerSetTopo234BodyBBDT", -10);
      TCut trigger_set_topo234bodybbdt = "B0_Hlt2Topo2BodyBBDTDecision_TOS==1||B0_Hlt2Topo3BodyBBDTDecision_TOS==1||B0_Hlt2Topo4BodyBBDTDecision_TOS==1";
      catTriggerSetTopo234bodybbdt.AddCondition("triggered", trigger_set_topo234bodybbdt.GetTitle(), 1);
      catTriggerSetTopo234bodybbdt.AddCondition("not_triggered", (!trigger_set_topo234bodybbdt).GetTitle(), 0);
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
    _rdcr->CreateIntCopyLeaf("catBkg", _rdcr->GetInterimLeafByName("B0_BKGCAT"));
  }
  // event and run number
  _rdcr->CreateIntCopyLeaf("idxEventNumber", _rdcr->GetInterimLeafByName("eventNumber"));
  _rdcr->CreateIntCopyLeaf("idxRunNumber", _rdcr->GetInterimLeafByName("runNumber"));
  // number of PVs
  _rdcr->CreateIntCopyLeaf("catNPV", _rdcr->GetInterimLeafByName("nPV"));
  // magnet direction
  _rdcr->CreateIntCopyLeaf("catMag", _rdcr->GetInterimLeafByName("Polarity"));
  // number of tracks
  _rdcr->CreateIntCopyLeaf("catNTrack", _rdcr->GetInterimLeafByName("nTracks"));
  // track ghost probability
  _rdcr->CreateDoubleCopyLeaf("varKplusTrackGhostProb", _rdcr->GetInterimLeafByName("Kplus_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varKminusTrackGhostProb", _rdcr->GetInterimLeafByName("Kminus_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varPiplusOneTrackGhostProb", _rdcr->GetInterimLeafByName("piplus_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varPiplusTwoTrackGhostProb", _rdcr->GetInterimLeafByName("piplus0_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varPiminusOneTrackGhostProb", _rdcr->GetInterimLeafByName("piminus_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varPiminusTwoTrackGhostProb", _rdcr->GetInterimLeafByName("piminus0_TRACK_GhostProb"));
  
  // End vertex chi2/ndof
  _rdcr->CreateDoubleLeaf("varBEndVtxChi2ndof", -99999999.).Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_ENDVERTEX_NDOF"));
  _rdcr->CreateDoubleLeaf("varDplusEndVtxChi2ndof", -99999999.).Divide(_rdcr->GetInterimLeafByName("Dplus_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName("Dplus_ENDVERTEX_NDOF"));
  _rdcr->CreateDoubleLeaf("varDminusEndVtxChi2ndof", -99999999.).Divide(_rdcr->GetInterimLeafByName("Dminus_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName("Dminus_ENDVERTEX_NDOF"));
  // sum of D transverse momenta
  _rdcr->CreateDoubleLeaf("varDDDTFSumPT", -99999999.).Add(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_PT"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_PT"+flat_suffix));
  // maximal kaon track fit chi2ndof
  _rdcr->CreateDoubleLeaf("varKaonMaxTrackFitChi2ndof", -999999.).Maximum(_rdcr->GetInterimLeafByName("Kplus_TRACK_CHI2NDOF"), _rdcr->GetInterimLeafByName("Kminus_TRACK_CHI2NDOF"));
  // maximal pion track fit chi2ndof
  ReducerLeaf<Double_t>& piplus_max_trackchi2 = _rdcr->CreateDoubleLeaf("varPiPlusMaxTrackFitChi2ndof", -999999.);
  piplus_max_trackchi2.Maximum(_rdcr->GetInterimLeafByName("piplus_TRACK_CHI2NDOF"), _rdcr->GetInterimLeafByName("piplus0_TRACK_CHI2NDOF"));
  ReducerLeaf<Double_t>& piminus_max_trackchi2 = _rdcr->CreateDoubleLeaf("varPiMinusMaxTrackFitChi2ndof", -999999.);
  piminus_max_trackchi2.Maximum(_rdcr->GetInterimLeafByName("piminus_TRACK_CHI2NDOF"), _rdcr->GetInterimLeafByName("piminus0_TRACK_CHI2NDOF"));
  _rdcr->CreateDoubleLeaf("varPionMaxTrackFitChi2ndof", -999999.).Maximum(piplus_max_trackchi2, piminus_max_trackchi2);
  // minimal kaon MinIP chi2
  _rdcr->CreateDoubleLeaf("varKaonMinMinIPChi2", -999999.).Minimum(_rdcr->GetInterimLeafByName("Kplus_MINIPCHI2"), _rdcr->GetInterimLeafByName("Kminus_MINIPCHI2"));
  // minimal pion MinIP chi2
  ReducerLeaf<Double_t>& piplus_min_minipchi2 = _rdcr->CreateDoubleLeaf("piplus_min_minipchi2", -999999.);
  piplus_min_minipchi2.Minimum(_rdcr->GetInterimLeafByName("piplus_MINIPCHI2"), _rdcr->GetInterimLeafByName("piplus0_MINIPCHI2"));
  ReducerLeaf<Double_t>& piminus_min_minipchi2 = _rdcr->CreateDoubleLeaf("piminus_min_minipchi2", -999999.);
  piminus_min_minipchi2.Minimum(_rdcr->GetInterimLeafByName("piminus_MINIPCHI2"), _rdcr->GetInterimLeafByName("piminus0_MINIPCHI2"));
  _rdcr->CreateDoubleLeaf("varPionMinMinIPChi2", -999999.).Minimum(piplus_min_minipchi2, piminus_min_minipchi2);
  // sum of D daughters transverse momentum
  ReducerLeaf<Double_t>& dplus_sum_daughters_pt = _rdcr->CreateDoubleLeaf("varDplusDaughtersSumPT", -999999.);
  dplus_sum_daughters_pt.Add(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_P0_PT"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_P1_PT"+flat_suffix));
  dplus_sum_daughters_pt.Add(dplus_sum_daughters_pt, _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_P2_PT"+flat_suffix));
  ReducerLeaf<Double_t>& dminus_sum_daughters_pt = _rdcr->CreateDoubleLeaf("varDminusDaughtersSumPT", -999999.);
  dminus_sum_daughters_pt.Add(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_P0_PT"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_P1_PT"+flat_suffix));
  dminus_sum_daughters_pt.Add(dminus_sum_daughters_pt, _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_P2_PT"+flat_suffix));

  // data taking period
  ReducerLeaf<Int_t>& cat_year_leaf = _rdcr->CreateIntLeaf("catYear", 0);
    cat_year_leaf.AddCondition("2011", "GpsTime < 1.325376e+15",  2011);
    cat_year_leaf.AddCondition("2012", "GpsTime >= 1.325376e+15", 2012);

  // decay tree fit
  // fit status
  if (_rdcr->LeafExists("B0_FitPVConst_status"+flat_suffix)) _rdcr->CreateIntCopyLeaf("varDTFStatusPVConst", _rdcr->GetInterimLeafByName("B0_FitPVConst_status"+flat_suffix));
  if (_rdcr->LeafExists("B0_FitDDPVConst_status"+flat_suffix)) _rdcr->CreateIntCopyLeaf("varDTFStatusDDPVConst", _rdcr->GetInterimLeafByName("B0_FitDDPVConst_status"+flat_suffix));

  // chi2ndof
  ReducerLeaf<Double_t>* dtf_chi2ndof_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitPVConst_chi2"+flat_suffix)) {
    dtf_chi2ndof_leaf_ptr = &_rdcr->CreateDoubleLeaf("varDTFChi2ndof", -1.0);
    dtf_chi2ndof_leaf_ptr->Divide(_rdcr->GetInterimLeafByName("B0_FitPVConst_chi2"+flat_suffix),
                                  _rdcr->GetInterimLeafByName("B0_FitPVConst_nDOF"+flat_suffix));
  } else if (_rdcr->LeafExists("B0_LOKI_DTF_CHI2NDOF")) {
    dtf_chi2ndof_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDTFChi2ndof", _rdcr->GetInterimLeafByName("B0_LOKI_DTF_CHI2NDOF"));
  }

  // IP chi2
  ReducerLeaf<Double_t>* ip_chi2_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitPVConst_IPCHI2"+flat_suffix)) {
    ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDTFIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_IPCHI2"+flat_suffix));
  } else if (_rdcr->LeafExists("B0_IPCHI2_OWNPV")) {
    ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varIPChi2OwnPV", _rdcr->GetInterimLeafByName("B0_IPCHI2_OWNPV"));
  }

  // Direction angle (DIRA)
  ReducerLeaf<Double_t>* dira_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* dplus_dira_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* dminus_dira_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitPVConst_DIRA"+flat_suffix)) {
    dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varBDTFDIRA", _rdcr->GetInterimLeafByName("B0_FitPVConst_DIRA"+flat_suffix));
    dplus_dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplusDTFDIRA", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_DIRA"+flat_suffix));
    dminus_dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDminusDTFDIRA", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_DIRA"+flat_suffix));
  } else if (_rdcr->LeafExists("B0_DIRA_OWNPV")) {
    dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDIRAOwnPV", _rdcr->GetInterimLeafByName("B0_DIRA_OWNPV"));
    dplus_dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplusDIRAOwnPV", _rdcr->GetInterimLeafByName("Dplus_DIRA_OWNPV"));
    dminus_dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDminusDIRAOwnPV", _rdcr->GetInterimLeafByName("Dminus_DIRA_OWNPV"));
  }

  // DTF PV position
  if (_rdcr->LeafExists("B0_FitPVConst_PV_X"+flat_suffix)) _rdcr->CreateDoubleCopyLeaf("varDTFPVPosX", _rdcr->GetInterimLeafByName("B0_FitPVConst_PV_X"+flat_suffix));
  if (_rdcr->LeafExists("B0_FitPVConst_PV_Y"+flat_suffix)) _rdcr->CreateDoubleCopyLeaf("varDTFPVPosY", _rdcr->GetInterimLeafByName("B0_FitPVConst_PV_Y"+flat_suffix));
  if (_rdcr->LeafExists("B0_FitPVConst_PV_Z"+flat_suffix)) _rdcr->CreateDoubleCopyLeaf("varDTFPVPosZ", _rdcr->GetInterimLeafByName("B0_FitPVConst_PV_Z"+flat_suffix));

  // momentum leaves
  // standard leaves
  _rdcr->CreateDoubleCopyLeaf("obsMomentum", _rdcr->GetInterimLeafByName("B0_P"));
  _rdcr->CreateDoubleCopyLeaf("obsMomentumX", _rdcr->GetInterimLeafByName("B0_PX"));
  _rdcr->CreateDoubleCopyLeaf("obsMomentumY", _rdcr->GetInterimLeafByName("B0_PY"));
  _rdcr->CreateDoubleCopyLeaf("obsMomentumZ", _rdcr->GetInterimLeafByName("B0_PZ"));
  _rdcr->CreateDoubleCopyLeaf("obsTransverseMomentum", _rdcr->GetInterimLeafByName("B0_PT"));
  // DTF leaves
  if (_rdcr->LeafExists("B0_FitPVConst_P"+flat_suffix)){
    _rdcr->CreateDoubleCopyLeaf("obsDTFMomentum", _rdcr->GetInterimLeafByName("B0_FitPVConst_P"+flat_suffix));
    _rdcr->CreateDoubleCopyLeaf("obsDTFMomentumError", _rdcr->GetInterimLeafByName("B0_FitPVConst_PERR"+flat_suffix));
    _rdcr->CreateDoubleCopyLeaf("obsDTFTransverseMomentum", _rdcr->GetInterimLeafByName("B0_FitPVConst_PT"+flat_suffix));
    _rdcr->CreateDoubleCopyLeaf("obsDTFTransverseMomentumError", _rdcr->GetInterimLeafByName("B0_FitPVConst_PTERR"+flat_suffix));
  }
  // daughters
  _rdcr->CreateDoubleCopyLeaf("varDplusP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varDplusPT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varDminusP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varDminusPT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_PT"+flat_suffix));
  // grand-daughters
  _rdcr->CreateDoubleCopyLeaf("varpiplusOneP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P0_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varpiplusOnePT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P0_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varpiplusTwoP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P1_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varpiplusTwoPT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P1_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varpiminusOneP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varpiminusOnePT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varpiminusTwoP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varpiminusTwoPT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varKplusP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P2_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varKplusPT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P2_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varKminusP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varKminusPT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_PT"+flat_suffix));
}

