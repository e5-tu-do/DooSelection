/******************************************/
// KeepListOfBranchesGrimReaper.cpp
//
// Standalone GrimReaper that removes all
// branches, except the ones given in a list.
//
// Author: Timon Schmelzer
// Date: 2016-12-02
//
// Example config file (in config.json):
// > keep_branches
// > {
// >     B_s0_MM
// >     B_s0_TAU
// >     B_s0_P
// > }
/******************************************/
// from BOOST

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/config/EasyConfig.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "KeepListOfBranchesGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, category_name, integer_value, config_file_name;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    config_file_name = argv[5];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "KeepListOfBranchesGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "KeepListOfBranchesGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name config_file_name" << doocore::io::endmsg;
    return 1;
  }


  doocore::config::EasyConfig config(config_file_name);
  std::vector<std::string> keep_branches = config.getVoStrings("keep_branches");

  dooselection::reducer::Reducer reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  for (auto&& keep_branch: keep_branches) {
    reducer.add_branch_keep(keep_branch);
  };

  reducer.Initialize();
  reducer.Run();
  reducer.Finalize();

  doocore::io::sinfo << "-info- \t" << "KeepListOfBranchesGrimReaper \t" << "Done!" << doocore::io::endmsg;
}
