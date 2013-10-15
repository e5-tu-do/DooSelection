/******************************************/
// MultiCutGrimReaper.cpp
//
// Standalone GrimReaper that performs cuts
// according to a provided config file.
//
// Author: Christophe Cauet
// Date: 2013-10-14
/******************************************/

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/config/EasyConfig.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "MultiCutGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, config_file_name;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    config_file_name = argv[5];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "MultiCutGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "MultiCutGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name config_file_name" << doocore::io::endmsg;
    return 1;
  }

  doocore::config::EasyConfig config(config_file_name);
  std::vector<std::string> cuts = config.getVoStrings("cuts");

  dooselection::reducer::Reducer reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  std::string cut_string = "";
  for(std::vector<std::string>::const_iterator cut = cuts.begin(); cut != cuts.end(); cut++){
    cut_string+=*cut;
    if (cut!=cuts.end()-1) cut_string+="&&";
  }
  reducer.set_cut_string(cut_string);

  reducer.Initialize();
  reducer.Run();
  reducer.Finalize();

  doocore::io::sinfo << "-info- \t" << "MultiCutGrimReaper \t" << "Done!" << doocore::io::endmsg;
}
