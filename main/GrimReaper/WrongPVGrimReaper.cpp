/******************************************/
// WrongPVGrimReaper.cpp
//
// Standalone GrimReaper adds the minimal 
// IP chi2 w.r.t any PV to a tuple.
//
// Author: Christophe Cauet
// Date: 2013-10-14
//
//Updated by Vanessa Müller
//Date: 2016-05-04
/******************************************/

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/config/EasyConfig.h"

// from DooSelection
#include "dooselection/reducer/WrongPVReducer.h"

// from BOOSGT
#include "boost/lexical_cast.hpp"

using namespace doocore::io;
using namespace dooselection::reducer;


int main(int argc, char * argv[]){
  sinfo << "-info-  \t" << "WrongPVGrimReaper \t" << "Welcome!" << endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, config_file_name;
  if (argc == 6){
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    config_file_name = argv[5];
  }
  else{
    serr << "-ERROR- \t" << "WrongPVGrimReaper \t" << "Parameters needed:" << endmsg;
    serr << "-ERROR- \t" << "WrongPVGrimReaper \t" << "input_file_name input_tree_name output_file_name output_tree_name config_file_name" << endmsg;
    return 1;
  }

  doocore::config::EasyConfig config(config_file_name);
  std::string chi2_leaf_name  = config.getString("branches.chi2_leaf");
  std::string chi2_any_leaf_name = config.getString("branches.chi2_any_leaf");
  std::string idxPV_leaf_name = config.getString("branches.idxPV_leaf");  
  std::string pv_x_leaf_name = config.getString("branches.pv_x_leaf");
  std::string pv_x_var_leaf_name = config.getString("branches.pv_x_var_leaf");
  std::string pv_x_true_leaf_name = config.getString("branches.pv_x_true_leaf");
  std::string pv_y_leaf_name = config.getString("branches.pv_y_leaf");
  std::string pv_y_var_leaf_name = config.getString("branches.pv_y_var_leaf");
  std::string pv_y_true_leaf_name = config.getString("branches.pv_y_true_leaf");
  std::string pv_z_leaf_name = config.getString("branches.pv_z_leaf");
  std::string pv_z_var_leaf_name = config.getString("branches.pv_z_var_leaf");
  std::string pv_z_true_leaf_name = config.getString("branches.pv_z_true_leaf");

  sinfo << "chi2_leaf_name:  " << chi2_leaf_name << endmsg;
  sinfo << "chi2_any_leaf_name:  " << chi2_any_leaf_name << endmsg;

  dooselection::reducer::WrongPVReducer reducer;

  reducer.set_input_file_path(inputfile);
  reducer.set_input_tree_path(inputtree);
  reducer.set_output_file_path(outputfile);
  reducer.set_output_tree_path(outputtree);
  reducer.set_chi2_leaf_name(chi2_leaf_name);
  reducer.set_chi2_any_leaf_name(chi2_any_leaf_name);
  reducer.set_idxPV_leaf_name(idxPV_leaf_name);
  reducer.set_pv_x_leaf_name(pv_x_leaf_name);
  reducer.set_pv_x_var_leaf_name(pv_x_var_leaf_name);
  reducer.set_pv_x_true_leaf_name(pv_x_true_leaf_name);
  reducer.set_pv_y_leaf_name(pv_y_leaf_name);
  reducer.set_pv_y_var_leaf_name(pv_y_var_leaf_name);
  reducer.set_pv_y_true_leaf_name(pv_y_true_leaf_name);
  reducer.set_pv_z_leaf_name(pv_z_leaf_name);
  reducer.set_pv_z_var_leaf_name(pv_z_var_leaf_name);
  reducer.set_pv_z_true_leaf_name(pv_z_true_leaf_name);

  reducer.Initialize();
  reducer.Run();
  reducer.Finalize();

  sinfo << "-info- \t" << "WrongPVGrimReaper \t" << "Done!" << endmsg;
}
