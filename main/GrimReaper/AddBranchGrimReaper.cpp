/******************************************/
// AddBranchGrimReaper.cpp
//
// Standalone GrimReaper that copies a branch 
// of a given TTree to another one, if the number
// of entries are the same.
//
// Author: Christophe Cauet
// Date: 2015-03-27
/******************************************/

// from BOOST
#include "boost/lexical_cast.hpp"

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/io/Progress.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"

// from ROOT
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "AddBranchGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, inputbranch, outputbranch;
  if (argc == 7){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    inputbranch = argv[5];
    outputbranch = argv[6];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "AddBranchGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "AddBranchGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name input_branch_name output_branch_name" << doocore::io::endmsg;
    return 1;
  }

  TFile infile(inputfile.c_str(), "read"),
        outfile(outputfile.c_str(), "update");
  TTree * intree{dynamic_cast<TTree*>(infile.Get(inputtree.c_str()))},
        * outtree{dynamic_cast<TTree*>(outfile.Get(outputtree.c_str()))};
  
  long long events_intree{intree->GetEntries()},
            events_outtree{outtree->GetEntries()};
  
  if (events_intree != events_outtree) {
    doocore::io::serr << "-ERROR- \t" << "AddBranchGrimReaper \t"
    << "Inputtree and outputtree are not equally sized!" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "AddBranchGrimReaper \t"
    << "Events in inputtree: \t" << intree->GetEntries() << ", events in outputtree: \t" << outtree->GetEntries() << doocore::io::endmsg;
    return 1;
  }
  
  double in_event, out_event;
  intree->SetBranchAddress(inputbranch.c_str(), &in_event);
  TBranch * outbranch{outtree->Branch(outputbranch.c_str(), &out_event)};
  
  doocore::io::Progress p("Looping over tree...", events_intree);
  for (long long cnt = 0; cnt < std::max(events_intree, events_outtree); cnt++) {
    ++p;
    intree->GetEntry(cnt);
    out_event = in_event;
    outbranch->Fill();
  }
  p.Finish();
  
  outtree->Write();

  delete intree,
  delete outtree;
  
  doocore::io::sinfo << "-info-  \t" << "AddBranchGrimReaper \t" << "Added new branch " << outputbranch
  << " to tree " << outputtree << " in " << outputfile << doocore::io::endmsg;
  doocore::io::sinfo << "-info-  \t" << "AddBranchGrimReaper \t" << "Done!" << doocore::io::endmsg;

  return 0;
}