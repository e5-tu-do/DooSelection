/******************************************/
// RemoveSingleBranchGrimReaper.cpp
//
// Standalone GrimReaper that removes a 
// single branch
//
// Author: Christophe Cauet
// Date: 2014-03-17
/******************************************/

// from DooCore
#include "doocore/io/MsgStream.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "RemoveSingleBranchGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, branch_name;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    branch_name = argv[5];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "RemoveSingleBranchGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "RemoveSingleBranchGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name branch_to_remove_name" << doocore::io::endmsg;
    return 1;
  }

  dooselection::reducer::Reducer reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  reducer.add_branch_omit(branch_name);

  reducer.Initialize();
  reducer.Run();
  reducer.Finalize();

  doocore::io::sinfo << "-info- \t" << "RemoveSingleBranchGrimReaper \t" << "Done!" << doocore::io::endmsg;
}
