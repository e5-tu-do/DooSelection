// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/config/EasyConfig.h"

// from DooSelection
#include "dooselection/reducer/VariableCategorizerReducer.h"

// from BOOSGT
#include "boost/lexical_cast.hpp"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "VariableCategorizerGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, config_file_name;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    config_file_name = argv[5];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "VariableCategorizerGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "VariableCategorizerGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name config_file_name" << doocore::io::endmsg;
    return 1;
  }

  doocore::config::EasyConfig config(config_file_name);
  std::vector<std::string> variables = config.getVoStrings("variables");
  dooselection::reducer::VariableCategorizerReducer reducer(config.getString("prefix"));

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  for(std::vector<std::string>::const_iterator var = variables.begin(); var != variables.end(); var++){
    std::vector<std::string> nbins = config.getVoStrings("variables."+(*var)+".bins");
    double min = config.getDouble("variables."+(*var)+".min");
    double max = config.getDouble("variables."+(*var)+".max");
    for(std::vector<std::string>::const_iterator bins = nbins.begin(); bins != nbins.end(); bins++){
      doocore::io::sinfo << "-info-  \t" << "VariableCategorizerGrimReaper \t" << "Variable: " << *var << " (" << boost::lexical_cast<int>(*bins) << " bin in range " << min << " - " << max << ")" << doocore::io::endmsg;
      reducer.set_variable(*var, boost::lexical_cast<int>(*bins), min, max);
    }
  }
  
  reducer.Initialize();
  reducer.Run();
  reducer.Finalize();

  doocore::io::sinfo << "-info-  \t" << "VariableCategorizerGrimReaper \t" << "Done!" << doocore::io::endmsg;
}
