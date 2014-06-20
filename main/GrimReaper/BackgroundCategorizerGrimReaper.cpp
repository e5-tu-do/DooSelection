/******************************************/
// BackgroundCategorizerGrimReaper.cpp
//
// Standalone GrimReaper analizes the background
// composition of a given tuple
//
// Author: Christophe Cauet
// Date: 2014-06-20
/******************************************/

// from DooSelection
#include <dooselection/reducer/BkgCategorizerReducer2.h>

// from BOOST
#include "boost/lexical_cast.hpp"

int main(int argc, char * argv[]){
  doocore::io::sinfo << "-info-  \t" << "BackgroundCategorizerGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree;
  int max_number_of_decays = 40;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    max_number_of_decays = boost::lexical_cast<int>(argv[5]);
  }
  if (argc == 5){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "BackgroundCategorizerGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "BackgroundCategorizerGrimReaper \t"<< "input_file_name input_tree_name output_file_name (optional: max_number_of_decays)" << doocore::io::endmsg;
    return 1;
  }
  dooselection::reducer::BkgCategorizerReducer2 reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);

  reducer.Initialize();

  //in the following the names of some variables are set to their names in the tuple. this only needs to be updated if TupleToolMCDecayTree is changed somewhen in the future
  if (reducer.LeafExists("B0_DECAYDEPTH")) {
    reducer.set_decay_matrix_name("B0_DECAYMTRX");
    reducer.set_decay_matrix_length_leaf<Float_t>(reducer.GetInterimLeafByName("B0_DECAYMTRX_LENGTH"));
    reducer.set_decay_depth_leaf<Float_t>(reducer.GetInterimLeafByName("B0_DECAYDEPTH"));
    reducer.set_decay_products_leaf<Float_t>(reducer.GetInterimLeafByName("B0_DECAYPRODUCTS"));
    dooselection::reducer::ReducerLeaf<Int_t>& bkgcat_leaf = reducer.CreateIntLeaf("bkgCategorizedDecays");
    reducer.set_background_category_leaf(bkgcat_leaf);
  }
  else{
    doocore::io::serr << "Leaf 'B0_DECAYDEPTH' does not exist!" << doocore::io::endmsg;
    return 1;
  }
  
  //in the following a vector of three particle IDs is created (gamma(-->22), mu(-->13), nu_mu(-->14)), that will be ignored in the counting process, i.e. they are left out when comparing decays with each other.
  std::vector<int> ignored_particles_ids;
  ignored_particles_ids.push_back(22);
  ignored_particles_ids.push_back(13);
  ignored_particles_ids.push_back(14);
  reducer.set_vec_of_ignored_particles(ignored_particles_ids);
  doocore::io::scfg << "Ignored particles: " << ignored_particles_ids << doocore::io::endmsg;
  
  //the number of different decays that are considered can be set as follows. Standard is 40.
  reducer.set_max_number_of_decays(max_number_of_decays);
  doocore::io::scfg << "Maximal number of considered decays : " << max_number_of_decays << doocore::io::endmsg;

  //it is possible to set the behaviour in terms of charge conjugated particles, i.e. if charge conjugated decays are counted as the same decay or if they are counted independently. For more details look into the documentation of BkgCategorizerReducer2. The default ist ChargesIrrel
  reducer.set_mode(dooselection::reducer::ChargesIrrel);

  reducer.Run();
  reducer.Finalize();

  doocore::io::sinfo << "-info-  \t" << "BackgroundCategorizerGrimReaper \t" << "Done!" << doocore::io::endmsg;
}
