/******************************************/
// DooVariablesGrimReaper.cpp
//
// Standalone GrimReaper that adds a lot of
// common variables to trees.
//
// Author: Christophe Cauet
// Date: 2013-10-14
/******************************************/

// from STL
#include <tuple>
#include <list>

// from ROOT
#include "TRandom3.h"

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
cfg_tuple Configure(Reducer* rdcr, std::string& channel);
void MCLeaves(Reducer* rdcr, cfg_tuple& cfg);
void MassLeaves(Reducer* rdcr, cfg_tuple& cfg);
void TimeLeaves(Reducer* rdcr, cfg_tuple& cfg);
void InfoLeaves(Reducer* rdcr, cfg_tuple& cfg);
void CopyLeaves(Reducer* rdcr, cfg_tuple& cfg);
void TrackTypeLeaves(Reducer* rdcr, cfg_tuple& cfg);
void TriggerLeaves(Reducer* rdcr, cfg_tuple& cfg);
void VetoLeaves(Reducer* rdcr, cfg_tuple& cfg);
void AuxiliaryLeaves(Reducer* rdcr, cfg_tuple& cfg);

// main
int main(int argc, char * argv[]){
  sinfo << "-info-  \t" << "DooVariablesGrimReaper \t" << "Welcome!" << endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, decay_channel;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    decay_channel = argv[5];
  }
  else{
    serr << "-ERROR- \t" << "DooVariablesGrimReaper \t" << "Parameters needed:" << endmsg;
    serr << "-ERROR- \t" << "DooVariablesGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name decay_channel" << endmsg;
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
  summary.AddHLine();
  MCLeaves(reducer, cfg);
  MassLeaves(reducer, cfg);
  TimeLeaves(reducer, cfg);
  InfoLeaves(reducer, cfg);
  CopyLeaves(reducer, cfg);
  TrackTypeLeaves(reducer, cfg);
  TriggerLeaves(reducer, cfg);
  VetoLeaves(reducer, cfg);
  AuxiliaryLeaves(reducer, cfg);

  reducer->Run();
  reducer->Finalize();
}

cfg_tuple Configure(Reducer* rdcr, std::string& channel){
  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  summary.AddSection("Channel");
  // typedef tuple: head, daughters, stable particles, isMC, isFlat
  std::string head ="";
  std::list<std::string> daughters;
  std::list<std::string> stable_particles;
  bool isMC = false;
  bool isFlat = false;
  if (channel == "Bd2JpsiKS"){
    head = "B0";
    daughters.push_back("J_psi_1S");
    daughters.push_back("KS0");
    stable_particles.push_back("muminus");
    stable_particles.push_back("muplus");
    stable_particles.push_back("piminus");
    stable_particles.push_back("piplus");
    isMC = rdcr->LeafExists("B0_BKGCAT");
    isFlat = (rdcr->LeafExists("flat_array_index") || rdcr->LeafExists("idxPV"));
  }
  else{
    serr << "-ERROR- \t" << "DooVariablesGrimReaper \t" << "No valid decay channel. Possible decay channels are:" << endmsg;
    serr << "-ERROR- \t" << "DooVariablesGrimReaper \t" << "- Bd2JspiKS" << endmsg;
  }
  summary.Add("Name", channel);
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

void MCLeaves(Reducer* rdcr, cfg_tuple& cfg){}
void MassLeaves(Reducer* rdcr, cfg_tuple& cfg){}
void TimeLeaves(Reducer* rdcr, cfg_tuple& cfg){}

void InfoLeaves(Reducer* rdcr, cfg_tuple& cfg){
  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  // number of PVs
  ReducerLeaf<Int_t>& var_npv_leaf = rdcr->CreateIntCopyLeaf("catNPV", rdcr->GetInterimLeafByName("nPV"));

  // magnet direction
  ReducerLeaf<Int_t>& var_mag_leaf = rdcr->CreateIntCopyLeaf("catMag", rdcr->GetInterimLeafByName("Polarity"));
  
  // number of tracks
  ReducerLeaf<Int_t>& var_ntrack_leaf = rdcr->CreateIntCopyLeaf("catNTrack", rdcr->GetInterimLeafByName("nTracks"));

  // data taking period
  ReducerLeaf<Int_t>& cat_year_leaf = rdcr->CreateIntLeaf("catYear", 0);
    cat_year_leaf.AddCondition("2011", "GpsTime < 1.325376e+15",  2011);
    cat_year_leaf.AddCondition("2012", "GpsTime >= 1.325376e+15", 2012);

  summary.Add("InfoLeaves", true);
}

void CopyLeaves(Reducer* rdcr, cfg_tuple& cfg){}

void TrackTypeLeaves(Reducer* rdcr, cfg_tuple& cfg){
  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  ReducerLeaf<Int_t>& catTrackType = rdcr->CreateIntLeaf("catTrackType", 0);
  if(rdcr->LeafExists("piplus_TRACK_Type")){
    catTrackType.AddCondition("longtrack", "(piplus_TRACK_Type==3)&&(piminus_TRACK_Type==3)", 33);
    catTrackType.AddCondition("downstream", "(piplus_TRACK_Type==5)&&(piminus_TRACK_Type==5)", 55);
    catTrackType.AddCondition("longdown", "(piplus_TRACK_Type==3)&&(piminus_TRACK_Type==5)", 35);
    catTrackType.AddCondition("downlong", "(piplus_TRACK_Type==5)&&(piminus_TRACK_Type==3)", 53);
  }
  summary.Add("TrackTypeLeaves", true);
}

void TriggerLeaves(Reducer* rdcr, cfg_tuple& cfg){}

void VetoLeaves(Reducer* rdcr, cfg_tuple& cfg){
  // handle flattened tuples (REPLACE THIS BY CONFIG!)
  std::string flat_suffix = "";
  if (rdcr->LeafExists("flat_array_index") || rdcr->LeafExists("idxPV")){
    doocore::io::sinfo << "You are running the reducer over a flat tuple!" << doocore::io::endmsg;
    if (!(rdcr->LeafExists("idxPV"))) ReducerLeaf<Int_t>& flat_index_leaf_ptr = rdcr->CreateIntCopyLeaf("idxPV", rdcr->GetInterimLeafByName("flat_array_index"));
    flat_suffix = "_flat";
  }

  // veto leafs
  std::string piplus_px, piplus_py, piplus_pz;
  std::string piminus_px, piminus_py, piminus_pz;
  std::string muplus_px, muplus_py, muplus_pz;
  std::string muminus_px, muminus_py, muminus_pz;

  std::string mass_hypo_constrains = "";
  if (rdcr->LeafExists("B0_FitJpsiPVConst_KS0_P0_PX")){
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
  else if (rdcr->LeafExists("piplus_PX")){
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
        rdcr->GetInterimLeafByName(piplus_px),
        rdcr->GetInterimLeafByName(piplus_py),
        rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        rdcr->GetInterimLeafByName(piminus_px),
        rdcr->GetInterimLeafByName(piminus_py),
        rdcr->GetInterimLeafByName(piminus_pz),
        139.57018);
    rdcr->RegisterDoubleLeaf(varKS0MassHypo_pipi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Jpsipipi = new KinematicReducerLeaf<Double_t>("varBMassHypo_Jpsipipi", "varBMassHypo_Jpsipipi", "Double_t", NULL);
    varBMassHypo_Jpsipipi->FixedMassDaughtersFourBodyDecayMotherMass(
        rdcr->GetInterimLeafByName(piplus_px),
        rdcr->GetInterimLeafByName(piplus_py),
        rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        rdcr->GetInterimLeafByName(piminus_px),
        rdcr->GetInterimLeafByName(piminus_py),
        rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        rdcr->GetInterimLeafByName(muplus_px),
        rdcr->GetInterimLeafByName(muplus_py),
        rdcr->GetInterimLeafByName(muplus_pz),
        105.6583715,
        rdcr->GetInterimLeafByName(muminus_px),
        rdcr->GetInterimLeafByName(muminus_py),
        rdcr->GetInterimLeafByName(muminus_pz),
        105.6583715);
    rdcr->RegisterDoubleLeaf(varBMassHypo_Jpsipipi);

    
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_piK = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_piK", "varKS0MassHypo_piK", "Double_t", NULL);
    varKS0MassHypo_piK->FixedMassDaughtersTwoBodyDecayMotherMass(
        rdcr->GetInterimLeafByName(piplus_px),
        rdcr->GetInterimLeafByName(piplus_py),
        rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        rdcr->GetInterimLeafByName(piminus_px),
        rdcr->GetInterimLeafByName(piminus_py),
        rdcr->GetInterimLeafByName(piminus_pz),
        493.677);
    rdcr->RegisterDoubleLeaf(varKS0MassHypo_piK);

    KinematicReducerLeaf<Double_t>* varBMassHypo_JpsipiK = new KinematicReducerLeaf<Double_t>("varBMassHypo_JpsipiK", "varBMassHypo_JpsipiK", "Double_t", NULL);
    varBMassHypo_JpsipiK->FixedMassDaughtersFourBodyDecayMotherMass(
        rdcr->GetInterimLeafByName(piplus_px),
        rdcr->GetInterimLeafByName(piplus_py),
        rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        rdcr->GetInterimLeafByName(piminus_px),
        rdcr->GetInterimLeafByName(piminus_py),
        rdcr->GetInterimLeafByName(piminus_pz),
        493.677,
        rdcr->GetInterimLeafByName(muplus_px),
        rdcr->GetInterimLeafByName(muplus_py),
        rdcr->GetInterimLeafByName(muplus_pz),
        105.6583715,
        rdcr->GetInterimLeafByName(muminus_px),
        rdcr->GetInterimLeafByName(muminus_py),
        rdcr->GetInterimLeafByName(muminus_pz),
        105.6583715);
    rdcr->RegisterDoubleLeaf(varBMassHypo_JpsipiK);
    
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_Kpi = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_Kpi", "varKS0MassHypo_Kpi", "Double_t", NULL);
    varKS0MassHypo_Kpi->FixedMassDaughtersTwoBodyDecayMotherMass(
        rdcr->GetInterimLeafByName(piplus_px),
        rdcr->GetInterimLeafByName(piplus_py),
        rdcr->GetInterimLeafByName(piplus_pz),
        493.677,
        rdcr->GetInterimLeafByName(piminus_px),
        rdcr->GetInterimLeafByName(piminus_py),
        rdcr->GetInterimLeafByName(piminus_pz),
        139.57018);
    rdcr->RegisterDoubleLeaf(varKS0MassHypo_Kpi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_JpsiKpi = new KinematicReducerLeaf<Double_t>("varBMassHypo_JpsiKpi", "varBMassHypo_JpsiKpi", "Double_t", NULL);
    varBMassHypo_JpsiKpi->FixedMassDaughtersFourBodyDecayMotherMass(
        rdcr->GetInterimLeafByName(piplus_px),
        rdcr->GetInterimLeafByName(piplus_py),
        rdcr->GetInterimLeafByName(piplus_pz),
        493.677,
        rdcr->GetInterimLeafByName(piminus_px),
        rdcr->GetInterimLeafByName(piminus_py),
        rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        rdcr->GetInterimLeafByName(muplus_px),
        rdcr->GetInterimLeafByName(muplus_py),
        rdcr->GetInterimLeafByName(muplus_pz),
        105.6583715,
        rdcr->GetInterimLeafByName(muminus_px),
        rdcr->GetInterimLeafByName(muminus_py),
        rdcr->GetInterimLeafByName(muminus_pz),
        105.6583715);
    rdcr->RegisterDoubleLeaf(varBMassHypo_JpsiKpi);
    
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_pip = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_pip", "varKS0MassHypo_pip", "Double_t", NULL);
    varKS0MassHypo_pip->FixedMassDaughtersTwoBodyDecayMotherMass(
        rdcr->GetInterimLeafByName(piplus_px),
        rdcr->GetInterimLeafByName(piplus_py),
        rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        rdcr->GetInterimLeafByName(piminus_px),
        rdcr->GetInterimLeafByName(piminus_py),
        rdcr->GetInterimLeafByName(piminus_pz),
        938.272046);
    rdcr->RegisterDoubleLeaf(varKS0MassHypo_pip);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Jpsipip = new KinematicReducerLeaf<Double_t>("varBMassHypo_Jpsipip", "varBMassHypo_Jpsipip", "Double_t", NULL);
    varBMassHypo_Jpsipip->FixedMassDaughtersFourBodyDecayMotherMass(
        rdcr->GetInterimLeafByName(piplus_px),
        rdcr->GetInterimLeafByName(piplus_py),
        rdcr->GetInterimLeafByName(piplus_pz),
        139.57018,
        rdcr->GetInterimLeafByName(piminus_px),
        rdcr->GetInterimLeafByName(piminus_py),
        rdcr->GetInterimLeafByName(piminus_pz),
        938.272046,
        rdcr->GetInterimLeafByName(muplus_px),
        rdcr->GetInterimLeafByName(muplus_py),
        rdcr->GetInterimLeafByName(muplus_pz),
        105.6583715,
        rdcr->GetInterimLeafByName(muminus_px),
        rdcr->GetInterimLeafByName(muminus_py),
        rdcr->GetInterimLeafByName(muminus_pz),
        105.6583715);
    rdcr->RegisterDoubleLeaf(varBMassHypo_Jpsipip);
    
    KinematicReducerLeaf<Double_t>* varKS0MassHypo_ppi = new KinematicReducerLeaf<Double_t>("varKS0MassHypo_ppi", "varKS0MassHypo_ppi", "Double_t", NULL);
    varKS0MassHypo_ppi->FixedMassDaughtersTwoBodyDecayMotherMass(
        rdcr->GetInterimLeafByName(piplus_px),
        rdcr->GetInterimLeafByName(piplus_py),
        rdcr->GetInterimLeafByName(piplus_pz),
        938.272046,
        rdcr->GetInterimLeafByName(piminus_px),
        rdcr->GetInterimLeafByName(piminus_py),
        rdcr->GetInterimLeafByName(piminus_pz),
        139.57018);
    rdcr->RegisterDoubleLeaf(varKS0MassHypo_ppi);

    KinematicReducerLeaf<Double_t>* varBMassHypo_Jpsippi = new KinematicReducerLeaf<Double_t>("varBMassHypo_Jpsippi", "varBMassHypo_Jpsippi", "Double_t", NULL);
    varBMassHypo_Jpsippi->FixedMassDaughtersFourBodyDecayMotherMass(
        rdcr->GetInterimLeafByName(piplus_px),
        rdcr->GetInterimLeafByName(piplus_py),
        rdcr->GetInterimLeafByName(piplus_pz),
        938.272046,
        rdcr->GetInterimLeafByName(piminus_px),
        rdcr->GetInterimLeafByName(piminus_py),
        rdcr->GetInterimLeafByName(piminus_pz),
        139.57018,
        rdcr->GetInterimLeafByName(muplus_px),
        rdcr->GetInterimLeafByName(muplus_py),
        rdcr->GetInterimLeafByName(muplus_pz),
        105.6583715,
        rdcr->GetInterimLeafByName(muminus_px),
        rdcr->GetInterimLeafByName(muminus_py),
        rdcr->GetInterimLeafByName(muminus_pz),
        105.6583715);
    rdcr->RegisterDoubleLeaf(varBMassHypo_Jpsippi);

    doocore::io::sinfo << "Veto leaves are filled using constrain: " << mass_hypo_constrains << doocore::io::endmsg;

    if (rdcr->LeafExists("varKS0MassHypoDaughtersPVConst_pipi")){
      // mass hypotheses using the maximal constrained fit (WIP)
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_pipi = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_pipi", "varKS0MassHypoDaughtersPVConst_pipi", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_pipi->FixedMassDaughtersTwoBodyDecayMotherMass(
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          139.57018,
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          139.57018);
      rdcr->RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_pipi);
      
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_piK = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_piK", "varKS0MassHypoDaughtersPVConst_piK", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_piK->FixedMassDaughtersTwoBodyDecayMotherMass(
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          139.57018,
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          493.677);
      rdcr->RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_piK);
      
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_Kpi = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_Kpi", "varKS0MassHypoDaughtersPVConst_Kpi", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_Kpi->FixedMassDaughtersTwoBodyDecayMotherMass(
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          493.677,
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          139.57018);
      rdcr->RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_Kpi);
      
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_pip = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_pip", "varKS0MassHypoDaughtersPVConst_pip", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_pip->FixedMassDaughtersTwoBodyDecayMotherMass(
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          139.57018,
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          938.272046);
      rdcr->RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_pip);
      
      KinematicReducerLeaf<Double_t>* varKS0MassHypoDaughtersPVConst_ppi = new KinematicReducerLeaf<Double_t>("varKS0MassHypoDaughtersPVConst_ppi", "varKS0MassHypoDaughtersPVConst_ppi", "Double_t", NULL);
      varKS0MassHypoDaughtersPVConst_ppi->FixedMassDaughtersTwoBodyDecayMotherMass(
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PX"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PY"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P0_PZ"+flat_suffix),
          938.272046,
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PX"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PY"+flat_suffix),
          rdcr->GetInterimLeafByName("B0_FitDaughtersPVConst_KS0_P1_PZ"+flat_suffix),
          139.57018);
      rdcr->RegisterDoubleLeaf(varKS0MassHypoDaughtersPVConst_ppi);
    }
  }
}

void AuxiliaryLeaves(Reducer* rdcr, cfg_tuple& cfg){}