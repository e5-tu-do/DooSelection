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
#include "doocore/io/MsgStream.h"


// from DooSelection
#include "dooselection/reducer/TMVAClassificationReducer.h"

int main(int argc, char* argv[]){
  doocore::io::sinfo << "-info-  \t" << "TMVAGrimReaper \t" << "Welcome!" << doocore::io::endmsg;
  std::string inputfile, inputtree, outputfile, outputtree, config_file_name;

  if (argc == 2) {
    config_file_name = argv[1];
  }
  else if (argc == 6) {
    inputfile = argv[1];
    inputtree = argv[2];
    outputfile = argv[3];
    outputtree = argv[4];
    config_file_name = argv[5];
  }
  else {
    doocore::io::serr << "-ERROR- \t" << "TMVAGrimReaper \t" << "Parameters needed:" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "TMVAGrimReaper \t"<< "input_file_name input_tree_name output_file_name output_tree_name config_file_name" << doocore::io::endmsg;
    doocore::io::serr << "-ERROR- \t" << "TMVAGrimReaper \t"<< "config_file_name" << doocore::io::endmsg;
    return 0;
  }
  doocore::config::EasyConfig config(config_file_name);

  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  dooselection::reducer::TMVAClassificationReducer reducer;

  namespace fs = boost::filesystem;
  using namespace doocore::io;
  
  TString method(config.getString("method"));
  std::vector<std::string> methods;
  boost::property_tree::ptree pt = config.getPTree();
  if (pt.get_child_optional("methods")) methods = config.getVoStrings("methods");

  TString track_type(config.getString("track_type"));

  fs::path input_file(config.getString("input_file"));
  fs::path output_file(config.getString("output_file"));
  TString tree(config.getString("data_tree"));
  TString output_tree(config.getString("output_tree"));

  if (argc == 6) {
    input_file = fs::path(inputfile);
    output_file = fs::path(outputfile);
    tree = inputtree.c_str();
    output_tree = outputtree.c_str();
  }
  
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
  std::vector<std::string> xml_files;
  if (pt.get_child_optional("xml_files")) xml_files = config.getVoStrings("xml_files");

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
  if (pt.get_child_optional("methods") && pt.get_child_optional("xml_files")) {
    if (methods.size() == xml_files.size()) {
      reducer.SetTMVAMethodsAndXMLFiles(methods, xml_files);
    }
    else {
      std::cout << "The number of methods and xml files differs! Please change!" << std::endl;
      return 0;
    }
  }
  else reducer.SetTMVAMethodAndXMLFile(method, xml_file);

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

  std::vector<std::string> float_variables;
  if (pt.get_child_optional("variables.float")) float_variables = config.getVoStrings("variables.float");
  std::vector<std::string> integer_variables;
  if (pt.get_child_optional("variables.integer")) integer_variables = config.getVoStrings("variables.integer");
  std::vector<std::string> spectator_variables;
  if (pt.get_child_optional("variables.spectator")) spectator_variables = config.getVoStrings("variables.spectator");

  summary.AddSection("Float");
  for(std::vector<std::string>::iterator it = float_variables.begin(); it != float_variables.end(); it++){
    reducer.SetTMVAVariable((*it), reducer.GetInterimLeafByName((*it)));
    summary.Add("", *it);
  }

  summary.AddSection("Integer");
  for(std::vector<std::string>::iterator it = integer_variables.begin(); it != integer_variables.end(); it++){
    reducer.SetTMVAVariable((*it), reducer.GetInterimLeafByName((*it)));
    summary.Add("", *it);
  }

  summary.AddSection("Spectator");
  for(std::vector<std::string>::iterator it = spectator_variables.begin(); it != spectator_variables.end(); it++){
    reducer.SetTMVASpectatorVariable((*it), reducer.GetInterimLeafByName((*it)));
    summary.Add("", *it);
  }

  reducer.Run();
  reducer.Finalize();

  summary.Write("summary.log");
}
