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
}

void CopyLeaves(Reducer* rdcr, cfg_tuple& cfg){}

void TrackTypeLeaves(Reducer* rdcr, cfg_tuple& cfg){
  sinfo << "track type leaves" << endmsg;
  // track type category
  ReducerLeaf<Int_t>& catTrackType = rdcr->CreateIntLeaf("catTrackType", 0);
  if(rdcr->LeafExists("piplus_TRACK_Type")){
    catTrackType.AddCondition("longtrack", "(piplus_TRACK_Type==3)&&(piminus_TRACK_Type==3)", 33);
    catTrackType.AddCondition("downstream", "(piplus_TRACK_Type==5)&&(piminus_TRACK_Type==5)", 55);
    catTrackType.AddCondition("longdown", "(piplus_TRACK_Type==3)&&(piminus_TRACK_Type==5)", 35);
    catTrackType.AddCondition("downlong", "(piplus_TRACK_Type==5)&&(piminus_TRACK_Type==3)", 53);
  }
}

void TriggerLeaves(Reducer* rdcr, cfg_tuple& cfg){}
void VetoLeaves(Reducer* rdcr, cfg_tuple& cfg){}
void AuxiliaryLeaves(Reducer* rdcr, cfg_tuple& cfg){}