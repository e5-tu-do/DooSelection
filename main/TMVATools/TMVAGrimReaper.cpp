// from STL

// from Boost
#ifdef __GNUG__
#define BOOST_NO_CXX11_SCOPED_ENUMS
#endif
#include <boost/filesystem.hpp>

// from ROOT
#include "TString.h"

// from RooFit

// from TMVA

// from BOOST

// from DooCore
#include "doocore/config/EasyConfig.h"
#include "doocore/config/Summary.h"

// from DooSelection
#include "dooselection/reducer/TMVAClassificationReducer.h"

// from here

int main(int argc, char* argv[]){
  if (argc != 2) {
    std::cout << "Usage:   " << argv[0] << " 'config_file_name'" << std::endl;
    return 0;
  }
  doocore::config::EasyConfig config(argv[1]);

  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  dooselection::reducer::TMVAClassificationReducer reducer;

  namespace fs = boost::filesystem;
  using namespace doocore::io;
  
  TString method(config.getString("method"));
  TString track_type(config.getString("track_type"));

  fs::path input_file(config.getString("input_file"));
  fs::path output_file(config.getString("output_file"));
  
  if (fs::exists(input_file)) {
    // new style input output files without enforcing arbitrary file naming
  } else {
    // old style input output files enforcing arbitrary file naming that can
    // get messy when applying multiple classifiers into one file

    swarn << "TMVAGrimReaper: You are using old style config files that enforce a naming scheme for output files." << endmsg;
    swarn << "             This functionality might be deprecated in a future release of TMVAGrimReaper." << endmsg;
    
    TString path(config.getString("data_path"));
    TString file(config.getString("data_file"));
    input_file = fs::path(path) / fs::path(file+".root");

    TString output_path(config.getString("output_path"));

    if (output_path == ""){
      output_path = path;
    }

    TString output_appendix(config.getString("output_appendix"));
    if (output_appendix == ""){
      output_appendix = method;
    }
    
    output_file = fs::path(output_path) / fs::path(file+"_"+track_type+"_"+output_appendix+".root");
  }
  
  TString xml_file(config.getString("xml_file"));
  TString tree(config.getString("data_tree"));
  TString output_tree(config.getString("output_tree"));
  if (output_tree == ""){
    output_tree = tree;
  }
  summary.Add("Method name", method);
  summary.Add("XML file", xml_file);
  summary.Add("Input file", input_file.string());
  summary.Add("Input tree", tree);
  summary.Add("Track type", track_type);
  summary.Add("Output file", output_file.string());
  summary.Add("Output tree", output_tree);
  //summary.Add("Output appendix", output_appendix);

  // TRACK_Type == 3 for Longtrack, TRACK_Type == 5 for Downstream
  if (track_type == "longtrack"){
    doocore::io::swarn << "Longtrack cuts applied!" << doocore::io::endmsg;
    reducer.set_cut_string("piminus_TRACK_Type==3 && piplus_TRACK_Type==3"); 
    track_type = "_"+track_type; 
  }
  else if (track_type == "downstream"){
    doocore::io::swarn << "Downstream cuts applied!" << doocore::io::endmsg;
    reducer.set_cut_string("piminus_TRACK_Type==5 && piplus_TRACK_Type==5");
    track_type = "_"+track_type;
  }
  else{
    doocore::io::swarn << "No downstream or longtrack cuts applied!" << doocore::io::endmsg;
    track_type = "";
  }

  // Register MVA method and XML file
  reducer.SetTMVAMethodAndXMLFile(method, xml_file);

  // Register input file
  reducer.set_input_file_path(input_file.string());
  reducer.set_input_tree_path(tree);
  reducer.PrepareIntitialTree();

  // Set output file
  reducer.set_output_file_path(output_file.string());
  reducer.set_output_tree_path(output_tree);
  reducer.PrepareFinalTree();

  // Register input variables
  summary.AddSection("Variables");

  std::vector<std::string> float_variables = config.getVoStrings("variables.float");
  std::vector<std::string> integer_variables = config.getVoStrings("variables.integer");

  for(std::vector<std::string>::iterator it = float_variables.begin(); it != float_variables.end(); it++){
    reducer.SetTMVAVariable((*it), reducer.GetInterimLeafByName((*it)));
    summary.Add("", *it);
  }

  for(std::vector<std::string>::iterator it = integer_variables.begin(); it != integer_variables.end(); it++){
    reducer.SetTMVAVariable((*it), reducer.GetInterimLeafByName((*it)));
    summary.Add("", *it);
  }

  reducer.Run();
  reducer.Finalize();

  summary.Write("summary.log");
}
