/******************************************/
// MomentumArrayFlattenerGrimReaper.cpp
//
// Standalone GrimReaper that flattens
// array-kind momentum leaves
//
// Author: Frank Meier
// Date: 2017-08-15
/******************************************/

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/config/EasyConfig.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"
#include "dooselection/reducer/ReducerLeaf.h"
#include "dooselection/reducer/MomentumArrayFlattenerReducer.h"

// from BOOSGT
#include "boost/lexical_cast.hpp"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "MomentumArrayFlattenerGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, config_file_name;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    config_file_name = argv[5];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "MomentumArrayFlattenerGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "MomentumArrayFlattenerGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name config_file_name" << doocore::io::endmsg;
    return 1;
  }

  doocore::config::EasyConfig config(config_file_name);
  std::vector<std::string> threemomentumvariables = config.getVoStrings("threemomentumvariables");
  std::vector<std::string> fourmomentumvariables = config.getVoStrings("fourmomentumvariables");

  dooselection::reducer::MomentumArrayFlattenerReducer reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  reducer.Initialize();

  for(std::vector<std::string>::const_iterator var = threemomentumvariables.begin(); var != threemomentumvariables.end(); var++){
    if (reducer.LeafExists(*var)){
      reducer.AddThreeMomentumArrayLeaf(reducer.GetInterimLeafByName(*var));
    }
    else{
      doocore::io::swarn << "-warning- \t" << "MomentumArrayFlattenerGrimReaper \t" << "Variable " << *var << " not in tree!" << doocore::io::endmsg;
    }
  }
  for(std::vector<std::string>::const_iterator var = fourmomentumvariables.begin(); var != fourmomentumvariables.end(); var++){
    if (reducer.LeafExists(*var)){
      reducer.AddFourMomentumArrayLeaf(reducer.GetInterimLeafByName(*var));
    }
    else{
      doocore::io::swarn << "-warning- \t" << "MomentumArrayFlattenerGrimReaper \t" << "Variable " << *var << " not in tree!" << doocore::io::endmsg;
    }
  }

  reducer.Run();
  reducer.Finalize();

  doocore::io::sinfo << "-info-  \t" << "MomentumArrayFlattenerGrimReaper \t" << "Done!" << doocore::io::endmsg;
}