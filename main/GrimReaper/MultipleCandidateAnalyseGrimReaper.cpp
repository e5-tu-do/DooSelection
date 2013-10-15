/******************************************/
// MultipleCandidateAnalyseGrimReaper.cpp
//
// Standalone GrimReaper that counts the
// number of multiple candidates.
//
// Author: Christophe Cauet
// Date: 2013-10-15
/******************************************/

// from DooCore
#include "doocore/io/MsgStream.h"

// from DooSelection
#include "dooselection/reducer/MultipleCandidateAnalyseReducer.h"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "MultipleCandidateAnalyseGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree;
  if (argc == 5){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "MultipleCandidateAnalyseGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "MultipleCandidateAnalyseGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name" << doocore::io::endmsg;
    return 1;
  }

  dooselection::reducer::MultipleCandidateAnalyseReducer reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  reducer.Initialize();

  if (reducer.LeafExists("idxEventNumber")){
    reducer.AddEventIdentifier("idxEventNumber");
    reducer.AddEventIdentifier("idxRunNumber");
    if (reducer.LeafExists("idxPV")) reducer.AddEventIdentifier("idxPV");
  }
  else if (reducer.LeafExists("eventNumber")){
    doocore::io::swarn << "-warning- \t" << "MultiCandAnaRdcr: Leaf 'idxEventNumber' does not exists, using 'eventNumber' instead." << doocore::io::endmsg;
    reducer.AddEventIdentifier("eventNumber");
    reducer.AddEventIdentifier("runNumber");
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "MultiCandAnaRdcr: Leaves 'idxEventNumber' and 'eventNumber' do not exist!" << doocore::io::endmsg;
  }

  reducer.Run();
  reducer.Finalize();

  doocore::io::sinfo << "-info- \t" << "MultipleCandidateAnalyseGrimReaper \t" << "Done!" << doocore::io::endmsg;
}