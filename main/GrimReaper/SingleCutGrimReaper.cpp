// from DooCore
#include "doocore/io/MsgStream.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "SingleCutGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, cut;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    cut = argv[5];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "SingleCutGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "SingleCutGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name cut_string" << doocore::io::endmsg;
    return 1;
  }

  dooselection::reducer::Reducer reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  reducer.set_cut_string(cut);

  reducer.Initialize();
  reducer.Run();
  reducer.Finalize();

  doocore::io::sinfo << "-info- \t" << "SingleCutGrimReaper \t" << "Done!" << doocore::io::endmsg;
}
