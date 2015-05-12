/******************************************/
// B02DDWSVariablesGrimReaper.cpp
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
  sinfo << "-info-  \t" << "B02DDWSVariablesGrimReaper \t" << "Welcome!" << endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, decay_channel;
  if (argc == 5){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    decay_channel = "B02DD";
  }
  else{
    serr << "-ERROR- \t" << "B02DDWSVariablesGrimReaper \t" << "Parameters needed:" << endmsg;
    serr << "-ERROR- \t" << "B02DDWSVariablesGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name" << endmsg;
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
    daughters.push_back("Dplus");
    stable_particles.push_back("Dplus1_Kminus_or_piminus");
    stable_particles.push_back("Dplus1_piplus_or_Kplus_One");
    stable_particles.push_back("Dplus1_piplus_or_Kplus_Two");
    stable_particles.push_back("Dplus2_Kminus_or_piminus");
    stable_particles.push_back("Dplus2_piplus_or_Kplus_One");
    stable_particles.push_back("Dplus2_piplus_or_Kplus_Two");
    isMC = _rdcr->LeafExists(head+"_BKGCAT");
    isFlat = (_rdcr->LeafExists("flat_array_index") || _rdcr->LeafExists("idxPV"));
  }
  else{
    serr << "-ERROR- \t" << "B02DDWSVariablesGrimReaper \t" << "No valid decay channel. Possible decay channels are:" << endmsg;
    serr << "-ERROR- \t" << "B02DDWSVariablesGrimReaper \t" << "- B02DD" << endmsg;
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
    _rdcr->CreateDoubleCopyLeaf("obsMassDauTwo", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_DplusA_M"+flat_suffix));
    _rdcr->CreateDoubleCopyLeaf("obsMassErrDauTwo", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_DplusA_MERR"+flat_suffix));
  } 
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_LOKI_MASS_DDConstr")) {
    if (main_observable_constraint == "LOKI") main_observable_constraint = "LOKI_MASS_DDConstr";
    if (main_observable_constraint_error == "LOKI") main_observable_constraint_error = "LOKI_MASSERR_DDConstr";
    _rdcr->CreateDoubleCopyLeaf("obsMassLokiDDPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_LOKI_MASS_DDConstr"));
    _rdcr->CreateDoubleCopyLeaf("obsMassErrLokiDDPVConst", _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_LOKI_MASSERR_DDConstr"));
    _rdcr->CreateDoubleCopyLeaf("obsMassLokiDauOne", _rdcr->GetInterimLeafByName("Dplus1_MM"));
    _rdcr->CreateDoubleCopyLeaf("obsMassErrLokiDauOne", _rdcr->GetInterimLeafByName("Dplus1_MMERR"));
    _rdcr->CreateDoubleCopyLeaf("obsMassLokiDauTwo", _rdcr->GetInterimLeafByName("Dplus2_MM"));
    _rdcr->CreateDoubleCopyLeaf("obsMassErrLokiDauTwo", _rdcr->GetInterimLeafByName("Dplus2_MMERR"));
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
    ReducerLeaf<Double_t>& varDplus1TauSignificance = _rdcr->CreateDoubleLeaf("varDplus1TauSignificance", -999999.);
    varDplus1TauSignificance.Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_tau"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_tauErr"+flat_suffix));
    ReducerLeaf<Double_t>& varDplus2TauSignificance = _rdcr->CreateDoubleLeaf("varDplus2TauSignificance", -999999.);
    varDplus2TauSignificance.Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_DplusA_tau"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_DplusA_tauErr"+flat_suffix));
    _rdcr->CreateDoubleLeaf("varDMinTauSignificance", -999999.).Minimum(varDplus1TauSignificance, varDplus2TauSignificance);
    _rdcr->CreateDoubleLeaf("varDMaxTauSignificance", -999999.).Maximum(varDplus1TauSignificance, varDplus2TauSignificance);
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
  std::string Dplus1_px, Dplus1_py, Dplus1_pz;
  std::string Dplus2_px, Dplus2_py, Dplus2_pz;
  std::string piplus1_px, piplus1_py, piplus1_pz;
  std::string piplus10_px, piplus10_py, piplus10_pz;
  std::string piplus2_px, piplus2_py, piplus2_pz;
  std::string piplus20_px, piplus20_py, piplus20_pz;
  std::string Kminus1_px, Kminus1_py, Kminus1_pz;
  std::string Kminus2_px, Kminus2_py, Kminus2_pz;

  std::string mass_hypo_constraints = "";
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_Dplus_PX")){
    Dplus1_px    = std::get<0>(cfg)+"_FitPVConst_Dplus_PX"+flat_suffix;
    Dplus1_py    = std::get<0>(cfg)+"_FitPVConst_Dplus_PY"+flat_suffix;
    Dplus1_pz    = std::get<0>(cfg)+"_FitPVConst_Dplus_PZ"+flat_suffix;
    Dplus2_px    = std::get<0>(cfg)+"_FitPVConst_DplusA_PX"+flat_suffix;
    Dplus2_py    = std::get<0>(cfg)+"_FitPVConst_DplusA_PY"+flat_suffix;
    Dplus2_pz    = std::get<0>(cfg)+"_FitPVConst_DplusA_PZ"+flat_suffix;
  }
  else if (_rdcr->LeafExists("Dplus1_PX")){
    Dplus1_px    = "Dplus1_PX";
    Dplus1_py    = "Dplus1_PY";
    Dplus1_pz    = "Dplus1_PZ";
    Dplus2_px    = "Dplus2_PX";
    Dplus2_py    = "Dplus2_PY";
    Dplus2_pz    = "Dplus2_PZ";
  }
  if (_rdcr->LeafExists(std::get<0>(cfg)+"_FitPVConst_Dplus_P0_PX")){
    Kminus1_px   = std::get<0>(cfg)+"_FitPVConst_Dplus_P2_PX"+flat_suffix;
    Kminus1_py   = std::get<0>(cfg)+"_FitPVConst_Dplus_P2_PY"+flat_suffix;
    Kminus1_pz   = std::get<0>(cfg)+"_FitPVConst_Dplus_P2_PZ"+flat_suffix;
    piplus1_px   = std::get<0>(cfg)+"_FitPVConst_Dplus_P0_PX"+flat_suffix;
    piplus1_py   = std::get<0>(cfg)+"_FitPVConst_Dplus_P0_PY"+flat_suffix;
    piplus1_pz   = std::get<0>(cfg)+"_FitPVConst_Dplus_P0_PZ"+flat_suffix;
    piplus10_px  = std::get<0>(cfg)+"_FitPVConst_Dplus_P1_PX"+flat_suffix;
    piplus10_py  = std::get<0>(cfg)+"_FitPVConst_Dplus_P1_PY"+flat_suffix;
    piplus10_pz  = std::get<0>(cfg)+"_FitPVConst_Dplus_P1_PZ"+flat_suffix;
    Kminus2_px   = std::get<0>(cfg)+"_FitPVConst_Dplus0_P2_PX"+flat_suffix;
    Kminus2_py   = std::get<0>(cfg)+"_FitPVConst_Dplus0_P2_PY"+flat_suffix;
    Kminus2_pz   = std::get<0>(cfg)+"_FitPVConst_Dplus0_P2_PZ"+flat_suffix;
    piplus2_px   = std::get<0>(cfg)+"_FitPVConst_Dplus0_P0_PX"+flat_suffix;
    piplus2_py   = std::get<0>(cfg)+"_FitPVConst_Dplus0_P0_PY"+flat_suffix;
    piplus2_pz   = std::get<0>(cfg)+"_FitPVConst_Dplus0_P0_PZ"+flat_suffix;
    piplus20_px  = std::get<0>(cfg)+"_FitPVConst_Dplus0_P1_PX"+flat_suffix;
    piplus20_py  = std::get<0>(cfg)+"_FitPVConst_Dplus0_P1_PY"+flat_suffix;
    piplus20_pz  = std::get<0>(cfg)+"_FitPVConst_Dplus0_P1_PZ"+flat_suffix;
    mass_hypo_constraints = "PV";
  }
  else if (_rdcr->LeafExists("Dplus1_Kminus_or_piminus_PX")){
    Kminus1_px   = "Dplus1_Kminus_or_piminus_PX";
    Kminus1_py   = "Dplus1_Kminus_or_piminus_PY";
    Kminus1_pz   = "Dplus1_Kminus_or_piminus_PZ";
    piplus1_px   = "Dplus1_piplus_or_Kplus_One_PX";
    piplus1_py   = "Dplus1_piplus_or_Kplus_One_PY";
    piplus1_pz   = "Dplus1_piplus_or_Kplus_One_PZ";
    piplus10_px  = "Dplus1_piplus_or_Kplus_Two_PX";
    piplus10_py  = "Dplus1_piplus_or_Kplus_Two_PY";
    piplus10_pz  = "Dplus1_piplus_or_Kplus_Two_PZ";
    Kminus2_px   = "Dplus2_Kminus_or_piminus_PX";
    Kminus2_py   = "Dplus2_Kminus_or_piminus_PY";
    Kminus2_pz   = "Dplus2_Kminus_or_piminus_PZ";
    piplus2_px   = "Dplus2_piplus_or_Kplus_One_PX";
    piplus2_py   = "Dplus2_piplus_or_Kplus_One_PY";
    piplus2_pz   = "Dplus2_piplus_or_Kplus_One_PZ";
    piplus20_px  = "Dplus2_piplus_or_Kplus_Two_PX";
    piplus20_py  = "Dplus2_piplus_or_Kplus_Two_PY";
    piplus20_pz  = "Dplus2_piplus_or_Kplus_Two_PZ";
    mass_hypo_constraints = "NoConstr";
  }
  
  if (mass_hypo_constraints!=""){
    // mass hypotheses
    KinematicReducerLeaf<Double_t>* varKSMassHypo_DplusOne = new KinematicReducerLeaf<Double_t>("varKSMassHypo_DplusOne", "varKSMassHypo_DplusOne", "Double_t", NULL);
    varKSMassHypo_DplusOne->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varKSMassHypo_DplusOne);

    KinematicReducerLeaf<Double_t>* varKSMassHypo_DplusTwo = new KinematicReducerLeaf<Double_t>("varKSMassHypo_DplusTwo", "varKSMassHypo_DplusTwo", "Double_t", NULL);
    varKSMassHypo_DplusTwo->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varKSMassHypo_DplusTwo);

    KinematicReducerLeaf<Double_t>* varKSMassHypo_Dplus2One = new KinematicReducerLeaf<Double_t>("varKSMassHypo_Dplus2One", "varKSMassHypo_Dplus2One", "Double_t", NULL);
    varKSMassHypo_Dplus2One->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varKSMassHypo_Dplus2One);

    KinematicReducerLeaf<Double_t>* varKSMassHypo_Dplus2Two = new KinematicReducerLeaf<Double_t>("varKSMassHypo_Dplus2Two", "varKSMassHypo_Dplus2Two", "Double_t", NULL);
    varKSMassHypo_Dplus2Two->FixedMassDaughtersTwoBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varKSMassHypo_Dplus2Two);    

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_Kpipi = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_Kpipi", "varDplus1MassHypo_Kpipi", "Double_t", NULL);
    varDplus1MassHypo_Kpipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_Kpipi);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_Kpipi = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_Kpipi", "varDplus2MassHypo_Kpipi", "Double_t", NULL);
    varDplus2MassHypo_Kpipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_Kpipi);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_KKpi = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_KKpi", "varDplus1MassHypo_KKpi", "Double_t", NULL);
    varDplus1MassHypo_KKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_KKpi);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_KKpi = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_KKpi", "varDplus2MassHypo_KKpi", "Double_t", NULL);
    varDplus2MassHypo_KKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_KKpi);

  KinematicReducerLeaf<Double_t>* varDplus1MassHypo_KpiK = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_KpiK", "varDplus1MassHypo_KpiK", "Double_t", NULL);
    varDplus1MassHypo_KpiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_KpiK);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_KpiK = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_KpiK", "varDplus2MassHypo_KpiK", "Double_t", NULL);
    varDplus2MassHypo_KpiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_KpiK);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_KKK = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_KKK", "varDplus1MassHypo_KKK", "Double_t", NULL);
    varDplus1MassHypo_KKK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_KKK);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_KKK = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_KKK", "varDplus2MassHypo_KKK", "Double_t", NULL);
    varDplus2MassHypo_KKK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_KKK);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_pipipi = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_pipipi", "varDplus1MassHypo_pipipi", "Double_t", NULL);
    varDplus1MassHypo_pipipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_pipipi);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_pipipi = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_pipipi", "varDplus2MassHypo_pipipi", "Double_t", NULL);
    varDplus2MassHypo_pipipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_pipipi);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_pipiK = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_pipiK", "varDplus1MassHypo_pipiK", "Double_t", NULL);
    varDplus1MassHypo_pipiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_pipiK);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_pipiK = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_pipiK", "varDplus2MassHypo_pipiK", "Double_t", NULL);
    varDplus2MassHypo_pipiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_pipiK);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_piKpi = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_piKpi", "varDplus1MassHypo_piKpi", "Double_t", NULL);
    varDplus1MassHypo_piKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_piKpi);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_piKpi = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_piKpi", "varDplus2MassHypo_piKpi", "Double_t", NULL);
    varDplus2MassHypo_piKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_piKpi);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_piKK = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_piKK", "varDplus1MassHypo_piKK", "Double_t", NULL);
    varDplus1MassHypo_piKK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_piKK);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_piKK = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_piKK", "varDplus2MassHypo_piKK", "Double_t", NULL);
    varDplus2MassHypo_piKK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_piKK);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_Kppi = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_Kppi", "varDplus1MassHypo_Kppi", "Double_t", NULL);
    varDplus1MassHypo_Kppi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_Kppi);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_Kppi = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_Kppi", "varDplus2MassHypo_Kppi", "Double_t", NULL);
    varDplus2MassHypo_Kppi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_Kppi);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_Kpip = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_Kpip", "varDplus1MassHypo_Kpip", "Double_t", NULL);
    varDplus1MassHypo_Kpip->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        938.272046);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_Kpip);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_Kpip = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_Kpip", "varDplus2MassHypo_Kpip", "Double_t", NULL);
    varDplus2MassHypo_Kpip->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        938.272046);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_Kpip);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_pKpi = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_pKpi", "varDplus1MassHypo_pKpi", "Double_t", NULL);
    varDplus1MassHypo_pKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_pKpi);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_pKpi = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_pKpi", "varDplus2MassHypo_pKpi", "Double_t", NULL);
    varDplus2MassHypo_pKpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_pKpi);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_ppiK = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_ppiK", "varDplus1MassHypo_ppiK", "Double_t", NULL);
    varDplus1MassHypo_ppiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_ppiK);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_ppiK = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_ppiK", "varDplus2MassHypo_ppiK", "Double_t", NULL);
    varDplus2MassHypo_ppiK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_ppiK);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_pipK = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_pipK", "varDplus1MassHypo_pipK", "Double_t", NULL);
    varDplus1MassHypo_pipK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_pipK);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_pipK = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_pipK", "varDplus2MassHypo_pipK", "Double_t", NULL);
    varDplus2MassHypo_pipK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        493.677);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_pipK);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_piKp = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_piKp", "varDplus1MassHypo_piKp", "Double_t", NULL);
    varDplus1MassHypo_piKp->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        938.272046);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_piKp);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_piKp = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_piKp", "varDplus2MassHypo_piKp", "Double_t", NULL);
    varDplus2MassHypo_piKp->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        938.272046);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_piKp);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_ppipi = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_ppipi", "varDplus1MassHypo_ppipi", "Double_t", NULL);
    varDplus1MassHypo_ppipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_ppipi);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_ppipi = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_ppipi", "varDplus2MassHypo_ppipi", "Double_t", NULL);
    varDplus2MassHypo_ppipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_ppipi);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_pippi = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_pippi", "varDplus1MassHypo_pippi", "Double_t", NULL);
    varDplus1MassHypo_pippi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_pippi);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_pippi = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_pippi", "varDplus2MassHypo_pippi", "Double_t", NULL);
    varDplus2MassHypo_pippi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        938.272046,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_pippi);

    KinematicReducerLeaf<Double_t>* varDplus1MassHypo_pipip = new KinematicReducerLeaf<Double_t>("varDplus1MassHypo_pipip", "varDplus1MassHypo_pipip", "Double_t", NULL);
    varDplus1MassHypo_pipip->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        938.272046);
    _rdcr->RegisterDoubleLeaf(varDplus1MassHypo_pipip);

    KinematicReducerLeaf<Double_t>* varDplus2MassHypo_pipip = new KinematicReducerLeaf<Double_t>("varDplus2MassHypo_pipip", "varDplus2MassHypo_pipip", "Double_t", NULL);
    varDplus2MassHypo_pipip->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        938.272046);
    _rdcr->RegisterDoubleLeaf(varDplus2MassHypo_pipip);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Dplus1pipi = new KinematicReducerLeaf<Double_t>("varBMassHypo_Dplus1pipi", "varBMassHypo_Dplus1pipi", "Double_t", NULL);
    varBMassHypo_Dplus1pipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Dplus1_px),
        _rdcr->GetInterimLeafByName(Dplus1_py),
        _rdcr->GetInterimLeafByName(Dplus1_pz),
        1869.62,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_Dplus1pipi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Dplus2pipi = new KinematicReducerLeaf<Double_t>("varBMassHypo_Dplus2pipi", "varBMassHypo_Dplus2pipi", "Double_t", NULL);
    varBMassHypo_Dplus2pipi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Dplus2_px),
        _rdcr->GetInterimLeafByName(Dplus2_py),
        _rdcr->GetInterimLeafByName(Dplus2_pz),
        1869.62,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        139.57018,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_Dplus2pipi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Dplus1Kpi = new KinematicReducerLeaf<Double_t>("varBMassHypo_Dplus1Kpi", "varBMassHypo_Dplus1Kpi", "Double_t", NULL);
    varBMassHypo_Dplus1Kpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Dplus1_px),
        _rdcr->GetInterimLeafByName(Dplus1_py),
        _rdcr->GetInterimLeafByName(Dplus1_pz),
        1869.62,
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus2_px),
        _rdcr->GetInterimLeafByName(piplus2_py),
        _rdcr->GetInterimLeafByName(piplus2_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_Dplus1Kpi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Dplus2Kpi = new KinematicReducerLeaf<Double_t>("varBMassHypo_Dplus2Kpi", "varBMassHypo_Dplus2Kpi", "Double_t", NULL);
    varBMassHypo_Dplus2Kpi->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Dplus2_px),
        _rdcr->GetInterimLeafByName(Dplus2_py),
        _rdcr->GetInterimLeafByName(Dplus2_pz),
        1869.62,
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus1_px),
        _rdcr->GetInterimLeafByName(piplus1_py),
        _rdcr->GetInterimLeafByName(piplus1_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_Dplus2Kpi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Dplus1piK = new KinematicReducerLeaf<Double_t>("varBMassHypo_Dplus1piK", "varBMassHypo_Dplus1piK", "Double_t", NULL);
    varBMassHypo_Dplus1piK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Dplus1_px),
        _rdcr->GetInterimLeafByName(Dplus1_py),
        _rdcr->GetInterimLeafByName(Dplus1_pz),
        1869.62,
        _rdcr->GetInterimLeafByName(Kminus2_px),
        _rdcr->GetInterimLeafByName(Kminus2_py),
        _rdcr->GetInterimLeafByName(Kminus2_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus20_px),
        _rdcr->GetInterimLeafByName(piplus20_py),
        _rdcr->GetInterimLeafByName(piplus20_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_Dplus1piK);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Dplus2piK = new KinematicReducerLeaf<Double_t>("varBMassHypo_Dplus2piK", "varBMassHypo_Dplus2piK", "Double_t", NULL);
    varBMassHypo_Dplus2piK->FixedMassDaughtersThreeBodyDecayMotherMass(
        _rdcr->GetInterimLeafByName(Dplus2_px),
        _rdcr->GetInterimLeafByName(Dplus2_py),
        _rdcr->GetInterimLeafByName(Dplus2_pz),
        1869.62,
        _rdcr->GetInterimLeafByName(Kminus1_px),
        _rdcr->GetInterimLeafByName(Kminus1_py),
        _rdcr->GetInterimLeafByName(Kminus1_pz),
        493.677,
        _rdcr->GetInterimLeafByName(piplus10_px),
        _rdcr->GetInterimLeafByName(piplus10_py),
        _rdcr->GetInterimLeafByName(piplus10_pz),
        139.57018);
    _rdcr->RegisterDoubleLeaf(varBMassHypo_Dplus2piK);

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
  ReducerLeaf<Int_t>& catD1FinalState = _rdcr->CreateIntLeaf("catD1FinalState", -10);
      TCut  D1_Kpipi = TCut(TString("abs("+std::get<0>(cfg)+"_FitPVConst_Dplus_P2_ID"+flat_suffix+")==321&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus_P0_ID"+flat_suffix+")==211&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus_P1_ID"+flat_suffix+")==211"));
      TCut  D1_pipipi = TCut(TString("abs("+std::get<0>(cfg)+"_FitPVConst_Dplus_P2_ID"+flat_suffix+")==211&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus_P0_ID"+flat_suffix+")==211&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus_P1_ID"+flat_suffix+")==211"));
      TCut  D1_KKpi = TCut(TString("abs("+std::get<0>(cfg)+"_FitPVConst_Dplus_P2_ID"+flat_suffix+")==321&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus_P0_ID"+flat_suffix+")==321&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus_P1_ID"+flat_suffix+")==211"));
      TCut  D1_KpiK = TCut(TString("abs("+std::get<0>(cfg)+"_FitPVConst_Dplus_P2_ID"+flat_suffix+")==321&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus_P0_ID"+flat_suffix+")==211&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus_P1_ID"+flat_suffix+")==321"));
      catD1FinalState.AddCondition("Kpipi",D1_Kpipi.GetTitle(),1);
      catD1FinalState.AddCondition("pipipi",D1_pipipi.GetTitle(),2);
      catD1FinalState.AddCondition("KKpi",D1_KKpi.GetTitle(),3);
      catD1FinalState.AddCondition("KpiK",D1_KpiK.GetTitle(),4);

  ReducerLeaf<Int_t>& catD2FinalState = _rdcr->CreateIntLeaf("catD2FinalState", -10);
      TCut  D2_Kpipi = TCut(TString("abs("+std::get<0>(cfg)+"_FitPVConst_Dplus0_P2_ID"+flat_suffix+")==321&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus0_P0_ID"+flat_suffix+")==211&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus0_P1_ID"+flat_suffix+")==211"));
      TCut  D2_pipipi = TCut(TString("abs("+std::get<0>(cfg)+"_FitPVConst_Dplus0_P2_ID"+flat_suffix+")==211&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus0_P0_ID"+flat_suffix+")==211&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus0_P1_ID"+flat_suffix+")==211"));
      TCut  D2_KKpi = TCut(TString("abs("+std::get<0>(cfg)+"_FitPVConst_Dplus0_P2_ID"+flat_suffix+")==321&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus0_P0_ID"+flat_suffix+")==321&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus0_P1_ID"+flat_suffix+")==211"));
      TCut  D2_KpiK = TCut(TString("abs("+std::get<0>(cfg)+"_FitPVConst_Dplus0_P2_ID"+flat_suffix+")==321&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus0_P0_ID"+flat_suffix+")==211&&abs("+std::get<0>(cfg)+"_FitPVConst_Dplus0_P1_ID"+flat_suffix+")==321"));
      catD2FinalState.AddCondition("Kpipi",D2_Kpipi.GetTitle(),1);
      catD2FinalState.AddCondition("pipipi",D2_pipipi.GetTitle(),2);
      catD2FinalState.AddCondition("KKpi",D2_KKpi.GetTitle(),3);
      catD2FinalState.AddCondition("KpiK",D2_KpiK.GetTitle(),4);

  ReducerLeaf<Int_t>& catDDFinalState = _rdcr->CreateIntLeaf("catDDFinalState", -10);
      catDDFinalState.AddCondition("KpipiKpipi", TString(D1_Kpipi && D2_Kpipi), 1);
      catDDFinalState.AddCondition("Kpipipipipi", TString(D1_Kpipi && D2_pipipi), 2);
      catDDFinalState.AddCondition("pipipiKpipi", TString(D1_pipipi && D2_Kpipi), 3);
      catDDFinalState.AddCondition("KpipiKKpi", TString(D1_Kpipi && D2_KKpi), 4);
      catDDFinalState.AddCondition("KKpiKpipi", TString(D1_KKpi && D2_Kpipi), 5);
      catDDFinalState.AddCondition("pipipipipipi", TString(D1_pipipi && D2_pipipi), 6);
      catDDFinalState.AddCondition("pipipiKKpi", TString(D1_pipipi && D2_KKpi), 7);
      catDDFinalState.AddCondition("KKpipipipi", TString(D1_KKpi && D2_pipipi), 8);
      catDDFinalState.AddCondition("KKpiKKpi", TString(D1_KKpi && D2_KKpi), 9);

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
  _rdcr->CreateDoubleCopyLeaf("varK1TrackGhostProb", _rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varK2TrackGhostProb", _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_TRACK_GhostProb"));
  _rdcr->CreateDoubleLeaf("varKaonMaxTrackGhostProb", -999999.).Maximum(_rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_TRACK_GhostProb"), _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varPi1OneTrackGhostProb", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_One_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varPi1TwoTrackGhostProb", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_Two_TRACK_GhostProb"));
  ReducerLeaf<Double_t>& varPi1MaxTrackGhostProb = _rdcr->CreateDoubleLeaf("varPi1MaxTrackGhostProb", -999999.);
  varPi1MaxTrackGhostProb.Maximum(_rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_One_TRACK_GhostProb"), _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_Two_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varPi2OneTrackGhostProb", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_One_TRACK_GhostProb"));
  _rdcr->CreateDoubleCopyLeaf("varPi2TwoTrackGhostProb", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_Two_TRACK_GhostProb"));
  ReducerLeaf<Double_t>& varPi2MaxTrackGhostProb = _rdcr->CreateDoubleLeaf("varPi2MaxTrackGhostProb", -999999.);
  varPi2MaxTrackGhostProb.Maximum(_rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_One_TRACK_GhostProb"), _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_Two_TRACK_GhostProb"));
  _rdcr->CreateDoubleLeaf("varPionMaxTrackGhostProb", -999999.).Maximum(varPi1MaxTrackGhostProb, varPi2MaxTrackGhostProb);

  // Primary vertex chi2/ndof
  ReducerLeaf<Double_t>& varPVVtxChi2ndof = _rdcr->CreateDoubleLeaf("varPVVtxChi2ndof", -99999999.);
  varPVVtxChi2ndof.Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_OWNPV_CHI2"), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_OWNPV_NDOF"));
  _rdcr->CreateDoubleLeaf("varPVVtxChi2ndof_log", -999999.).Log(varPVVtxChi2ndof);
  // Separation from PV
  _rdcr->CreateDoubleLeaf("varDMinVtxSepChi2", -99999999.).Minimum(_rdcr->GetInterimLeafByName("Dplus1_LOKI_VertexSeparation_CHI2"), _rdcr->GetInterimLeafByName("Dplus2_LOKI_VertexSeparation_CHI2"));
  _rdcr->CreateDoubleLeaf("varDMaxVtxSepChi2", -99999999.).Maximum(_rdcr->GetInterimLeafByName("Dplus1_LOKI_VertexSeparation_CHI2"), _rdcr->GetInterimLeafByName("Dplus2_LOKI_VertexSeparation_CHI2"));
  // End vertex chi2/ndof
  ReducerLeaf<Double_t>& varBEndVtxChi2ndof = _rdcr->CreateDoubleLeaf("varBEndVtxChi2ndof", -99999999.);
  varBEndVtxChi2ndof.Divide(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_ENDVERTEX_NDOF"));
  _rdcr->CreateDoubleLeaf("varBEndVtxChi2ndof_log", -999999.).Log(varBEndVtxChi2ndof);
  ReducerLeaf<Double_t>& varDplus1EndVtxChi2ndof = _rdcr->CreateDoubleLeaf("varDplus1EndVtxChi2ndof", -99999999.);
  varDplus1EndVtxChi2ndof.Divide(_rdcr->GetInterimLeafByName("Dplus1_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName("Dplus1_ENDVERTEX_NDOF"));
  ReducerLeaf<Double_t>& varDplus2EndVtxChi2ndof = _rdcr->CreateDoubleLeaf("varDplus2EndVtxChi2ndof", -99999999.);
  varDplus2EndVtxChi2ndof.Divide(_rdcr->GetInterimLeafByName("Dplus2_ENDVERTEX_CHI2"), _rdcr->GetInterimLeafByName("Dplus2_ENDVERTEX_NDOF"));
  _rdcr->CreateDoubleLeaf("varDMaxEndVtxChi2ndof", -999999.).Maximum(varDplus1EndVtxChi2ndof, varDplus2EndVtxChi2ndof);
  // sum of D transverse momenta
  _rdcr->CreateDoubleLeaf("varDDDTFSumPT", -99999999.).Add(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_PT"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_DplusA_PT"+flat_suffix));
  // maximal kaon track fit chi2ndof
  _rdcr->CreateDoubleLeaf("varKaonMaxTrackFitChi2ndof", -999999.).Maximum(_rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_TRACK_CHI2NDOF"), _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_TRACK_CHI2NDOF"));
  // maximal pion track fit chi2ndof
  ReducerLeaf<Double_t>& piplus1_max_trackchi2 = _rdcr->CreateDoubleLeaf("varPi1MaxTrackFitChi2ndof", -999999.);
  piplus1_max_trackchi2.Maximum(_rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_One_TRACK_CHI2NDOF"), _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_Two_TRACK_CHI2NDOF"));
  ReducerLeaf<Double_t>& piplus2_max_trackchi2 = _rdcr->CreateDoubleLeaf("varPi2MaxTrackFitChi2ndof", -999999.);
  piplus2_max_trackchi2.Maximum(_rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_One_TRACK_CHI2NDOF"), _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_Two_TRACK_CHI2NDOF"));
  _rdcr->CreateDoubleLeaf("varPionMaxTrackFitChi2ndof", -999999.).Maximum(piplus1_max_trackchi2, piplus2_max_trackchi2);
  // sum of D daughters transverse momentum
  ReducerLeaf<Double_t>& dplus1_sum_pions_pt = _rdcr->CreateDoubleLeaf("varDplus1PionsSumPT", -999999.);
  dplus1_sum_pions_pt.Add(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_P0_PT"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_P1_PT"+flat_suffix));
  ReducerLeaf<Double_t>& dplus1_sum_daughters_pt = _rdcr->CreateDoubleLeaf("varDplus1DaughtersSumPT", -999999.);
  dplus1_sum_daughters_pt.Add(dplus1_sum_pions_pt, _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus_P2_PT"+flat_suffix));
  ReducerLeaf<Double_t>& dplus2_sum_pions_pt = _rdcr->CreateDoubleLeaf("varDplus2PionsSumPT", -999999.);
  dplus2_sum_pions_pt.Add(_rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus0_P0_PT"+flat_suffix), _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus0_P1_PT"+flat_suffix));
  ReducerLeaf<Double_t>& dplus2_sum_daughters_pt = _rdcr->CreateDoubleLeaf("varDplus2DaughtersSumPT", -999999.);
  dplus2_sum_daughters_pt.Add(dplus2_sum_pions_pt, _rdcr->GetInterimLeafByName(std::get<0>(cfg)+"_FitPVConst_Dplus0_P2_PT"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varDMinDaughtersSumPT", -999999.).Minimum(dplus1_sum_daughters_pt, dplus2_sum_daughters_pt);

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
  ReducerLeaf<Double_t>* dplus1_ip_chi2_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* dplus2_ip_chi2_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitPVConst_Dplus_IPCHI2"+flat_suffix)) {
    dplus1_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplus1IPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_IPCHI2"+flat_suffix));
    dplus2_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplus2IPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_DplusA_IPCHI2"+flat_suffix));
  } else if (_rdcr->LeafExists("Dplus1_IPCHI2_OWNPV")) {
    dplus1_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplus1IPChi2", _rdcr->GetInterimLeafByName("Dplus1_IPCHI2_OWNPV"));
    dplus2_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplus2IPChi2", _rdcr->GetInterimLeafByName("Dplus2_IPCHI2_OWNPV"));
  }
  _rdcr->CreateDoubleLeaf("varDIPChi2Minimum", -999999.).Minimum(*dplus1_ip_chi2_leaf_ptr, *dplus2_ip_chi2_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varDIPChi2Maximum", -999999.).Maximum(*dplus1_ip_chi2_leaf_ptr, *dplus2_ip_chi2_leaf_ptr);
  ReducerLeaf<Double_t>* Kminus1_ip_chi2_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* piplus1_ip_chi2_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* piplus10_ip_chi2_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* Kminus2_ip_chi2_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* piplus2_ip_chi2_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* piplus20_ip_chi2_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitPVConst_Dplus_P0_IPCHI2"+flat_suffix)) {
    piplus1_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPi1OneIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_IPCHI2"+flat_suffix));
    piplus10_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPi1TwoIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_IPCHI2"+flat_suffix));
    Kminus1_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varK1IPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_IPCHI2"+flat_suffix));
    piplus2_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPi2OneIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P0_IPCHI2"+flat_suffix));
    piplus20_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPi2TwoIPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P1_IPCHI2"+flat_suffix));
    Kminus2_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varK2IPChi2", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P2_IPCHI2"+flat_suffix));
  } else if (_rdcr->LeafExists("Dplus_IPCHI2_OWNPV")) {
    piplus1_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPi1OneIPChi2", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_One_IPCHI2_OWNPV"));
    piplus10_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPi1TwoIPChi2", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_Two_IPCHI2_OWNPV"));
    Kminus1_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varK1IPChi2", _rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_IPCHI2_OWNPV"));
    piplus2_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPi2OneIPChi2", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_One_IPCHI2_OWNPV"));
    piplus20_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varPi2TwoIPChi2", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_Two_IPCHI2_OWNPV"));
    Kminus2_ip_chi2_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varK2IPChi2", _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_IPCHI2_OWNPV"));
  }
  _rdcr->CreateDoubleLeaf("varKaonIPChi2Minimum", -999999.).Minimum(*Kminus1_ip_chi2_leaf_ptr, *Kminus2_ip_chi2_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varPionOneIPChi2Minimum", -999999.).Minimum(*piplus2_ip_chi2_leaf_ptr, *piplus1_ip_chi2_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varPionTwoIPChi2Minimum", -999999.).Minimum(*piplus20_ip_chi2_leaf_ptr, *piplus10_ip_chi2_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varKaonIPChi2Maximum", -999999.).Maximum(*Kminus1_ip_chi2_leaf_ptr, *Kminus2_ip_chi2_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varPionOneIPChi2Maximum", -999999.).Maximum(*piplus2_ip_chi2_leaf_ptr, *piplus1_ip_chi2_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varPionTwoIPChi2Maximum", -999999.).Maximum(*piplus20_ip_chi2_leaf_ptr, *piplus10_ip_chi2_leaf_ptr);

  // Direction angle (DIRA)
  ReducerLeaf<Double_t>* dira_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* dplus1_dira_leaf_ptr = NULL;
  ReducerLeaf<Double_t>* dplus2_dira_leaf_ptr = NULL;
  if (_rdcr->LeafExists("B0_FitPVConst_DIRA"+flat_suffix)) {
    dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varBDTFDIRA", _rdcr->GetInterimLeafByName("B0_FitPVConst_DIRA"+flat_suffix));
    dplus1_dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplus1DTFDIRA", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_DIRA"+flat_suffix));
    dplus2_dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplus2DTFDIRA", _rdcr->GetInterimLeafByName("B0_FitPVConst_DplusA_DIRA"+flat_suffix));
  } else if (_rdcr->LeafExists("B0_DIRA_OWNPV")) {
    dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varBDIRAOwnPV", _rdcr->GetInterimLeafByName("B0_DIRA_OWNPV"));
    dplus1_dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplus1DIRAOwnPV", _rdcr->GetInterimLeafByName("Dplus1_DIRA_OWNPV"));
    dplus2_dira_leaf_ptr = &_rdcr->CreateDoubleCopyLeaf("varDplus2DIRAOwnPV", _rdcr->GetInterimLeafByName("Dplus2_DIRA_OWNPV"));
  }
  _rdcr->CreateDoubleLeaf("varDMinDIRA", -999999.).Minimum(*dplus1_dira_leaf_ptr, *dplus2_dira_leaf_ptr);
  _rdcr->CreateDoubleLeaf("varDMaxDIRA", -999999.).Maximum(*dplus1_dira_leaf_ptr, *dplus2_dira_leaf_ptr);

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
  _rdcr->CreateDoubleCopyLeaf("varDplus1P", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varDplus1PT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varDplus2P", _rdcr->GetInterimLeafByName("B0_FitPVConst_DplusA_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varDplus2PT", _rdcr->GetInterimLeafByName("B0_FitPVConst_DplusA_PT"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varDMinP", -999999.).Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_DplusA_P"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varDMinPT", -999999.).Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_DplusA_PT"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varDMaxP", -999999.).Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_DplusA_P"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varDMaxPT", -999999.).Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_DplusA_PT"+flat_suffix));
  // grand-daughters
  _rdcr->CreateDoubleCopyLeaf("varPi1OneP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPi1OnePT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPi1TwoP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPi1TwoPT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPi2OneP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P0_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPi2OnePT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P0_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPi2TwoP", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P1_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varPi2TwoPT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P1_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varK1P", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varK1PT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_PT"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varK2P", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P2_P"+flat_suffix));
  _rdcr->CreateDoubleCopyLeaf("varK2PT", _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P2_PT"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varPionOneMinP", -999999.).Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P0_P"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varPionTwoMinP", -999999.).Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P1_P"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varKaonMinP", -999999.).Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P2_P"+flat_suffix));
  ReducerLeaf<Double_t>& varPionOneMinPT = _rdcr->CreateDoubleLeaf("varPionOneMinPT", -999999.);
  varPionOneMinPT.Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P0_PT"+flat_suffix));
  ReducerLeaf<Double_t>& varPionTwoMinPT = _rdcr->CreateDoubleLeaf("varPionTwoMinPT", -999999.);
  varPionTwoMinPT.Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P1_PT"+flat_suffix));
  ReducerLeaf<Double_t>& varPionMinPT = _rdcr->CreateDoubleLeaf("varPionMinPT", -999999.);
  varPionMinPT.Minimum(varPionOneMinPT, varPionTwoMinPT);
  ReducerLeaf<Double_t>& varKaonMinPT = _rdcr->CreateDoubleLeaf("varKaonMinPT", -999999.);
  varKaonMinPT.Minimum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P2_PT"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varHadronMinPT", -999999.).Minimum(varPionMinPT, varKaonMinPT);
  _rdcr->CreateDoubleLeaf("varPionOneMaxP", -999999.).Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P0_P"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varPionTwoMaxP", -999999.).Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P1_P"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varKaonMaxP", -999999.).Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_P"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P2_P"+flat_suffix));
  ReducerLeaf<Double_t>& varPionOneMaxPT = _rdcr->CreateDoubleLeaf("varPionOneMaxPT", -999999.);
  varPionOneMaxPT.Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P0_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P0_PT"+flat_suffix));
  ReducerLeaf<Double_t>& varPionTwoMaxPT = _rdcr->CreateDoubleLeaf("varPionTwoMaxPT", -999999.);
  varPionTwoMaxPT.Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P1_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P1_PT"+flat_suffix));
  ReducerLeaf<Double_t>& varPionMaxPT = _rdcr->CreateDoubleLeaf("varPionMaxPT", -999999.);
  varPionMaxPT.Maximum(varPionOneMaxPT, varPionTwoMaxPT);
  ReducerLeaf<Double_t>& varKaonMaxPT = _rdcr->CreateDoubleLeaf("varKaonMaxPT", -999999.);
  varKaonMaxPT.Maximum(_rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus_P2_PT"+flat_suffix), _rdcr->GetInterimLeafByName("B0_FitPVConst_Dplus0_P2_PT"+flat_suffix));
  _rdcr->CreateDoubleLeaf("varHadronMaxPT", -999999.).Maximum(varPionMaxPT, varKaonMaxPT);

  // Velo track quality
  _rdcr->CreateDoubleCopyLeaf("varK1VELOChi2ndof", _rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_TRACK_VeloCHI2NDOF"));
  _rdcr->CreateDoubleCopyLeaf("varK2VELOChi2ndof", _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_TRACK_VeloCHI2NDOF"));
  ReducerLeaf<Double_t>& varKaonMinVELOChi2ndof = _rdcr->CreateDoubleLeaf("varKaonMinVELOChi2ndof", -999999.);
  varKaonMinVELOChi2ndof.Minimum(_rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_TRACK_VeloCHI2NDOF"), _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_TRACK_VeloCHI2NDOF"));
  _rdcr->CreateDoubleLeaf("varKaonMinVELOChi2ndof_log", -999999.).Log(varKaonMinVELOChi2ndof);
  ReducerLeaf<Double_t>& varKaonMaxVELOChi2ndof = _rdcr->CreateDoubleLeaf("varKaonMaxVELOChi2ndof", -999999.);
  varKaonMaxVELOChi2ndof.Maximum(_rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_TRACK_VeloCHI2NDOF"), _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_TRACK_VeloCHI2NDOF"));
  _rdcr->CreateDoubleLeaf("varKaonMaxVELOChi2ndof_log", -999999.).Log(varKaonMaxVELOChi2ndof);
  ReducerLeaf<Double_t>& varPi1OneVELOChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPi1OneVELOChi2ndof", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_One_TRACK_VeloCHI2NDOF"));
  ReducerLeaf<Double_t>& varPi1TwoVELOChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPi1TwoVELOChi2ndof", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_Two_TRACK_VeloCHI2NDOF"));
  ReducerLeaf<Double_t>& varPi1MinVELOChi2ndof = _rdcr->CreateDoubleLeaf("varPi1MinVELOChi2ndof", -999999.);
  varPi1MinVELOChi2ndof.Minimum(varPi1OneVELOChi2ndof, varPi1TwoVELOChi2ndof);
  ReducerLeaf<Double_t>& varPi1MaxVELOChi2ndof = _rdcr->CreateDoubleLeaf("varPi1MaxVELOChi2ndof", -999999.);
  varPi1MaxVELOChi2ndof.Maximum(varPi1OneVELOChi2ndof, varPi1TwoVELOChi2ndof);
  ReducerLeaf<Double_t>& varPi2OneVELOChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPi2OneVELOChi2ndof", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_One_TRACK_VeloCHI2NDOF"));
  ReducerLeaf<Double_t>& varPi2TwoVELOChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPi2TwoVELOChi2ndof", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_Two_TRACK_VeloCHI2NDOF"));
  ReducerLeaf<Double_t>& varPi2MinVELOChi2ndof = _rdcr->CreateDoubleLeaf("varPi2MinVELOChi2ndof", -999999.);
  varPi2MinVELOChi2ndof.Minimum(varPi2OneVELOChi2ndof, varPi2TwoVELOChi2ndof);
  ReducerLeaf<Double_t>& varPionMinVELOChi2ndof = _rdcr->CreateDoubleLeaf("varPionMinVELOChi2ndof", -999999.);
  varPionMinVELOChi2ndof.Minimum(varPi1MinVELOChi2ndof, varPi2MinVELOChi2ndof);
  _rdcr->CreateDoubleLeaf("varPionMinVELOChi2ndof_log", -999999.).Log(varPionMinVELOChi2ndof);
  ReducerLeaf<Double_t>& varPi2MaxVELOChi2ndof = _rdcr->CreateDoubleLeaf("varPi2MaxVELOChi2ndof", -999999.);
  varPi2MaxVELOChi2ndof.Maximum(varPi2OneVELOChi2ndof, varPi2TwoVELOChi2ndof);
  ReducerLeaf<Double_t>& varPionMaxVELOChi2ndof = _rdcr->CreateDoubleLeaf("varPionMaxVELOChi2ndof", -999999.);
  varPionMaxVELOChi2ndof.Maximum(varPi1MaxVELOChi2ndof, varPi2MaxVELOChi2ndof);
  _rdcr->CreateDoubleLeaf("varPionMaxVELOChi2ndof_log", -999999.).Log(varPionMaxVELOChi2ndof);
  ReducerLeaf<Double_t>& varHadronMinVELOChi2ndof = _rdcr->CreateDoubleLeaf("varHadronMinVELOChi2ndof", -999999.);
  varHadronMinVELOChi2ndof.Minimum(varKaonMinVELOChi2ndof, varPionMinVELOChi2ndof);
  _rdcr->CreateDoubleLeaf("varHadronMinVELOChi2ndof_log", -999999.).Log(varHadronMinVELOChi2ndof);
  ReducerLeaf<Double_t>& varHadronMaxVELOChi2ndof = _rdcr->CreateDoubleLeaf("varHadronMaxVELOChi2ndof", -999999.);
  varHadronMaxVELOChi2ndof.Minimum(varKaonMaxVELOChi2ndof, varPionMaxVELOChi2ndof);
  _rdcr->CreateDoubleLeaf("varHadronMaxVELOChi2ndof_log", -999999.).Log(varHadronMaxVELOChi2ndof);

  // T-stations track quality
  _rdcr->CreateDoubleCopyLeaf("varK1TChi2ndof", _rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_TRACK_TCHI2NDOF"));
  _rdcr->CreateDoubleCopyLeaf("varK2TChi2ndof", _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_TRACK_TCHI2NDOF"));
  ReducerLeaf<Double_t>& varKaonMinTChi2ndof = _rdcr->CreateDoubleLeaf("varKaonMinTChi2ndof", -999999.);
  varKaonMinTChi2ndof.Minimum(_rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_TRACK_TCHI2NDOF"), _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_TRACK_TCHI2NDOF"));
  _rdcr->CreateDoubleLeaf("varKaonMinTChi2ndof_log", -999999.).Log(varKaonMinTChi2ndof);
  ReducerLeaf<Double_t>& varKaonMaxTChi2ndof = _rdcr->CreateDoubleLeaf("varKaonMaxTChi2ndof", -999999.);
  varKaonMaxTChi2ndof.Maximum(_rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_TRACK_TCHI2NDOF"), _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_TRACK_TCHI2NDOF"));
  _rdcr->CreateDoubleLeaf("varKaonMaxTChi2ndof_log", -999999.).Log(varKaonMaxTChi2ndof);
  ReducerLeaf<Double_t>& varPi1OneTChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPi1OneTChi2ndof", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_One_TRACK_TCHI2NDOF"));
  ReducerLeaf<Double_t>& varPi1TwoTChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPi1TwoTChi2ndof", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_Two_TRACK_TCHI2NDOF"));
  ReducerLeaf<Double_t>& varPi1MinTChi2ndof = _rdcr->CreateDoubleLeaf("varPi1MinTChi2ndof", -999999.);
  varPi1MinTChi2ndof.Minimum(varPi1OneTChi2ndof, varPi1TwoTChi2ndof);
  ReducerLeaf<Double_t>& varPi1MaxTChi2ndof = _rdcr->CreateDoubleLeaf("varPi1MaxTChi2ndof", -999999.);
  varPi1MaxTChi2ndof.Maximum(varPi1OneTChi2ndof, varPi1TwoTChi2ndof);
  ReducerLeaf<Double_t>& varPi2OneTChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPi2OneTChi2ndof", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_One_TRACK_TCHI2NDOF"));
  ReducerLeaf<Double_t>& varPi2TwoTChi2ndof = _rdcr->CreateDoubleCopyLeaf("varPi2TwoTChi2ndof", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_Two_TRACK_TCHI2NDOF"));
  ReducerLeaf<Double_t>& varPi2MinTChi2ndof = _rdcr->CreateDoubleLeaf("varPi2MinTChi2ndof", -999999.);
  varPi2MinTChi2ndof.Minimum(varPi2OneTChi2ndof, varPi2TwoTChi2ndof);
  ReducerLeaf<Double_t>& varPionMinTChi2ndof = _rdcr->CreateDoubleLeaf("varPionMinTChi2ndof", -999999.);
  varPionMinTChi2ndof.Minimum(varPi1MinTChi2ndof, varPi2MinTChi2ndof);
  _rdcr->CreateDoubleLeaf("varPionMinTChi2ndof_log", -999999.).Log(varPionMinTChi2ndof);
  ReducerLeaf<Double_t>& varPi2MaxTChi2ndof = _rdcr->CreateDoubleLeaf("varPi2MaxTChi2ndof", -999999.);
  varPi2MaxTChi2ndof.Maximum(varPi2OneTChi2ndof, varPi2TwoTChi2ndof);
  ReducerLeaf<Double_t>& varPionMaxTChi2ndof = _rdcr->CreateDoubleLeaf("varPionMaxTChi2ndof", -999999.);
  varPionMaxTChi2ndof.Maximum(varPi1MaxTChi2ndof, varPi2MaxTChi2ndof);
  _rdcr->CreateDoubleLeaf("varPionMaxTChi2ndof_log", -999999.).Log(varPionMaxTChi2ndof);
  ReducerLeaf<Double_t>& varHadronMinTChi2ndof = _rdcr->CreateDoubleLeaf("varHadronMinTChi2ndof", -999999.);
  varHadronMinTChi2ndof.Minimum(varKaonMinTChi2ndof, varPionMinTChi2ndof);
  _rdcr->CreateDoubleLeaf("varHadronMinTChi2ndof_log", -999999.).Log(varHadronMinTChi2ndof);
  ReducerLeaf<Double_t>& varHadronMaxTChi2ndof = _rdcr->CreateDoubleLeaf("varHadronMaxTChi2ndof", -999999.);
  varHadronMaxTChi2ndof.Minimum(varKaonMaxTChi2ndof, varPionMaxTChi2ndof);
  _rdcr->CreateDoubleLeaf("varHadronMaxTChi2ndof_log", -999999.).Log(varHadronMaxTChi2ndof);

  // track match quality
  _rdcr->CreateDoubleCopyLeaf("varK1MatchChi2", _rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_TRACK_MatchCHI2"));
  _rdcr->CreateDoubleCopyLeaf("varK2MatchChi2", _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_TRACK_MatchCHI2"));
  ReducerLeaf<Double_t>& varKaonMinMatchChi2 = _rdcr->CreateDoubleLeaf("varKaonMinMatchChi2", -999999.);
  varKaonMinMatchChi2.Minimum(_rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_TRACK_MatchCHI2"), _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_TRACK_MatchCHI2"));
  _rdcr->CreateDoubleLeaf("varKaonMinMatchChi2_log", -999999.).Log(varKaonMinMatchChi2);
  ReducerLeaf<Double_t>& varKaonMaxMatchChi2 = _rdcr->CreateDoubleLeaf("varKaonMaxMatchChi2", -999999.);
  varKaonMaxMatchChi2.Maximum(_rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_TRACK_MatchCHI2"), _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_TRACK_MatchCHI2"));
  _rdcr->CreateDoubleLeaf("varKaonMaxMatchChi2_log", -999999.).Log(varKaonMaxMatchChi2);
  ReducerLeaf<Double_t>& varPi1OneMatchChi2 = _rdcr->CreateDoubleCopyLeaf("varPi1OneMatchChi2", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_One_TRACK_MatchCHI2"));
  ReducerLeaf<Double_t>& varPi1TwoMatchChi2 = _rdcr->CreateDoubleCopyLeaf("varPi1TwoMatchChi2", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_Two_TRACK_MatchCHI2"));
  ReducerLeaf<Double_t>& varPi1MinMatchChi2 = _rdcr->CreateDoubleLeaf("varPi1MinMatchChi2", -999999.);
  varPi1MinMatchChi2.Minimum(varPi1OneMatchChi2, varPi1TwoMatchChi2);
  ReducerLeaf<Double_t>& varPi1MaxMatchChi2 = _rdcr->CreateDoubleLeaf("varPi1MaxMatchChi2", -999999.);
  varPi1MaxMatchChi2.Maximum(varPi1OneMatchChi2, varPi1TwoMatchChi2);
  ReducerLeaf<Double_t>& varPi2OneMatchChi2 = _rdcr->CreateDoubleCopyLeaf("varPi2OneMatchChi2", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_One_TRACK_MatchCHI2"));
  ReducerLeaf<Double_t>& varPi2TwoMatchChi2 = _rdcr->CreateDoubleCopyLeaf("varPi2TwoMatchChi2", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_Two_TRACK_MatchCHI2"));
  ReducerLeaf<Double_t>& varPi2MinMatchChi2 = _rdcr->CreateDoubleLeaf("varPi2MinMatchChi2", -999999.);
  varPi2MinMatchChi2.Minimum(varPi2OneMatchChi2, varPi2TwoMatchChi2);
  ReducerLeaf<Double_t>& varPionMinMatchChi2 = _rdcr->CreateDoubleLeaf("varPionMinMatchChi2", -999999.);
  varPionMinMatchChi2.Minimum(varPi1MinMatchChi2, varPi2MinMatchChi2);
  _rdcr->CreateDoubleLeaf("varPionMinMatchChi2_log", -999999.).Log(varPionMinMatchChi2);
  ReducerLeaf<Double_t>& varPi2MaxMatchChi2 = _rdcr->CreateDoubleLeaf("varPi2MaxMatchChi2", -999999.);
  varPi2MaxMatchChi2.Maximum(varPi2OneMatchChi2, varPi2TwoMatchChi2);
  ReducerLeaf<Double_t>& varPionMaxMatchChi2 = _rdcr->CreateDoubleLeaf("varPionMaxMatchChi2", -999999.);
  varPionMaxMatchChi2.Maximum(varPi1MaxMatchChi2, varPi2MaxMatchChi2);
  _rdcr->CreateDoubleLeaf("varPionMaxMatchChi2_log", -999999.).Log(varPionMaxMatchChi2);
  ReducerLeaf<Double_t>& varHadronMinMatchChi2 = _rdcr->CreateDoubleLeaf("varHadronMinMatchChi2", -999999.);
  varHadronMinMatchChi2.Minimum(varKaonMinMatchChi2, varPionMinMatchChi2);
  _rdcr->CreateDoubleLeaf("varHadronMinMatchChi2_log", -999999.).Log(varHadronMinMatchChi2);
  ReducerLeaf<Double_t>& varHadronMaxMatchChi2 = _rdcr->CreateDoubleLeaf("varHadronMaxMatchChi2", -999999.);
  varHadronMaxMatchChi2.Minimum(varKaonMaxMatchChi2, varPionMaxMatchChi2);
  _rdcr->CreateDoubleLeaf("varHadronMaxMatchChi2_log", -999999.).Log(varHadronMaxMatchChi2);

  // PID variables
  ReducerLeaf<Double_t>& varK1_ProbNNk = _rdcr->CreateDoubleCopyLeaf("varK1_ProbNNk", _rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_ProbNNk"));
  ReducerLeaf<Double_t>& varK1_ProbNNpi = _rdcr->CreateDoubleCopyLeaf("varK1_ProbNNpi", _rdcr->GetInterimLeafByName("Dplus1_Kminus_or_piminus_ProbNNpi"));
  ReducerLeaf<Double_t>& varK1_SumProbNNkpi = _rdcr->CreateDoubleLeaf("varK1_SumProbNNkpi", -999999.);
  varK1_SumProbNNkpi.Add(varK1_ProbNNk, varK1_ProbNNpi);
  _rdcr->CreateDoubleLeaf("varK1_PID", -999999.).Divide(varK1_ProbNNk, varK1_SumProbNNkpi);
  ReducerLeaf<Double_t>& varK2_ProbNNk = _rdcr->CreateDoubleCopyLeaf("varK2_ProbNNk", _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_ProbNNk"));
  ReducerLeaf<Double_t>& varK2_ProbNNpi = _rdcr->CreateDoubleCopyLeaf("varK2_ProbNNpi", _rdcr->GetInterimLeafByName("Dplus2_Kminus_or_piminus_ProbNNpi"));
  ReducerLeaf<Double_t>& varK2_SumProbNNkpi = _rdcr->CreateDoubleLeaf("varK2_SumProbNNkpi", -999999.);
  varK2_SumProbNNkpi.Add(varK2_ProbNNk, varK2_ProbNNpi);
  _rdcr->CreateDoubleLeaf("varK2_PID", -999999.).Divide(varK2_ProbNNk, varK2_SumProbNNkpi);
  ReducerLeaf<Double_t>& varPiOne1_ProbNNk = _rdcr->CreateDoubleCopyLeaf("varPiOne1_ProbNNk", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_One_ProbNNk"));
  ReducerLeaf<Double_t>& varPiOne1_ProbNNpi = _rdcr->CreateDoubleCopyLeaf("varPiOne1_ProbNNpi", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_One_ProbNNpi"));
  ReducerLeaf<Double_t>& varPiOne1_SumProbNNkpi = _rdcr->CreateDoubleLeaf("varPiOne1_SumProbNNkpi", -999999.);
  varPiOne1_SumProbNNkpi.Add(varPiOne1_ProbNNk, varPiOne1_ProbNNpi);
  _rdcr->CreateDoubleLeaf("varPiOne1_PID", -999999.).Divide(varPiOne1_ProbNNk, varPiOne1_SumProbNNkpi);
  ReducerLeaf<Double_t>& varPiOne2_ProbNNk = _rdcr->CreateDoubleCopyLeaf("varPiOne2_ProbNNk", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_One_ProbNNk"));
  ReducerLeaf<Double_t>& varPiOne2_ProbNNpi = _rdcr->CreateDoubleCopyLeaf("varPiOne2_ProbNNpi", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_One_ProbNNpi"));
  ReducerLeaf<Double_t>& varPiOne2_SumProbNNkpi = _rdcr->CreateDoubleLeaf("varPiOne2_SumProbNNkpi", -999999.);
  varPiOne2_SumProbNNkpi.Add(varPiOne2_ProbNNk, varPiOne2_ProbNNpi);
  _rdcr->CreateDoubleLeaf("varPiOne2_PID", -999999.).Divide(varPiOne2_ProbNNk, varPiOne2_SumProbNNkpi);
  ReducerLeaf<Double_t>& varPiTwo1_ProbNNk = _rdcr->CreateDoubleCopyLeaf("varPiTwo1_ProbNNk", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_Two_ProbNNk"));
  ReducerLeaf<Double_t>& varPiTwo1_ProbNNpi = _rdcr->CreateDoubleCopyLeaf("varPiTwo1_ProbNNpi", _rdcr->GetInterimLeafByName("Dplus1_piplus_or_Kplus_Two_ProbNNpi"));
  ReducerLeaf<Double_t>& varPiTwo1_SumProbNNkpi = _rdcr->CreateDoubleLeaf("varPiTwo1_SumProbNNkpi", -999999.);
  varPiTwo1_SumProbNNkpi.Add(varPiTwo1_ProbNNk, varPiTwo1_ProbNNpi);
  _rdcr->CreateDoubleLeaf("varPiTwo1_PID", -999999.).Divide(varPiTwo1_ProbNNk, varPiTwo1_SumProbNNkpi);
  ReducerLeaf<Double_t>& varPiTwo2_ProbNNk = _rdcr->CreateDoubleCopyLeaf("varPiTwo2_ProbNNk", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_Two_ProbNNk"));
  ReducerLeaf<Double_t>& varPiTwo2_ProbNNpi = _rdcr->CreateDoubleCopyLeaf("varPiTwo2_ProbNNpi", _rdcr->GetInterimLeafByName("Dplus2_piplus_or_Kplus_Two_ProbNNpi"));
  ReducerLeaf<Double_t>& varPiTwo2_SumProbNNkpi = _rdcr->CreateDoubleLeaf("varPiTwo2_SumProbNNkpi", -999999.);
  varPiTwo2_SumProbNNkpi.Add(varPiTwo2_ProbNNk, varPiTwo2_ProbNNpi);
  _rdcr->CreateDoubleLeaf("varPiTwo2_PID", -999999.).Divide(varPiTwo2_ProbNNk, varPiTwo2_SumProbNNkpi);
}
