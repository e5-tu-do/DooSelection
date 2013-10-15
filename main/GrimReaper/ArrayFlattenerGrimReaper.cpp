/******************************************/
// MultipleCandidateAnalyseGrimReaper.cpp
//
// Standalone GrimReaper that flattens
// array-kind leaves
//
// Author: Christophe Cauet
// Date: 2013-10-14
/******************************************/

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/config/EasyConfig.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"
#include "dooselection/reducer/ReducerLeaf.h"
#include "dooselection/reducer/ArrayFlattenerReducer.h"

// from BOOSGT
#include "boost/lexical_cast.hpp"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "ArrayFlattenerGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, config_file_name;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    config_file_name = argv[5];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "ArrayFlattenerGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "ArrayFlattenerGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name config_file_name" << doocore::io::endmsg;
    return 1;
  }

  doocore::config::EasyConfig config(config_file_name);
  std::vector<std::string> variables = config.getVoStrings("variables");

  dooselection::reducer::ArrayFlattenerReducer reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  reducer.set_name_array_index(config.getString("index_leaf_name"));

  reducer.Initialize();

  for(std::vector<std::string>::const_iterator var = variables.begin(); var != variables.end(); var++){
    if (reducer.LeafExists(*var)){
      reducer.AddArrayLengthLeaf(reducer.GetInterimLeafByName(*var));
    }
    else{
      doocore::io::swarn << "-warning- \t" << "ArrayFlattenerGrimReaper \t" << "Variable " << *var << " not in tree!" << doocore::io::endmsg;
    }
  }

  reducer.Run();
  reducer.Finalize();

  doocore::io::sinfo << "-info-  \t" << "ArrayFlattenerGrimReaper \t" << "Done!" << doocore::io::endmsg;
}