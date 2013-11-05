/******************************************/
// CandidateSelectionGrimReaper.cpp
//
// Standalone GrimReaper that selects between
// multiple candidates depending on a 'best'
// or 'random' selection.
//
// Author: Christophe Cauet
// Date: 2013-10-15
/******************************************/

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/config/EasyConfig.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"

// from BOOSGT
#include "boost/lexical_cast.hpp"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "CandidateSelectionGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, best_or_random;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    best_or_random = argv[5];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "CandidateSelectionGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "CandidateSelectionGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name best_or_random" << doocore::io::endmsg;
    return 1;
  }

  dooselection::reducer::Reducer reducer;
  
  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  reducer.Initialize();

  if (reducer.LeafExists("idxEventNumber")){
    reducer.SetEventNumberLeaf<Float_t>(reducer.GetInterimLeafByName("idxEventNumber"));
    reducer.SetRunNumberLeaf<Float_t>(reducer.GetInterimLeafByName("idxRunNumber"));
  }
  else if (reducer.LeafExists("eventNumber")){
    doocore::io::swarn << "-warning- \t" << "CandidateSelectionGrimReaper: Leaf 'idxEventNumber' does not exists, using 'eventNumber' instead." << doocore::io::endmsg;
    reducer.SetEventNumberLeaf<Float_t>(reducer.GetInterimLeafByName("eventNumber"));
    reducer.SetRunNumberLeaf<Float_t>(reducer.GetInterimLeafByName("runNumber"));
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "CandidateSelectionGrimReaper: Leaves 'idxEventNumber' and 'eventNumber' do not exist!" << doocore::io::endmsg;
    abort();
  }

  if (best_or_random == "best"){
    if (reducer.LeafExists("varDTFChi2ndof")){
      reducer.SetBestCandidateLeaf<Float_t>(reducer.GetInterimLeafByName("varDTFChi2ndof"));
    }
    else if (reducer.LeafExists("varChi2ndof")){
      doocore::io::swarn << "-warning- \t" << "CandidateSelectionGrimReaper: Leaf 'varDTFChi2ndof' does not exists, using 'varChi2ndof' instead." << doocore::io::endmsg;
      reducer.SetBestCandidateLeaf<Float_t>(reducer.GetInterimLeafByName("varChi2ndof"));
    } 
    else{
      doocore::io::serr << "-ERROR- \t" << "CandidateSelectionGrimReaper: Leaves 'varDTFChi2ndof' and 'varChi2ndof' do not exist!" << doocore::io::endmsg;
      abort();
    }
  }
  else if (best_or_random == "random"){
    if (reducer.LeafExists("idxRandom")){
      reducer.SetBestCandidateLeaf<Float_t>(reducer.GetInterimLeafByName("idxRandom"));
    } 
    else{
      doocore::io::serr << "-ERROR- \t" << "CandidateSelectionGrimReaper: Leaf 'idxRandom' does not exist!" << doocore::io::endmsg;
      abort();
    }
  }
  
  reducer.Run();
  reducer.Finalize();

  doocore::io::sinfo << "-info-  \t" << "CandidateSelectionGrimReaper \t" << "Done!" << doocore::io::endmsg;
}