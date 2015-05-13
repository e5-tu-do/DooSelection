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
  if (_channel == "B02DD"){
    head = "B0";
    daughters.push_back("Dplus");
    daughters.push_back("Dminus");
    stable_particles.push_back("Dplus_Kminus_or_piminus");
    stable_particles.push_back("Dplus_piplus_or_Kplus_One");
    stable_particles.push_back("Dplus_piplus_or_Kplus_Two");
    stable_particles.push_back("Dminus_Kplus_or_piplus");
    stable_particles.push_back("Dminus_piminus_or_Kminus_One");
    stable_particles.push_back("Dminus_piminus_or_Kminus_Two");
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
    fit_constraints = "PV";
    tau_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsTime", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_tau"+flat_suffix), 1000.0);
    tau_err_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("obsTimeErr", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_tauErr"+flat_suffix), 1000.0);
    ReducerLeaf<Double_t>& varDplusTauSignificance = _rdcr->CreateDoubleLeaf("varDplusTauSignificance", -999999.);
    varDplusTauSignificance.Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_tau"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_tauErr"+flat_suffix));
    ReducerLeaf<Double_t>& varDminusTauSignificance = _rdcr->CreateDoubleLeaf("varDminusTauSignificance", -999999.);
    varDminusTauSignificance.Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_tau"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_tauErr"+flat_suffix));
    _rdcr->CreateDoubleLeaf("varDMinTauSignificance", -999999.).Minimum(varDplusTauSignificance, varDminusTauSignificance);
    _rdcr->CreateDoubleLeaf("varDMaxTauSignificance", -999999.).Maximum(varDplusTauSignificance, varDminusTauSignificance);
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
  _rdcr->CreateIntCopyLeaf("catTriggerHlt2IncPhiTOS", _rdcr->GetInterimLeafByName("B0_Hlt2IncPhiDecision_TOS"));

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
  std::string Dplus_px, Dplus_py, Dplus_pz;
  std::string Dminus_px, Dminus_py, Dminus_pz;
  std::string piplus_px, piplus_py, piplus_pz;
  std::string piplus0_px, piplus0_py, piplus0_pz;
  std::string piminus_px, piminus_py, piminus_pz;
  std::string piminus0_px, piminus0_py, piminus0_pz;
  std::string Kplus_px, Kplus_py, Kplus_pz;
  std::string Kminus_px, Kminus_py, Kminus_pz;

  std::string mass_hypo_constraints = "";
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_Dplus_PX")){
    Dplus_px    = std::get<0>(cfg)+"_FitPVConst_Dplus_PX"+flat_suffix;
    Dplus_py    = std::get<0>(cfg)+"_FitPVConst_Dplus_PY"+flat_suffix;
    Dplus_pz    = std::get<0>(cfg)+"_FitPVConst_Dplus_PZ"+flat_suffix;
    Dminus_px   = std::get<0>(cfg)+"_FitPVConst_Dminus_PX"+flat_suffix;
    Dminus_py   = std::get<0>(cfg)+"_FitPVConst_Dminus_PY"+flat_suffix;
    Dminus_pz   = std::get<0>(cfg)+"_FitPVConst_Dminus_PZ"+flat_suffix;
  }
  else if (_rdcr->LeafExists("Dplus_PX")){
    Dplus_px    = "Dplus_PX";
    Dplus_py    = "Dplus_PY";
    Dplus_pz    = "Dplus_PZ";
    Dminus_px   = "Dminus_PX";
    Dminus_py   = "Dminus_PY";
    Dminus_pz   = "Dminus_PZ";
  }
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_Dplus_P0_PX")){
    Kplus_px    = std::get<0>(cfg)+"_FitPVConst_Dminus_P2_PX"+flat_suffix;
    Kplus_py    = std::get<0>(cfg)+"_FitPVConst_Dminus_P2_PY"+flat_suffix;
    Kplus_pz    = std::get<0>(cfg)+"_FitPVConst_Dminus_P2_PZ"+flat_suffix;
    piminus_px  = std::get<0>(cfg)+"_FitPVConst_Dminus_P0_PX"+flat_suffix;
    piminus_py  = std::get<0>(cfg)+"_FitPVConst_Dminus_P0_PY"+flat_suffix;
    piminus_pz  = std::get<0>(cfg)+"_FitPVConst_Dminus_P0_PZ"+flat_suffix;
    piminus0_px = std::get<0>(cfg)+"_FitPVConst_Dminus_P1_PX"+flat_suffix;
    piminus0_py = std::get<0>(cfg)+"_FitPVConst_Dminus_P1_PY"+flat_suffix;
    piminus0_pz = std::get<0>(cfg)+"_FitPVConst_Dminus_P1_PZ"+flat_suffix;
    Kminus_px   = std::get<0>(cfg)+"_FitPVConst_Dplus_P2_PX"+flat_suffix;
    Kminus_py   = std::get<0>(cfg)+"_FitPVConst_Dplus_P2_PY"+flat_suffix;
    Kminus_pz   = std::get<0>(cfg)+"_FitPVConst_Dplus_P2_PZ"+flat_suffix;
    piplus_px   = std::get<0>(cfg)+"_FitPVConst_Dplus_P0_PX"+flat_suffix;
    piplus_py   = std::get<0>(cfg)+"_FitPVConst_Dplus_P0_PY"+flat_suffix;
    piplus_pz   = std::get<0>(cfg)+"_FitPVConst_Dplus_P0_PZ"+flat_suffix;
    piplus0_px  = std::get<0>(cfg)+"_FitPVConst_Dplus_P1_PX"+flat_suffix;
    piplus0_py  = std::get<0>(cfg)+"_FitPVConst_Dplus_P1_PY"+flat_suffix;
    piplus0_pz  = std::get<0>(cfg)+"_FitPVConst_Dplus_P1_PZ"+flat_suffix;
    mass_hypo_constraints = "PV";
  }
  else if (_rdcr->LeafExists("Dplus_Kminus_or_piminus_PX")){
    Kplus_px    = "Dminus_Kplus_or_piplus_PX";
    Kplus_py    = "Dminus_Kplus_or_piplus_PY";
    Kplus_pz    = "Dminus_Kplus_or_piplus_PZ";
    piminus_px  = "Dminus_piminus_or_Kminus_One_PX";
    piminus_py  = "Dminus_piminus_or_Kminus_One_PY";
    piminus_pz  = "Dminus_piminus_or_Kminus_One_PZ";
    piminus0_px = "Dminus_piminus_or_Kminus_Two_PX";
    piminus0_py = "Dminus_piminus_or_Kminus_Two_PY";
    piminus0_pz = "Dminus_piminus_or_Kminus_Two_PZ";
    Kminus_px   = "Dplus_Kminus_or_piminus_PX";
    Kminus_py   = "Dplus_Kminus_or_piminus_PY";
    Kminus_pz   = "Dplus_Kminus_or_piminus_PZ";
    piplus_px   = "Dplus_piplus_or_Kplus_One_PX";
    piplus_py   = "Dplus_piplus_or_Kplus_One_PY";
    piplus_pz   = "Dplus_piplus_or_Kplus_One_PZ";
    piplus0_px  = "Dplus_piplus_or_Kplus_Two_PX";
    piplus0_py  = "Dplus_piplus_or_Kplus_Two_PY";
    piplus0_pz  = "Dplus_piplus_or_Kplus_Two_PZ";
    mass_hypo_constraints = "NoConstr";
  }
  
  if (mass_hypo_constraints!=""){
    // mass hypotheses
    KinematicReducerLeaf<Double_t>* varKSMassHypo_DplusOne = new KinematicReducerLeaf<Double_t>("varKSMassHypo_DplusOne", "varKSMassHypo_DplusOne", "Double_t", NULL);
    varKSMassHypo_DplusOne->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varKSMassHypo_DplusOne);

    KinematicReducerLeaf<Double_t>* varKSMassHypo_DplusTwo = new KinematicReducerLeaf<Double_t>("varKSMassHypo_DplusTwo", "varKSMassHypo_DplusTwo", "Double_t", NULL);
    varKSMassHypo_DplusTwo->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varKSMassHypo_DplusTwo);

    KinematicReducerLeaf<Double_t>* varKSMassHypo_DminusOne = new KinematicReducerLeaf<Double_t>("varKSMassHypo_DminusOne", "varKSMassHypo_DminusOne", "Double_t", NULL);
    varKSMassHypo_DminusOne->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varKSMassHypo_DminusOne);

    KinematicReducerLeaf<Double_t>* varKSMassHypo_DminusTwo = new KinematicReducerLeaf<Double_t>("varKSMassHypo_DminusTwo", "varKSMassHypo_DminusTwo", "Double_t", NULL);
    varKSMassHypo_DminusTwo->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varKSMassHypo_DminusTwo);    

    KinematicReducerLeaf<Double_t>* varPhiMassHypo_DplusOne = new KinematicReducerLeaf<Double_t>("varPhiMassHypo_DplusOne", "varPhiMassHypo_DplusOne", "Double_t", NULL);
    varPhiMassHypo_DplusOne->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varPhiMassHypo_DplusOne);

    KinematicReducerLeaf<Double_t>* varPhiMassHypo_DplusTwo = new KinematicReducerLeaf<Double_t>("varPhiMassHypo_DplusTwo", "varPhiMassHypo_DplusTwo", "Double_t", NULL);
    varPhiMassHypo_DplusTwo->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varPhiMassHypo_DplusTwo);

    KinematicReducerLeaf<Double_t>* varPhiMassHypo_DminusOne = new KinematicReducerLeaf<Double_t>("varPhiMassHypo_DminusOne", "varPhiMassHypo_DminusOne", "Double_t", NULL);
    varPhiMassHypo_DminusOne->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varPhiMassHypo_DminusOne);

    KinematicReducerLeaf<Double_t>* varPhiMassHypo_DminusTwo = new KinematicReducerLeaf<Double_t>("varPhiMassHypo_DminusTwo", "varPhiMassHypo_DminusTwo", "Double_t", NULL);
    varPhiMassHypo_DminusTwo->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varPhiMassHypo_DminusTwo);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_Kpipi = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_Kpipi", "varDplusMassHypo_Kpipi", "Double_t", NULL);
    varDplusMassHypo_Kpipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_Kpipi);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_Kpipi = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_Kpipi", "varDminusMassHypo_Kpipi", "Double_t", NULL);
    varDminusMassHypo_Kpipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_Kpipi);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_KKpi = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_KKpi", "varDplusMassHypo_KKpi", "Double_t", NULL);
    varDplusMassHypo_KKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_KKpi);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_KKpi = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_KKpi", "varDminusMassHypo_KKpi", "Double_t", NULL);
    varDminusMassHypo_KKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_KKpi);

  KinematicReducerLeaf<Double_t>* varDplusMassHypo_KpiK = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_KpiK", "varDplusMassHypo_KpiK", "Double_t", NULL);
    varDplusMassHypo_KpiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_KpiK);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_KpiK = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_KpiK", "varDminusMassHypo_KpiK", "Double_t", NULL);
    varDminusMassHypo_KpiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_KpiK);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_KKK = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_KKK", "varDplusMassHypo_KKK", "Double_t", NULL);
    varDplusMassHypo_KKK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_KKK);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_KKK = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_KKK", "varDminusMassHypo_KKK", "Double_t", NULL);
    varDminusMassHypo_KKK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_KKK);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_pipipi = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_pipipi", "varDplusMassHypo_pipipi", "Double_t", NULL);
    varDplusMassHypo_pipipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_pipipi);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_pipipi = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_pipipi", "varDminusMassHypo_pipipi", "Double_t", NULL);
    varDminusMassHypo_pipipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_pipipi);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_pipiK = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_pipiK", "varDplusMassHypo_pipiK", "Double_t", NULL);
    varDplusMassHypo_pipiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_pipiK);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_pipiK = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_pipiK", "varDminusMassHypo_pipiK", "Double_t", NULL);
    varDminusMassHypo_pipiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_pipiK);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_piKpi = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_piKpi", "varDplusMassHypo_piKpi", "Double_t", NULL);
    varDplusMassHypo_piKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_piKpi);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_piKpi = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_piKpi", "varDminusMassHypo_piKpi", "Double_t", NULL);
    varDminusMassHypo_piKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_piKpi);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_piKK = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_piKK", "varDplusMassHypo_piKK", "Double_t", NULL);
    varDplusMassHypo_piKK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_piKK);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_piKK = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_piKK", "varDminusMassHypo_piKK", "Double_t", NULL);
    varDminusMassHypo_piKK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_piKK);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_Kppi = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_Kppi", "varDplusMassHypo_Kppi", "Double_t", NULL);
    varDplusMassHypo_Kppi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_Kppi);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_Kppi = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_Kppi", "varDminusMassHypo_Kppi", "Double_t", NULL);
    varDminusMassHypo_Kppi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_Kppi);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_Kpip = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_Kpip", "varDplusMassHypo_Kpip", "Double_t", NULL);
    varDplusMassHypo_Kpip->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        938.272046);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_Kpip);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_Kpip = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_Kpip", "varDminusMassHypo_Kpip", "Double_t", NULL);
    varDminusMassHypo_Kpip->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        938.272046);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_Kpip);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_pKpi = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_pKpi", "varDplusMassHypo_pKpi", "Double_t", NULL);
    varDplusMassHypo_pKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_pKpi);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_pKpi = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_pKpi", "varDminusMassHypo_pKpi", "Double_t", NULL);
    varDminusMassHypo_pKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_pKpi);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_ppiK = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_ppiK", "varDplusMassHypo_ppiK", "Double_t", NULL);
    varDplusMassHypo_ppiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_ppiK);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_ppiK = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_ppiK", "varDminusMassHypo_ppiK", "Double_t", NULL);
    varDminusMassHypo_ppiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_ppiK);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_pipK = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_pipK", "varDplusMassHypo_pipK", "Double_t", NULL);
    varDplusMassHypo_pipK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_pipK);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_pipK = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_pipK", "varDminusMassHypo_pipK", "Double_t", NULL);
    varDminusMassHypo_pipK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_pipK);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_piKp = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_piKp", "varDplusMassHypo_piKp", "Double_t", NULL);
    varDplusMassHypo_piKp->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        938.272046);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_piKp);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_piKp = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_piKp", "varDminusMassHypo_piKp", "Double_t", NULL);
    varDminusMassHypo_piKp->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        938.272046);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_piKp);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_ppipi = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_ppipi", "varDplusMassHypo_ppipi", "Double_t", NULL);
    varDplusMassHypo_ppipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_ppipi);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_ppipi = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_ppipi", "varDminusMassHypo_ppipi", "Double_t", NULL);
    varDminusMassHypo_ppipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_ppipi);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_pippi = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_pippi", "varDplusMassHypo_pippi", "Double_t", NULL);
    varDplusMassHypo_pippi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_pippi);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_pippi = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_pippi", "varDminusMassHypo_pippi", "Double_t", NULL);
    varDminusMassHypo_pippi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_pippi);

    KinematicReducerLeaf<Double_t>* varDplusMassHypo_pipip = new KinematicReducerLeaf<Double_t>("varDplusMassHypo_pipip", "varDplusMassHypo_pipip", "Double_t", NULL);
    varDplusMassHypo_pipip->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        938.272046);
    _rdcr->RegisterDoubleLeaf(varDplusMassHypo_pipip);

    KinematicReducerLeaf<Double_t>* varDminusMassHypo_pipip = new KinematicReducerLeaf<Double_t>("varDminusMassHypo_pipip", "varDminusMassHypo_pipip", "Double_t", NULL);
    varDminusMassHypo_pipip->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        938.272046);
    _rdcr->RegisterDoubleLeaf(varDminusMassHypo_pipip);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Dpluspipi = new KinematicReducerLeaf<Double_t>("varBMassHypo_Dpluspipi", "varBMassHypo_Dpluspipi", "Double_t", NULL);
    varBMassHypo_Dpluspipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Dplus_px),
        _rdcr->GetInterimLeafByName(Dplus_py),
        _rdcr->GetInterimLeafByName(Dplus_pz),
        1869.62,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_Dpluspipi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Dminuspipi = new KinematicReducerLeaf<Double_t>("varBMassHypo_Dminuspipi", "varBMassHypo_Dminuspipi", "Double_t", NULL);
    varBMassHypo_Dminuspipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Dminus_px),
        _rdcr->GetInterimLeafByName(Dminus_py),
        _rdcr->GetInterimLeafByName(Dminus_pz),
        1869.62,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_Dminuspipi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_DplusKpi = new KinematicReducerLeaf<Double_t>("varBMassHypo_DplusKpi", "varBMassHypo_DplusKpi", "Double_t", NULL);
    varBMassHypo_DplusKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Dplus_px),
        _rdcr->GetInterimLeafByName(Dplus_py),
        _rdcr->GetInterimLeafByName(Dplus_pz),
        1869.62,
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus_px),
        _rdcr->GetInterimLeafByName(piminus_py),
        _rdcr->GetInterimLeafByName(piminus_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_DplusKpi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_DminusKpi = new KinematicReducerLeaf<Double_t>("varBMassHypo_DminusKpi", "varBMassHypo_DminusKpi", "Double_t", NULL);
    varBMassHypo_DminusKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Dminus_px),
        _rdcr->GetInterimLeafByName(Dminus_py),
        _rdcr->GetInterimLeafByName(Dminus_pz),
        1869.62,
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus_px),
        _rdcr->GetInterimLeafByName(piplus_py),
        _rdcr->GetInterimLeafByName(piplus_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_DminusKpi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_DpluspiK = new KinematicReducerLeaf<Double_t>("varBMassHypo_DpluspiK", "varBMassHypo_DpluspiK", "Double_t", NULL);
    varBMassHypo_DpluspiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Dplus_px),
        _rdcr->GetInterimLeafByName(Dplus_py),
        _rdcr->GetInterimLeafByName(Dplus_pz),
        1869.62,
        _rdcr->GetInterimLeafByName(Kplus_px),
        _rdcr->GetInterimLeafByName(Kplus_py),
        _rdcr->GetInterimLeafByName(Kplus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piminus0_px),
        _rdcr->GetInterimLeafByName(piminus0_py),
        _rdcr->GetInterimLeafByName(piminus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_DpluspiK);

    KinematicReducerLeaf<Double_t>* varBMassHypo_DminuspiK = new KinematicReducerLeaf<Double_t>("varBMassHypo_DminuspiK", "varBMassHypo_DminuspiK", "Double_t", NULL);
    varBMassHypo_DminuspiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Dminus_px),
        _rdcr->GetInterimLeafByName(Dminus_py),
        _rdcr->GetInterimLeafByName(Dminus_pz),
        1869.62,
        _rdcr->GetInterimLeafByName(Kminus_px),
        _rdcr->GetInterimLeafByName(Kminus_py),
        _rdcr->GetInterimLeafByName(Kminus_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus0_px),
        _rdcr->GetInterimLeafByName(piplus0_py),
        _rdcr->GetInterimLeafByName(piplus0_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_DminuspiK);

    doocore::io::sinfo << "Veto leaves are filled using constrain: " << mass_hypo_constraints << doocore::io::endmsg;
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
  // final state
  ReducerLeaf<Int_t>& catDplusFinalState = _rdcr->CreateIntLeaf("catDplusFinalState", -10);
      TCut  Dplus_Kpipi = TCut(TString(std::get<0>(cfg)+"_FitPVConst_Dplus_P2_ID"+flat_suffix+"==-321&&"+std::get<0>(cfg)+"_FitPVConst_Dplus_P0_ID"+flat_suffix+"==211&&"+std::get<0>(cfg)+"_FitPVConst_Dplus_P1_ID"+flat_suffix+"==211"));
      TCut  Dplus_pipipi = TCut(TString(std::get<0>(cfg)+"_FitPVConst_Dplus_P2_ID"+flat_suffix+"==-211&&"+std::get<0>(cfg)+"_FitPVConst_Dplus_P0_ID"+flat_suffix+"==211&&"+std::get<0>(cfg)+"_FitPVConst_Dplus_P1_ID"+flat_suffix+"==211"));
      TCut  Dplus_KKpi = TCut(TString(std::get<0>(cfg)+"_FitPVConst_Dplus_P2_ID"+flat_suffix+"==-321&&"+std::get<0>(cfg)+"_FitPVConst_Dplus_P0_ID"+flat_suffix+"==321&&"+std::get<0>(cfg)+"_FitPVConst_Dplus_P1_ID"+flat_suffix+"==211"));
      TCut  Dplus_KpiK = TCut(TString(std::get<0>(cfg)+"_FitPVConst_Dplus_P2_ID"+flat_suffix+"==-321&&"+std::get<0>(cfg)+"_FitPVConst_Dplus_P0_ID"+flat_suffix+"==211&&"+std::get<0>(cfg)+"_FitPVConst_Dplus_P1_ID"+flat_suffix+"==321"));
      catDplusFinalState.AddCondition("Kpipi",Dplus_Kpipi.GetTitle(),1);
      catDplusFinalState.AddCondition("pipipi",Dplus_pipipi.GetTitle(),2);
      catDplusFinalState.AddCondition("KKpi",Dplus_KKpi.GetTitle(),3);
      catDplusFinalState.AddCondition("KpiK",Dplus_KpiK.GetTitle(),4);

  ReducerLeaf<Int_t>& catDminusFinalState = _rdcr->CreateIntLeaf("catDminusFinalState", -10);
      TCut  Dminus_Kpipi = TCut(TString(std::get<0>(cfg)+"_FitPVConst_Dminus_P2_ID"+flat_suffix+"==321&&"+std::get<0>(cfg)+"_FitPVConst_Dminus_P0_ID"+flat_suffix+"==-211&&"+std::get<0>(cfg)+"_FitPVConst_Dminus_P1_ID"+flat_suffix+"==-211"));
      TCut  Dminus_pipipi = TCut(TString(std::get<0>(cfg)+"_FitPVConst_Dminus_P2_ID"+flat_suffix+"==211&&"+std::get<0>(cfg)+"_FitPVConst_Dminus_P0_ID"+flat_suffix+"==-211&&"+std::get<0>(cfg)+"_FitPVConst_Dminus_P1_ID"+flat_suffix+"==-211"));
      TCut  Dminus_KKpi = TCut(TString(std::get<0>(cfg)+"_FitPVConst_Dminus_P2_ID"+flat_suffix+"==321&&"+std::get<0>(cfg)+"_FitPVConst_Dminus_P0_ID"+flat_suffix+"==-321&&"+std::get<0>(cfg)+"_FitPVConst_Dminus_P1_ID"+flat_suffix+"==-211"));
      TCut  Dminus_KpiK = TCut(TString(std::get<0>(cfg)+"_FitPVConst_Dminus_P2_ID"+flat_suffix+"==321&&"+std::get<0>(cfg)+"_FitPVConst_Dminus_P0_ID"+flat_suffix+"==-211&&"+std::get<0>(cfg)+"_FitPVConst_Dminus_P1_ID"+flat_suffix+"==-321"));
      catDminusFinalState.AddCondition("Kpipi",Dminus_Kpipi.GetTitle(),1);
      catDminusFinalState.AddCondition("pipipi",Dminus_pipipi.GetTitle(),2);
      catDminusFinalState.AddCondition("KKpi",Dminus_KKpi.GetTitle(),3);
      catDminusFinalState.AddCondition("KpiK",Dminus_KpiK.GetTitle(),4);

  ReducerLeaf<Int_t>& catDDFinalState = _rdcr->CreateIntLeaf("catDDFinalState", -10);
      catDDFinalState.AddCondition("KpipiKpipi", TString(Dplus_Kpipi && Dminus_Kpipi), 1);
      catDDFinalState.AddCondition("Kpipipipipi", TString(Dplus_Kpipi && Dminus_pipipi), 2);
      catDDFinalState.AddCondition("pipipiKpipi", TString(Dplus_pipipi && Dminus_Kpipi), 3);
      catDDFinalState.AddCondition("KpipiKKpi", TString(Dplus_Kpipi && Dminus_KKpi), 4);
      catDDFinalState.AddCondition("KKpiKpipi", TString(Dplus_KKpi && Dminus_Kpipi), 5);
      catDDFinalState.AddCondition("pipipipipipi", TString(Dplus_pipipi && Dminus_pipipi), 6);
      catDDFinalState.AddCondition("pipipiKKpi", TString(Dplus_pipipi && Dminus_KKpi), 7);
      catDDFinalState.AddCondition("KKpipipipi", TString(Dplus_KKpi && Dminus_pipipi), 8);
      catDDFinalState.AddCondition("KKpiKKpi", TString(Dplus_KKpi && Dminus_KKpi), 9);

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
  _rdcr->CreateDoubleCopyLeaf("varKplusTrackGhostProb", _rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varKminusTrackGhostProb", _rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_TRACK_GhostProb"));
  _rdcr->CreateDoubleLeaf("varKaonMaxTrackGhostProb", -999999.).Maximum(_rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_TRACK_GhostProb"), _rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varPiplusOneTrackGhostProb", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_One_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varPiplusTwoTrackGhostProb", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_Two_TRACK_GhostProb"));
  ReducerLeaf<Double_t>& varPiplusMaxTrackGhostProb = _rdcr->CreateDoubleLeaf("varPiplusMaxTrackGhostProb", -999999.);
  varPiplusMaxTrackGhostProb.Maximum(_rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_One_TRACK_GhostProb"), _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_Two_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varPiminusOneTrackGhostProb", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_One_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varPiminusTwoTrackGhostProb", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_Two_TRACK_GhostProb"));
  ReducerLeaf<Double_t>& varPiminusMaxTrackGhostProb = _rdcr->CreateDoubleLeaf("varPiminusMaxTrackGhostProb", -999999.);
  varPiminusMaxTrackGhostProb.Maximum(_rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_One_TRACK_GhostProb"), _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_Two_TRACK_GhostProb"));
  _rdcr->CreateDoubleLeaf("varPionMaxTrackGhostProb", -999999.).Maximum(varPiplusMaxTrackGhostProb, varPiminusMaxTrackGhostProb);

  // Primary vertex chi2/ndof
  ReducerLeaf<Double_t>& varPVVtxChi2ndof = _rdcr->CreateDoubleLeaf("varPVVtxChi2ndof", -99999999.);
  varPVVtxChi2ndof.Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_OWNPV_CHI2"), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_OWNPV_NDOF"));
  _rdcr->CreateDoubleLeaf("varPVVtxChi2ndof_log", -999999.).Log(varPVVtxChi2ndof);
  // Separation from PV
  _rdcr->CreateDoubleLeaf("varDMinVtxSepChi2", -99999999.).Minimum(_rdcr->GetInterimLeafByName("Dplus_LOKI_VertexSeparation_CHI2"), _rdcr->GetInterimLeafByName("Dminus_LOKI_VertexSeparation_CHI2"));
  _rdcr->CreateDoubleLeaf("varDMaxVtxSepChi2", -99999999.).Maximum(_rdcr->GetInterimLeafByName("Dplus_LOKI_VertexSeparation_CHI2"), _rdcr->GetInterimLeafByName("Dminus_LOKI_VertexSeparation_CHI2"));
  // End vertex chi2/ndof
  ReducerLeaf<Double_t>& varBEndVtxChi2ndof = _rdcr->CreateDoubleLeaf("varBEndVtxChi2ndof", -99999999.);
  varBEndVtxChi2ndof.Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_ENDVERTEX_NDOF"));
  _rdcr->CreateDoubleLeaf("varBEndVtxChi2ndof_log", -999999.).Log(varBEndVtxChi2ndof);
  ReducerLeaf<Double_t>& varDplusEndVtxChi2ndof = _rdcr->CreateDoubleLeaf("varDplusEndVtxChi2ndof", -99999999.);
  varDplusEndVtxChi2ndof.Divide(_rdcr->GetInterimLeafByName("Dplus_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName("Dplus_ENDVERTEX_NDOF"));
  ReducerLeaf<Double_t>& varDminusEndVtxChi2ndof = _rdcr->CreateDoubleLeaf("varDminusEndVtxChi2ndof", -99999999.);
  varDminusEndVtxChi2ndof.Divide(_rdcr->GetInterimLeafByName("Dminus_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName("Dminus_ENDVERTEX_NDOF"));
  _rdcr->CreateDoubleLeaf("varDMaxEndVtxChi2ndof", -999999.).Maximum(varDplusEndVtxChi2ndof, varDminusEndVtxChi2ndof);
  // sum of D transverse momenta
  _rdcr->CreateDoubleLeaf("varDDDTFSumPT", -99999999.).Add(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_PT"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_PT"+flat_suffix));
  // maximal kaon track fit chi2ndof
  _rdcr->CreateDoubleLeaf("varKaonMaxTrackFitChi2ndof", -999999.).Maximum(_rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_TRACK_CHI2NDOF"), _rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_TRACK_CHI2NDOF"));
  // maximal pion track fit chi2ndof
  ReducerLeaf<Double_t>& piplus_max_trackchi2 = _rdcr->CreateDoubleLeaf("varPiplusMaxTrackFitChi2ndof", -999999.);
  piplus_max_trackchi2.Maximum(_rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_One_TRACK_CHI2NDOF"), _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_Two_TRACK_CHI2NDOF"));
  ReducerLeaf<Double_t>& piminus_max_trackchi2 = _rdcr->CreateDoubleLeaf("varPiminusMaxTrackFitChi2ndof", -999999.);
  piminus_max_trackchi2.Maximum(_rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_One_TRACK_CHI2NDOF"), _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_Two_TRACK_CHI2NDOF"));
  _rdcr->CreateDoubleLeaf("varPionMaxTrackFitChi2ndof", -999999.).Maximum(piplus_max_trackchi2, piminus_max_trackchi2);
  // sum of D daughters transverse momentum
  ReducerLeaf<Double_t>& dplus_sum_pions_pt = _rdcr->CreateDoubleLeaf("varDplusPionsSumPT", -999999.);
  dplus_sum_pions_pt.Add(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_P0_PT"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_P1_PT"+flat_suffix));
  ReducerLeaf<Double_t>& dplus_sum_daughters_pt = _rdcr->CreateDoubleLeaf("varDplusDaughtersSumPT", -999999.);
  dplus_sum_daughters_pt.Add(dplus_sum_pions_pt, _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_P2_PT"+flat_suffix));
  ReducerLeaf<Double_t>& dminus_sum_pions_pt = _rdcr->CreateDoubleLeaf("varDminusPionsSumPT", -999999.);
  dminus_sum_pions_pt.Add(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_P0_PT"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_P1_PT"+flat_suffix));
  ReducerLeaf<Double_t>& dminus_sum_daughters_pt = _rdcr->CreateDoubleLeaf("varDminusDaughtersSumPT", -999999.);
  dminus_sum_daughters_pt.Add(dminus_sum_pions_pt, _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dminus_P2_PT"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varDMinDaughtersSumPT", -999999.).Minimum(dplus_sum_daughters_pt, dminus_sum_daughters_pt);

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
  _rdcr->CreateDoubleLeaf("varDTFChi2ndof_log", -999999.).Log(*dtf_chi2ndof_leaf_ptr);

  // IP chi2
  ReducerLeaf<Double_t>* ip_chi2_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitPVConst_IPCHI2"+flat_suffix)) {
    ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_IPCHI2"+flat_suffix));
  } else if (_rdcr->LeafExists("B0_IPCHI2_OWNPV")) {
    ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varIPChi2", _rdcr->GetInterimLeafByName("B0_IPCHI2_OWNPV"));
  }
  _rdcr->CreateDoubleLeaf("varIPChi2_log", -999999.).Log(*ip_chi2_leaf_ptr);
  ReducerLeaf<Double_t>* dplus_ip_chi2_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* dminus_ip_chi2_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitPVConst_Dplus_IPCHI2"+flat_suffix)) {
    dplus_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplusIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_IPCHI2"+flat_suffix));
    dminus_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDminusIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_IPCHI2"+flat_suffix));
  } else if (_rdcr->LeafExists("Dplus_IPCHI2_OWNPV")) {
    dplus_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplusIPChi2", _rdcr->GetInterimLeafByName("Dplus_IPCHI2_OWNPV"));
    dminus_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDminusIPChi2", _rdcr->GetInterimLeafByName("Dminus_IPCHI2_OWNPV"));
  }
  _rdcr->CreateDoubleLeaf("varDIPChi2Minimum", -999999.).Minimum(*dplus_ip_chi2_leaf_ptr, *dminus_ip_chi2_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varDIPChi2Maximum", -999999.).Maximum(*dplus_ip_chi2_leaf_ptr, *dminus_ip_chi2_leaf_ptr);
  ReducerLeaf<Double_t>* Kplus_ip_chi2_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* piminus_ip_chi2_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* piminus0_ip_chi2_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* Kminus_ip_chi2_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* piplus_ip_chi2_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* piplus0_ip_chi2_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitPVConst_Dminus_P0_IPCHI2"+flat_suffix)) {
    piminus_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPiminusOneIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P0_IPCHI2"+flat_suffix));
    piminus0_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPiminusTwoIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P1_IPCHI2"+flat_suffix));
    Kplus_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varKplusIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P2_IPCHI2"+flat_suffix));
    piplus_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPiplusOneIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_IPCHI2"+flat_suffix));
    piplus0_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPiplusTwoIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_IPCHI2"+flat_suffix));
    Kminus_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varKminusIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_IPCHI2"+flat_suffix));
  } else if (_rdcr->LeafExists("Dplus_IPCHI2_OWNPV")) {
    piminus_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPiminusOneIPChi2", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_One_IPCHI2_OWNPV"));
    piminus0_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPiminusTwoIPChi2", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_Two_IPCHI2_OWNPV"));
    Kplus_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varKplusIPChi2", _rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_IPCHI2_OWNPV"));
    piplus_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPiplusOneIPChi2", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_One_IPCHI2_OWNPV"));
    piplus0_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPiplusTwoIPChi2", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_Two_IPCHI2_OWNPV"));
    Kminus_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varKminusIPChi2", _rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_IPCHI2_OWNPV"));
  }
  _rdcr->CreateDoubleLeaf("varKaonIPChi2Minimum", -999999.).Minimum(*Kplus_ip_chi2_leaf_ptr, *Kminus_ip_chi2_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varPionOneIPChi2Minimum", -999999.).Minimum(*piplus_ip_chi2_leaf_ptr, *piminus_ip_chi2_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varPionTwoIPChi2Minimum", -999999.).Minimum(*piplus0_ip_chi2_leaf_ptr, *piminus0_ip_chi2_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varKaonIPChi2Maximum", -999999.).Maximum(*Kplus_ip_chi2_leaf_ptr, *Kminus_ip_chi2_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varPionOneIPChi2Maximum", -999999.).Maximum(*piplus_ip_chi2_leaf_ptr, *piminus_ip_chi2_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varPionTwoIPChi2Maximum", -999999.).Maximum(*piplus0_ip_chi2_leaf_ptr, *piminus0_ip_chi2_leaf_ptr);

  // Direction angle (DIRA)
  ReducerLeaf<Double_t>* dira_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* dplus_dira_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* dminus_dira_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitPVConst_DIRA"+flat_suffix)) {
    dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varBDTFDIRA", _rdcr->GetInterimLeafByName("B0_FitPVConst_DIRA"+flat_suffix));
    dplus_dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplusDTFDIRA", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_DIRA"+flat_suffix));
    dminus_dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDminusDTFDIRA", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_DIRA"+flat_suffix));
  } else if (_rdcr->LeafExists("B0_DIRA_OWNPV")) {
    dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varBDIRAOwnPV", _rdcr->GetInterimLeafByName("B0_DIRA_OWNPV"));
    dplus_dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplusDIRAOwnPV", _rdcr->GetInterimLeafByName("Dplus_DIRA_OWNPV"));
    dminus_dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDminusDIRAOwnPV", _rdcr->GetInterimLeafByName("Dminus_DIRA_OWNPV"));
  }
  _rdcr->CreateDoubleLeaf("varDMinDIRA", -999999.).Minimum(*dplus_dira_leaf_ptr, *dminus_dira_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varDMaxDIRA", -999999.).Maximum(*dplus_dira_leaf_ptr, *dminus_dira_leaf_ptr);

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
  _rdcr->CreateDoubleLeaf("varDMinP", -999999.).Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varDMinPT", -999999.).Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_PT"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varDMaxP", -999999.).Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varDMaxPT", -999999.).Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_PT"+flat_suffix));
  // grand-daughters
  _rdcr->CreateDoubleCopyLeaf("varPiplusOneP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPiplusOnePT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPiplusTwoP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPiplusTwoPT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPiminusOneP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P0_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPiminusOnePT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P0_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPiminusTwoP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P1_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPiminusTwoPT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P1_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varKplusP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P2_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varKplusPT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P2_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varKminusP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varKminusPT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_PT"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varPionOneMinP", -999999.).Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P0_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_P"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varPionTwoMinP", -999999.).Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P1_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_P"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varKaonMinP", -999999.).Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P2_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_P"+flat_suffix));
  ReducerLeaf<Double_t>& varPionOneMinPT = _rdcr->CreateDoubleLeaf("varPionOneMinPT", -999999.);
  varPionOneMinPT.Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P0_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_PT"+flat_suffix));
  ReducerLeaf<Double_t>& varPionTwoMinPT = _rdcr->CreateDoubleLeaf("varPionTwoMinPT", -999999.);
  varPionTwoMinPT.Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P1_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_PT"+flat_suffix));
  ReducerLeaf<Double_t>& varPionMinPT = _rdcr->CreateDoubleLeaf("varPionMinPT", -999999.);
  varPionMinPT.Minimum(varPionOneMinPT, varPionTwoMinPT);
  ReducerLeaf<Double_t>& varKaonMinPT = _rdcr->CreateDoubleLeaf("varKaonMinPT", -999999.);
  varKaonMinPT.Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P2_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_PT"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varHadronMinPT", -999999.).Minimum(varPionMinPT, varKaonMinPT);
  _rdcr->CreateDoubleLeaf("varPionOneMaxP", -999999.).Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P0_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_P"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varPionTwoMaxP", -999999.).Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P1_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_P"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varKaonMaxP", -999999.).Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P2_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_P"+flat_suffix));
  ReducerLeaf<Double_t>& varPionOneMaxPT = _rdcr->CreateDoubleLeaf("varPionOneMaxPT", -999999.);
  varPionOneMaxPT.Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P0_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_PT"+flat_suffix));
  ReducerLeaf<Double_t>& varPionTwoMaxPT = _rdcr->CreateDoubleLeaf("varPionTwoMaxPT", -999999.);
  varPionTwoMaxPT.Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P1_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_PT"+flat_suffix));
  ReducerLeaf<Double_t>& varPionMaxPT = _rdcr->CreateDoubleLeaf("varPionMaxPT", -999999.);
  varPionMaxPT.Maximum(varPionOneMaxPT, varPionTwoMaxPT);
  ReducerLeaf<Double_t>& varKaonMaxPT = _rdcr->CreateDoubleLeaf("varKaonMaxPT", -999999.);
  varKaonMaxPT.Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dminus_P2_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_PT"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varHadronMaxPT", -999999.).Maximum(varPionMaxPT, varKaonMaxPT);

  // Velo track quality
  _rdcr->CreateDoubleCopyLeaf("varKminusVELOChi2ndof", _rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_TRACK_VeloCHI2NDOF"));
  _rdcr->CreateDoubleCopyLeaf("varKplusVELOChi2ndof", _rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_TRACK_VeloCHI2NDOF"));
  ReducerLeaf<Double_t>& varKaonMinVELOChi2ndof = _rdcr->CreateDoubleLeaf("varKaonMinVELOChi2ndof", -999999.);
  varKaonMinVELOChi2ndof.Minimum(_rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_TRACK_VeloCHI2NDOF"), _rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_TRACK_VeloCHI2NDOF"));
  _rdcr->CreateDoubleLeaf("varKaonMinVELOChi2ndof_log", -999999.).Log(varKaonMinVELOChi2ndof);
  ReducerLeaf<Double_t>& varKaonMaxVELOChi2ndof = _rdcr->CreateDoubleLeaf("varKaonMaxVELOChi2ndof", -999999.);
  varKaonMaxVELOChi2ndof.Maximum(_rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_TRACK_VeloCHI2NDOF"), _rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_TRACK_VeloCHI2NDOF"));
  _rdcr->CreateDoubleLeaf("varKaonMaxVELOChi2ndof_log", -999999.).Log(varKaonMaxVELOChi2ndof);
  ReducerLeaf<Double_t>& varPiplusOneVELOChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPiplusOneVELOChi2ndof", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_One_TRACK_VeloCHI2NDOF"));
  ReducerLeaf<Double_t>& varPiplusTwoVELOChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPiplusTwoVELOChi2ndof", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_Two_TRACK_VeloCHI2NDOF"));
  ReducerLeaf<Double_t>& varPiplusMinVELOChi2ndof = _rdcr->CreateDoubleLeaf("varPiplusMinVELOChi2ndof", -999999.);
  varPiplusMinVELOChi2ndof.Minimum(varPiplusOneVELOChi2ndof, varPiplusTwoVELOChi2ndof);
  ReducerLeaf<Double_t>& varPiplusMaxVELOChi2ndof = _rdcr->CreateDoubleLeaf("varPiplusMaxVELOChi2ndof", -999999.);
  varPiplusMaxVELOChi2ndof.Maximum(varPiplusOneVELOChi2ndof, varPiplusTwoVELOChi2ndof);
  ReducerLeaf<Double_t>& varPiminusOneVELOChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPiminusOneVELOChi2ndof", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_One_TRACK_VeloCHI2NDOF"));
  ReducerLeaf<Double_t>& varPiminusTwoVELOChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPiminusTwoVELOChi2ndof", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_Two_TRACK_VeloCHI2NDOF"));
  ReducerLeaf<Double_t>& varPiminusMinVELOChi2ndof = _rdcr->CreateDoubleLeaf("varPiminusMinVELOChi2ndof", -999999.);
  varPiminusMinVELOChi2ndof.Minimum(varPiminusOneVELOChi2ndof, varPiminusTwoVELOChi2ndof);
  ReducerLeaf<Double_t>& varPionMinVELOChi2ndof = _rdcr->CreateDoubleLeaf("varPionMinVELOChi2ndof", -999999.);
  varPionMinVELOChi2ndof.Minimum(varPiplusMinVELOChi2ndof, varPiminusMinVELOChi2ndof);
  _rdcr->CreateDoubleLeaf("varPionMinVELOChi2ndof_log", -999999.).Log(varPionMinVELOChi2ndof);
  ReducerLeaf<Double_t>& varPiminusMaxVELOChi2ndof = _rdcr->CreateDoubleLeaf("varPiminusMaxVELOChi2ndof", -999999.);
  varPiminusMaxVELOChi2ndof.Maximum(varPiminusOneVELOChi2ndof, varPiminusTwoVELOChi2ndof);
  ReducerLeaf<Double_t>& varPionMaxVELOChi2ndof = _rdcr->CreateDoubleLeaf("varPionMaxVELOChi2ndof", -999999.);
  varPionMaxVELOChi2ndof.Maximum(varPiplusMaxVELOChi2ndof, varPiminusMaxVELOChi2ndof);
  _rdcr->CreateDoubleLeaf("varPionMaxVELOChi2ndof_log", -999999.).Log(varPionMaxVELOChi2ndof);
  ReducerLeaf<Double_t>& varHadronMinVELOChi2ndof = _rdcr->CreateDoubleLeaf("varHadronMinVELOChi2ndof", -999999.);
  varHadronMinVELOChi2ndof.Minimum(varKaonMinVELOChi2ndof, varPionMinVELOChi2ndof);
  _rdcr->CreateDoubleLeaf("varHadronMinVELOChi2ndof_log", -999999.).Log(varHadronMinVELOChi2ndof);
  ReducerLeaf<Double_t>& varHadronMaxVELOChi2ndof = _rdcr->CreateDoubleLeaf("varHadronMaxVELOChi2ndof", -999999.);
  varHadronMaxVELOChi2ndof.Minimum(varKaonMaxVELOChi2ndof, varPionMaxVELOChi2ndof);
  _rdcr->CreateDoubleLeaf("varHadronMaxVELOChi2ndof_log", -999999.).Log(varHadronMaxVELOChi2ndof);

  // T-stations track quality
  _rdcr->CreateDoubleCopyLeaf("varKminusTChi2ndof", _rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_TRACK_TCHI2NDOF"));
  _rdcr->CreateDoubleCopyLeaf("varKplusTChi2ndof", _rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_TRACK_TCHI2NDOF"));
  ReducerLeaf<Double_t>& varKaonMinTChi2ndof = _rdcr->CreateDoubleLeaf("varKaonMinTChi2ndof", -999999.);
  varKaonMinTChi2ndof.Minimum(_rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_TRACK_TCHI2NDOF"), _rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_TRACK_TCHI2NDOF"));
  _rdcr->CreateDoubleLeaf("varKaonMinTChi2ndof_log", -999999.).Log(varKaonMinTChi2ndof);
  ReducerLeaf<Double_t>& varKaonMaxTChi2ndof = _rdcr->CreateDoubleLeaf("varKaonMaxTChi2ndof", -999999.);
  varKaonMaxTChi2ndof.Maximum(_rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_TRACK_TCHI2NDOF"), _rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_TRACK_TCHI2NDOF"));
  _rdcr->CreateDoubleLeaf("varKaonMaxTChi2ndof_log", -999999.).Log(varKaonMaxTChi2ndof);
  ReducerLeaf<Double_t>& varPiplusOneTChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPiplusOneTChi2ndof", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_One_TRACK_TCHI2NDOF"));
  ReducerLeaf<Double_t>& varPiplusTwoTChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPiplusTwoTChi2ndof", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_Two_TRACK_TCHI2NDOF"));
  ReducerLeaf<Double_t>& varPiplusMinTChi2ndof = _rdcr->CreateDoubleLeaf("varPiplusMinTChi2ndof", -999999.);
  varPiplusMinTChi2ndof.Minimum(varPiplusOneTChi2ndof, varPiplusTwoTChi2ndof);
  ReducerLeaf<Double_t>& varPiplusMaxTChi2ndof = _rdcr->CreateDoubleLeaf("varPiplusMaxTChi2ndof", -999999.);
  varPiplusMaxTChi2ndof.Maximum(varPiplusOneTChi2ndof, varPiplusTwoTChi2ndof);
  ReducerLeaf<Double_t>& varPiminusOneTChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPiminusOneTChi2ndof", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_One_TRACK_TCHI2NDOF"));
  ReducerLeaf<Double_t>& varPiminusTwoTChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPiminusTwoTChi2ndof", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_Two_TRACK_TCHI2NDOF"));
  ReducerLeaf<Double_t>& varPiminusMinTChi2ndof = _rdcr->CreateDoubleLeaf("varPiminusMinTChi2ndof", -999999.);
  varPiminusMinTChi2ndof.Minimum(varPiminusOneTChi2ndof, varPiminusTwoTChi2ndof);
  ReducerLeaf<Double_t>& varPionMinTChi2ndof = _rdcr->CreateDoubleLeaf("varPionMinTChi2ndof", -999999.);
  varPionMinTChi2ndof.Minimum(varPiplusMinTChi2ndof, varPiminusMinTChi2ndof);
  _rdcr->CreateDoubleLeaf("varPionMinTChi2ndof_log", -999999.).Log(varPionMinTChi2ndof);
  ReducerLeaf<Double_t>& varPiminusMaxTChi2ndof = _rdcr->CreateDoubleLeaf("varPiminusMaxTChi2ndof", -999999.);
  varPiminusMaxTChi2ndof.Maximum(varPiminusOneTChi2ndof, varPiminusTwoTChi2ndof);
  ReducerLeaf<Double_t>& varPionMaxTChi2ndof = _rdcr->CreateDoubleLeaf("varPionMaxTChi2ndof", -999999.);
  varPionMaxTChi2ndof.Maximum(varPiplusMaxTChi2ndof, varPiminusMaxTChi2ndof);
  _rdcr->CreateDoubleLeaf("varPionMaxTChi2ndof_log", -999999.).Log(varPionMaxTChi2ndof);
  ReducerLeaf<Double_t>& varHadronMinTChi2ndof = _rdcr->CreateDoubleLeaf("varHadronMinTChi2ndof", -999999.);
  varHadronMinTChi2ndof.Minimum(varKaonMinTChi2ndof, varPionMinTChi2ndof);
  _rdcr->CreateDoubleLeaf("varHadronMinTChi2ndof_log", -999999.).Log(varHadronMinTChi2ndof);
  ReducerLeaf<Double_t>& varHadronMaxTChi2ndof = _rdcr->CreateDoubleLeaf("varHadronMaxTChi2ndof", -999999.);
  varHadronMaxTChi2ndof.Minimum(varKaonMaxTChi2ndof, varPionMaxTChi2ndof);
  _rdcr->CreateDoubleLeaf("varHadronMaxTChi2ndof_log", -999999.).Log(varHadronMaxTChi2ndof);

  // track match quality
  _rdcr->CreateDoubleCopyLeaf("varKminusMatchChi2", _rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_TRACK_MatchCHI2"));
  _rdcr->CreateDoubleCopyLeaf("varKplusMatchChi2", _rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_TRACK_MatchCHI2"));
  ReducerLeaf<Double_t>& varKaonMinMatchChi2 = _rdcr->CreateDoubleLeaf("varKaonMinMatchChi2", -999999.);
  varKaonMinMatchChi2.Minimum(_rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_TRACK_MatchCHI2"), _rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_TRACK_MatchCHI2"));
  _rdcr->CreateDoubleLeaf("varKaonMinMatchChi2_log", -999999.).Log(varKaonMinMatchChi2);
  ReducerLeaf<Double_t>& varKaonMaxMatchChi2 = _rdcr->CreateDoubleLeaf("varKaonMaxMatchChi2", -999999.);
  varKaonMaxMatchChi2.Maximum(_rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_TRACK_MatchCHI2"), _rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_TRACK_MatchCHI2"));
  _rdcr->CreateDoubleLeaf("varKaonMaxMatchChi2_log", -999999.).Log(varKaonMaxMatchChi2);
  ReducerLeaf<Double_t>& varPiplusOneMatchChi2 = _rdcr->CreateDoubleCopyLeaf("varPiplusOneMatchChi2", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_One_TRACK_MatchCHI2"));
  ReducerLeaf<Double_t>& varPiplusTwoMatchChi2 = _rdcr->CreateDoubleCopyLeaf("varPiplusTwoMatchChi2", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_Two_TRACK_MatchCHI2"));
  ReducerLeaf<Double_t>& varPiplusMinMatchChi2 = _rdcr->CreateDoubleLeaf("varPiplusMinMatchChi2", -999999.);
  varPiplusMinMatchChi2.Minimum(varPiplusOneMatchChi2, varPiplusTwoMatchChi2);
  ReducerLeaf<Double_t>& varPiplusMaxMatchChi2 = _rdcr->CreateDoubleLeaf("varPiplusMaxMatchChi2", -999999.);
  varPiplusMaxMatchChi2.Maximum(varPiplusOneMatchChi2, varPiplusTwoMatchChi2);
  ReducerLeaf<Double_t>& varPiminusOneMatchChi2 = _rdcr->CreateDoubleCopyLeaf("varPiminusOneMatchChi2", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_One_TRACK_MatchCHI2"));
  ReducerLeaf<Double_t>& varPiminusTwoMatchChi2 = _rdcr->CreateDoubleCopyLeaf("varPiminusTwoMatchChi2", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_Two_TRACK_MatchCHI2"));
  ReducerLeaf<Double_t>& varPiminusMinMatchChi2 = _rdcr->CreateDoubleLeaf("varPiminusMinMatchChi2", -999999.);
  varPiminusMinMatchChi2.Minimum(varPiminusOneMatchChi2, varPiminusTwoMatchChi2);
  ReducerLeaf<Double_t>& varPionMinMatchChi2 = _rdcr->CreateDoubleLeaf("varPionMinMatchChi2", -999999.);
  varPionMinMatchChi2.Minimum(varPiplusMinMatchChi2, varPiminusMinMatchChi2);
  _rdcr->CreateDoubleLeaf("varPionMinMatchChi2_log", -999999.).Log(varPionMinMatchChi2);
  ReducerLeaf<Double_t>& varPiminusMaxMatchChi2 = _rdcr->CreateDoubleLeaf("varPiminusMaxMatchChi2", -999999.);
  varPiminusMaxMatchChi2.Maximum(varPiminusOneMatchChi2, varPiminusTwoMatchChi2);
  ReducerLeaf<Double_t>& varPionMaxMatchChi2 = _rdcr->CreateDoubleLeaf("varPionMaxMatchChi2", -999999.);
  varPionMaxMatchChi2.Maximum(varPiplusMaxMatchChi2, varPiminusMaxMatchChi2);
  _rdcr->CreateDoubleLeaf("varPionMaxMatchChi2_log", -999999.).Log(varPionMaxMatchChi2);
  ReducerLeaf<Double_t>& varHadronMinMatchChi2 = _rdcr->CreateDoubleLeaf("varHadronMinMatchChi2", -999999.);
  varHadronMinMatchChi2.Minimum(varKaonMinMatchChi2, varPionMinMatchChi2);
  _rdcr->CreateDoubleLeaf("varHadronMinMatchChi2_log", -999999.).Log(varHadronMinMatchChi2);
  ReducerLeaf<Double_t>& varHadronMaxMatchChi2 = _rdcr->CreateDoubleLeaf("varHadronMaxMatchChi2", -999999.);
  varHadronMaxMatchChi2.Minimum(varKaonMaxMatchChi2, varPionMaxMatchChi2);
  _rdcr->CreateDoubleLeaf("varHadronMaxMatchChi2_log", -999999.).Log(varHadronMaxMatchChi2);

  // PID variables
  ReducerLeaf<Double_t>& varKminus_ProbNNk = _rdcr->CreateDoubleCopyLeaf("varKminus_ProbNNk", _rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_ProbNNk"));
  ReducerLeaf<Double_t>& varKminus_ProbNNpi = _rdcr->CreateDoubleCopyLeaf("varKminus_ProbNNpi", _rdcr->GetInterimLeafByName("Dplus_Kminus_or_piminus_ProbNNpi"));
  ReducerLeaf<Double_t>& varKminus_SumProbNNkpi = _rdcr->CreateDoubleLeaf("varKminus_SumProbNNkpi", -999999.);
  varKminus_SumProbNNkpi.Add(varKminus_ProbNNk, varKminus_ProbNNpi);
  _rdcr->CreateDoubleLeaf("varKminus_PID", -999999.).Divide(varKminus_ProbNNk, varKminus_SumProbNNkpi);
  ReducerLeaf<Double_t>& varKplus_ProbNNk = _rdcr->CreateDoubleCopyLeaf("varKplus_ProbNNk", _rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_ProbNNk"));
  ReducerLeaf<Double_t>& varKplus_ProbNNpi = _rdcr->CreateDoubleCopyLeaf("varKplus_ProbNNpi", _rdcr->GetInterimLeafByName("Dminus_Kplus_or_piplus_ProbNNpi"));
  ReducerLeaf<Double_t>& varKplus_SumProbNNkpi = _rdcr->CreateDoubleLeaf("varKplus_SumProbNNkpi", -999999.);
  varKplus_SumProbNNkpi.Add(varKplus_ProbNNk, varKplus_ProbNNpi);
  _rdcr->CreateDoubleLeaf("varKplus_PID", -999999.).Divide(varKplus_ProbNNk, varKplus_SumProbNNkpi);
  ReducerLeaf<Double_t>& varPiOneminus_ProbNNk = _rdcr->CreateDoubleCopyLeaf("varPiOneminus_ProbNNk", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_One_ProbNNk"));
  ReducerLeaf<Double_t>& varPiOneminus_ProbNNpi = _rdcr->CreateDoubleCopyLeaf("varPiOneminus_ProbNNpi", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_One_ProbNNpi"));
  ReducerLeaf<Double_t>& varPiOneminus_SumProbNNkpi = _rdcr->CreateDoubleLeaf("varPiOneminus_SumProbNNkpi", -999999.);
  varPiOneminus_SumProbNNkpi.Add(varPiOneminus_ProbNNk, varPiOneminus_ProbNNpi);
  _rdcr->CreateDoubleLeaf("varPiOneminus_PID", -999999.).Divide(varPiOneminus_ProbNNk, varPiOneminus_SumProbNNkpi);
  ReducerLeaf<Double_t>& varPiOneplus_ProbNNk = _rdcr->CreateDoubleCopyLeaf("varPiOneplus_ProbNNk", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_One_ProbNNk"));
  ReducerLeaf<Double_t>& varPiOneplus_ProbNNpi = _rdcr->CreateDoubleCopyLeaf("varPiOneplus_ProbNNpi", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_One_ProbNNpi"));
  ReducerLeaf<Double_t>& varPiOneplus_SumProbNNkpi = _rdcr->CreateDoubleLeaf("varPiOneplus_SumProbNNkpi", -999999.);
  varPiOneplus_SumProbNNkpi.Add(varPiOneplus_ProbNNk, varPiOneplus_ProbNNpi);
  _rdcr->CreateDoubleLeaf("varPiOneplus_PID", -999999.).Divide(varPiOneplus_ProbNNk, varPiOneplus_SumProbNNkpi);
  ReducerLeaf<Double_t>& varPiTwominus_ProbNNk = _rdcr->CreateDoubleCopyLeaf("varPiTwominus_ProbNNk", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_Two_ProbNNk"));
  ReducerLeaf<Double_t>& varPiTwominus_ProbNNpi = _rdcr->CreateDoubleCopyLeaf("varPiTwominus_ProbNNpi", _rdcr->GetInterimLeafByName("Dminus_piminus_or_Kminus_Two_ProbNNpi"));
  ReducerLeaf<Double_t>& varPiTwominus_SumProbNNkpi = _rdcr->CreateDoubleLeaf("varPiTwominus_SumProbNNkpi", -999999.);
  varPiTwominus_SumProbNNkpi.Add(varPiTwominus_ProbNNk, varPiTwominus_ProbNNpi);
  _rdcr->CreateDoubleLeaf("varPiTwominus_PID", -999999.).Divide(varPiTwominus_ProbNNk, varPiTwominus_SumProbNNkpi);
  ReducerLeaf<Double_t>& varPiTwoplus_ProbNNk = _rdcr->CreateDoubleCopyLeaf("varPiTwoplus_ProbNNk", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_Two_ProbNNk"));
  ReducerLeaf<Double_t>& varPiTwoplus_ProbNNpi = _rdcr->CreateDoubleCopyLeaf("varPiTwoplus_ProbNNpi", _rdcr->GetInterimLeafByName("Dplus_piplus_or_Kplus_Two_ProbNNpi"));
  ReducerLeaf<Double_t>& varPiTwoplus_SumProbNNkpi = _rdcr->CreateDoubleLeaf("varPiTwoplus_SumProbNNkpi", -999999.);
  varPiTwoplus_SumProbNNkpi.Add(varPiTwoplus_ProbNNk, varPiTwoplus_ProbNNpi);
  _rdcr->CreateDoubleLeaf("varPiTwoplus_PID", -999999.).Divide(varPiTwoplus_ProbNNk, varPiTwoplus_SumProbNNkpi);
}
