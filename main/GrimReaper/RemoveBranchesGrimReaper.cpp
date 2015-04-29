/******************************************/
// RemoveBranchesGrimReaper.cpp
//
// Standalone GrimReaper that removes 
// branches matching a given regular expression
//
// Author: Frank Meier
// Date: 2015-04-27
/******************************************/

// from DooCore
#include "doocore/io/MsgStream.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "RemoveBranchesGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, branch_name;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    branch_name = argv[5];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "RemoveBranchesGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "RemoveBranchesGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name regular_expression_of_branches_to_remove" << doocore::io::endmsg;
    return 1;
  }

  dooselection::reducer::Reducer reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  reducer.AddBranchesOmitRegex(branch_name);

  reducer.Initialize();
  reducer.Run();
  reducer.Finalize();

  doocore::io::sinfo << "-info- \t" << "RemoveBranchesGrimReaper \t" << "Done!" << doocore::io::endmsg;
}
