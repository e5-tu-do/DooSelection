/******************************************/
// AddCategoryGrimReaper.cpp
//
// Standalone GrimReaper that adds a new leaf 
// and sets the entry to a given value
//
// Author: Christophe Cauet
// Date: 2014-03-07
/******************************************/

// from BOOST
#include "boost/lexical_cast.hpp"

// from DooCore
#include "doocore/io/MsgStream.h"

// from DooSelection
#include "dooselection/reducer/Reducer.h"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "AddCategoryGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, category_name, integer_value;
  if (argc == 7){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    category_name = argv[5];
    integer_value = argv[6];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "AddCategoryGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "AddCategoryGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name category_name integer_value" << doocore::io::endmsg;
    return 1;
  }

  doocore::io::sinfo << "-info-  \t" << "AddCategoryGrimReaper \t" << "New leaf: '" << category_name << "'' with default value: " <<  integer_value << doocore::io::endmsg;

  dooselection::reducer::Reducer reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  reducer.CreateIntLeaf(category_name.c_str(), boost::lexical_cast<int>(integer_value));

  reducer.Initialize();
  reducer.Run();
  reducer.Finalize();

  doocore::io::sinfo << "-info- \t" << "AddCategoryGrimReaper \t" << "Done!" << doocore::io::endmsg;
}
