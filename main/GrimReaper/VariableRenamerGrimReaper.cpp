/******************************************/
// VariableRenamerGrimReaper.cpp
//
// Standalone GrimReaper that renames
// variables according to a given config file
//
// Author: Christophe Cauet
// Date: 2013-10-15
/******************************************/

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/config/EasyConfig.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "VariableRenamerGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, config_file_name;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    config_file_name = argv[5];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "VariableRenamerGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "VariableRenamerGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name config_file_name" << doocore::io::endmsg;
    return 1;
  }

  doocore::config::EasyConfig config(config_file_name);
  std::vector<std::pair<std::string, std::string> > substitutions = config.getVoStringPairs("substitutions");

  dooselection::reducer::Reducer reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  for(std::vector<std::pair<std::string,std::string>>::const_iterator substitution = substitutions.begin(); substitution != substitutions.end(); substitution++){
    reducer.AddNameMapping((*substitution).first.c_str(), (*substitution).second.c_str());
    doocore::io::sinfo << "will substitute " << (*substitution).first.c_str() << " for " << (*substitution).second.c_str() << doocore::io::endmsg;
  }
  
  reducer.Initialize();
  reducer.Run();
  reducer.Finalize();

  doocore::io::sinfo << "-info- \t" << "VariableRenamerGrimReaper \t" << "Done!" << doocore::io::endmsg;
}
