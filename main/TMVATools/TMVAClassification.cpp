// from STL

// from ROOT
#include "TString.h"
#include "TFile.h"
#include "TApplication.h"
#include "TTreeFormula.h"

// from RooFit

// from TMVA
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/Config.h"
#include "TMVAGui.C"

// from BOOST
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/config/EasyConfig.h"
#include "doocore/config/Summary.h"
#include "doocore/system/Tools.h"

// from here

int main(int argc, char * argv[]){
  if (argc != 2) {
    std::cout << "Usage:   " << argv[0] << " config_file_name" << std::endl;
    return 0;
  }
  doocore::config::EasyConfig config(argv[1]);
  doocore::config::Summary& summary = doocore::config::Summary::GetInstance();
  summary.Add("Config file", argv[1]);

  //===========================================================================
  // Initializing variables and constants
  //===========================================================================
  TString job_name(config.getString("general.name"));
  summary.Add("Job name", job_name);

  bool debug_mode(config.getBool("general.debug_mode"));
  summary.Add("Debug mode", std::to_string(debug_mode));

  summary.AddSection("Input");
  TString input_path(config.getString("general.input.path"));
  summary.Add("Path", input_path);

  TString input_file_name;
  TString input_tree_name;
  TString input_sig_file_name;
  TString input_bkg_file_name;
  TString input_sig_tree_name;
  TString input_bkg_tree_name;
  TString sig_sweight;
  TString bkg_sweight;
  TString sig_cut;
  TString sig_train_cut;
  TString sig_test_cut;
  TString bkg_train_cut;
  TString bkg_test_cut;
  TString bkg_cut;

  if (config.getBool("general.use_single_input_file")){
    input_file_name = config.getString("general.input.file");
    input_tree_name = config.getString("general.input.tree");

    if ((input_file_name == "") || (input_tree_name == "") ){
      doocore::io::serr << "-ERROR- " << "When use single input file, config objects: 'general.input.file' and 'general.input.tree' have to be defined!" << doocore::io::endmsg;
      return 0;
    }
    summary.Add("File name", input_file_name);
    summary.Add("Tree name", input_tree_name);

    if (config.getBool("general.use_cuts")){
      sig_cut = config.getString("general.input.cuts");
      bkg_cut = config.getString("general.input.cuts");
      summary.Add("Cut on signal sample", sig_cut);
      summary.Add("Cut on background sample", bkg_cut);
    }

    sig_sweight = config.getString("general.input.sig_sweight");
    bkg_sweight = config.getString("general.input.bkg_sweight");
    if (sig_sweight == "") sig_sweight = "sweight_sig";
    if (bkg_sweight == "") bkg_sweight = "sweight_bkg";
    summary.Add("Signal sWeight", sig_sweight);
    summary.Add("Background sWeight", bkg_sweight);
  }
  else{
    input_sig_file_name = config.getString("general.input.sig_file");
    input_bkg_file_name = config.getString("general.input.bkg_file");
    input_sig_tree_name = config.getString("general.input.sig_tree");
    input_bkg_tree_name = config.getString("general.input.bkg_tree");
    if ((input_sig_file_name == "") || (input_bkg_file_name == "") || (input_sig_tree_name == "") || (input_bkg_tree_name == "") ){
      doocore::io::serr << "-ERROR- " << "When use different input files for signal and background, config objects: 'general.input.sig_file', 'general.input.bkg_file', 'general.input.sig_tree' and 'general.input.bkg_tree' have to be defined!" << doocore::io::endmsg;
      return 0;
    }
    summary.Add("Signal file name", input_sig_file_name);
    summary.Add("Signal tree name", input_sig_tree_name);
    summary.Add("Background file name", input_bkg_file_name);
    summary.Add("Background tree name", input_bkg_tree_name);

    if (config.getBool("general.use_cuts")){
      sig_cut = config.getString("general.input.cuts");
      bkg_cut = config.getString("general.input.cuts");
      summary.Add("Cut on signal sample", sig_cut);
      summary.Add("Cut on background sample", bkg_cut);
    }
    if (config.getBool("general.use_train_test_cuts")){
      sig_train_cut = config.getString("general.input.train_cut");
      sig_test_cut = config.getString("general.input.test_cut");
      summary.Add("Cut for training sample", sig_train_cut);
      summary.Add("Cut for testing sample", sig_test_cut);
    }
  }

  summary.AddSection("Output");
  TString output_path("tmva/"+config.getString("general.output.path"));
  summary.Add("Path", output_path);

  // copy config file to output directory
  doocore::system::tools::CopyFileToDirectory(argv[1], (std::string)output_path+"/"+(std::string)job_name+"/Config");

  summary.AddSection("Factory");
  TString factory_options(config.getString("factory.options"));
  summary.Add("Options", factory_options);
  TString split_options(config.getString("factory.split_options"));
  summary.Add("Split options", split_options);

  //===========================================================================
  // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
  //===========================================================================
  if (debug_mode) doocore::io::serr << "-debug- " << "output file" << doocore::io::endmsg;

  TFile * output_file  = new TFile(TString(output_path)+"/"+job_name+"/"+"tmva_classification.root", "RECREATE" );

  // set directory where the weight files are stored
  (TMVA::gConfig().GetIONames()).fWeightFileDir = TString(output_path)+"/"+job_name+"/Weights";

  //===========================================================================
  // Create the factory object
  //===========================================================================
  if (debug_mode) doocore::io::serr << "-debug- " << "create factory" << doocore::io::endmsg;

  TMVA::Factory * factory = new TMVA::Factory( "tmva", output_file, factory_options);

  //===========================================================================
  // Register input variables
  //===========================================================================
  if (debug_mode) doocore::io::serr << "-debug- " << "add variables" << doocore::io::endmsg;

  summary.AddSection("Variables");
  std::vector<std::string> float_variables = config.getVoStrings("variables.float");
  std::vector<std::string> integer_variables = config.getVoStrings("variables.integer");

  for(std::vector<std::string>::iterator it = float_variables.begin(); it != float_variables.end(); it++){
    factory->AddVariable(*it);
    summary.Add("", *it);
  }

  for(std::vector<std::string>::iterator it = integer_variables.begin(); it != integer_variables.end(); it++){
    factory->AddVariable(*it, 'I');
    summary.Add("", *it);
  }

  factory->SetWeightExpression(sig_sweight, "Signal");
  factory->SetWeightExpression(bkg_sweight, "Background");

  //===========================================================================
  // Specifying classification training data
  //===========================================================================
  TFile * file;
  TTree * tree;
  TFile * sig_file = NULL;
  TTree * sig_tree = NULL;
  TTree * sig_train_tree = NULL;
  TTree * sig_test_tree = NULL;
  TFile * bkg_file = NULL;
  TTree * bkg_tree = NULL;
  TTree * bkg_train_tree = NULL;
  TTree * bkg_test_tree = NULL;

  if (config.getBool("general.use_single_input_file")){
    if (debug_mode) doocore::io::serr << "-debug- " << "use single input file" << doocore::io::endmsg;
    //---------------------------------------------------------------------------
    // Registration of a single ROOT tree containing the input data for signal and background
    //---------------------------------------------------------------------------
    file = TFile::Open(input_path+input_file_name);
    tree = (TTree*)file->Get(input_tree_name);

    std::vector<std::string> cut_variables;
    if (config.getBool("general.use_cuts")){
      TTreeFormula form("form_cut", sig_cut, tree);
      TLeaf* leaf = form.GetLeaf(0);
      int i = 1;
      while (leaf != NULL) {
        cut_variables.push_back(leaf->GetName());
        leaf = form.GetLeaf(i);
        ++i;
      }
    }
    if (config.getBool("general.use_train_test_cuts")){
      TTreeFormula form("form_cut", sig_train_cut, tree);
      TLeaf* leaf = form.GetLeaf(0);
      int i = 1;
      while (leaf != NULL) {
        cut_variables.push_back(leaf->GetName());
        leaf = form.GetLeaf(i);
        ++i;
      }
    }
    tree->SetBranchStatus("*", false);

    tree->SetBranchStatus(sig_sweight, true);
    tree->SetBranchStatus(bkg_sweight, true);

    for (std::vector<std::string>::const_iterator it = float_variables.begin(), end = float_variables.end();
         it != end; ++it) {
      tree->SetBranchStatus(it->c_str(), true);
    }
    for (std::vector<std::string>::const_iterator it = integer_variables.begin(), end = integer_variables.end();
         it != end; ++it) {
      tree->SetBranchStatus(it->c_str(), true);
    }
    if (config.getBool("general.use_cuts")){
      for (std::vector<std::string>::const_iterator it = cut_variables.begin(), end = cut_variables.end();
           it != end; ++it) {
        tree->SetBranchStatus(it->c_str(), true);
      }
    }

    /// ##################################################################################################################
    /// The root of the problem is that TMVA creates a copy of the user trees if a tree is provided with a cut condition.
    /// TMVA uses TTree::CopyTree for this.
    /// If the input tree is very large this might cause memory problems and generate warning messages such as:
    /// >> Error in <TTree::Fill>: Failed filling branch:Bd2JpsiKstar.B0_ENDVERTEX_COV_, nbytes=-1, entry=24456
    /// >>  This error is symptomatic of a Tree created as a memory-resident Tree
    /// >>  Instead of doing:
    /// >>     TTree *T = new TTree(...)
    /// >>     TFile *f = new TFile(...)
    /// >>  you should do:
    /// >>     TFile *f = new TFile(...)
    /// >>     TTree *T = new TTree(...)
    /// ##################################################################################################################

    // TTree::CopyTree can use this file to temporarily store the TTree
    TFile *interim_file = new TFile("interim_file", "RECREATE");

    //Set input trees
    factory->SetInputTrees(tree, (TCut)sig_cut, (TCut)bkg_cut);
  }
  else {
    if (debug_mode) doocore::io::serr << "-debug- " << "use different input files for signal and background" << doocore::io::endmsg;
    //---------------------------------------------------------------------------
    // Registration of signal and background ROOT trees from different TFile sources
    //---------------------------------------------------------------------------
    sig_file = TFile::Open(input_path+input_sig_file_name);
    sig_tree = (TTree*)sig_file->Get(input_sig_tree_name);

    bkg_file = TFile::Open(input_path+input_bkg_file_name);
    bkg_tree = (TTree*)bkg_file->Get(input_bkg_tree_name);

    // Define signal and background weights
    Double_t kSigWeight = 1.0;
    Double_t kBkgWeight = 1.0;

    if (!(config.getBool("general.use_cuts"))){
      // Set input trees
      if(config.getBool("general.use_train_test_cuts")){
        sig_train_tree = sig_tree->CopyTree(config.getString("general.input.train_cut").c_str());
        sig_test_tree = sig_tree->CopyTree(config.getString("general.input.test_cut").c_str());
        factory->AddSignalTree(sig_train_tree, kSigWeight, "Training");
        factory->AddSignalTree(sig_test_tree, kSigWeight, "Testing");
        bkg_train_tree = bkg_tree->CopyTree(config.getString("general.input.train_cut").c_str());
        bkg_test_tree = bkg_tree->CopyTree(config.getString("general.input.test_cut").c_str());
        factory->AddBackgroundTree(bkg_train_tree, kBkgWeight, "Training");
        factory->AddBackgroundTree(bkg_test_tree, kBkgWeight, "Testing");
      }
      else{
        factory->AddSignalTree(sig_tree, kSigWeight);
        factory->AddBackgroundTree(bkg_tree, kBkgWeight);
      }
    }
    else {
      sig_tree->SetBranchStatus("*", false);
      bkg_tree->SetBranchStatus("*", false);

      for (std::vector<std::string>::const_iterator it = float_variables.begin(), end = float_variables.end(); it != end; ++it) {
        sig_tree->SetBranchStatus(it->c_str(), true);
        bkg_tree->SetBranchStatus(it->c_str(), true);
      }
      for (std::vector<std::string>::const_iterator it = integer_variables.begin(), end = integer_variables.end(); it != end; ++it) {
        sig_tree->SetBranchStatus(it->c_str(), true);
        bkg_tree->SetBranchStatus(it->c_str(), true);
      }

      sig_tree->SetBranchStatus("Index", true);
      bkg_tree->SetBranchStatus("Index", true);

      TFile *interim_file = new TFile("interim_file", "RECREATE");

      // Set input trees
      if(config.getBool("general.use_train_test_cuts")){
        sig_train_cut = config.getString("general.input.train_cut") + "&&" + sig_cut;
        sig_test_cut = config.getString("general.input.test_cut") + "&&" + sig_cut;
        sig_train_tree = sig_tree->CopyTree(sig_train_cut.Data());
        sig_test_tree = sig_tree->CopyTree(sig_test_cut.Data());
        factory->AddSignalTree(sig_train_tree, kSigWeight, "Training");
        factory->AddSignalTree(sig_test_tree, kSigWeight, "Testing");
        bkg_train_cut = config.getString("general.input.train_cut") + "&&" + bkg_cut;
        bkg_test_cut = config.getString("general.input.test_cut") + "&&" + bkg_cut;
        bkg_train_tree = bkg_tree->CopyTree(bkg_train_cut.Data());
        bkg_test_tree = bkg_tree->CopyTree(bkg_test_cut.Data());
        factory->AddBackgroundTree(bkg_train_tree, kBkgWeight, "Training");
        factory->AddBackgroundTree(bkg_test_tree, kBkgWeight, "Testing");
      }
      else{
        factory->AddTree(sig_tree, "Signal", kSigWeight, TCut(sig_cut));
        factory->AddTree(bkg_tree, "Background", kBkgWeight, TCut(bkg_cut));
      }
    }
  }

  //===========================================================================
  // Prepare training and testing
  //===========================================================================
  if (debug_mode) doocore::io::serr << "-debug- " << "prepare training and testing" << doocore::io::endmsg;

  TStopwatch sw;
  sw.Start();
  factory->PrepareTrainingAndTestTree("", "", split_options);

  //===========================================================================
  // Select MVA methods
  //===========================================================================
  if (debug_mode) doocore::io::serr << "-debug- " << "select mva methods" << doocore::io::endmsg;

  summary.AddSection("MVA methods");
  std::vector<std::string> methods = config.getVoStrings("factory.methods");;

  for(std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++){
    boost::regex expr("(.*)\\s*/{1}\\s*(.*)\\s*/{1}\\s*(.*)");
    boost::match_results<std::string::const_iterator> what;
    TMVA::Types::EMVA method_type;

    if (debug_mode) doocore::io::serr << *it << doocore::io::endmsg;
    if( regex_search( *it, what, expr ) ) {
      std::string type( what[1].first, what[1].second );
      std::string name( what[2].first, what[2].second );
      std::string options( what[3].first, what[3].second );

      if (debug_mode) doocore::io::serr << "-debug- " << "Using method '" << type << "' with name '" << name << "' and options '" << options << "'." << doocore::io::endmsg;

      if (type == "TMVA::Types::kBDT"){method_type = TMVA::Types::kBDT;}
      else if (type == "TMVA::Types::kFisher"){method_type = TMVA::Types::kFisher;}
      else if (type == "TMVA::Types::kMLP"){method_type = TMVA::Types::kMLP;}
      else if (type == "TMVA::Types::kTMlpANN"){method_type = TMVA::Types::kTMlpANN;}
      else if (type == "TMVA::Types::kCFMlpANN"){method_type = TMVA::Types::kCFMlpANN;}
      else if (type == "TMVA::Types::kLikelihood"){method_type = TMVA::Types::kLikelihood;}
      else if (type == "TMVA::Types::kCuts"){method_type = TMVA::Types::kCuts;}
      else if (type == "TMVA::Types::kPDEFoam"){method_type = TMVA::Types::kPDEFoam;}
      else if (type == "TMVA::Types::kKNN"){method_type = TMVA::Types::kKNN;}
      else if (type == "TMVA::Types::kSVM"){method_type = TMVA::Types::kSVM;}
      else if (type == "TMVA::Types::kRuleFit"){method_type = TMVA::Types::kRuleFit;}
      else{doocore::io::serr << "Unknown TMVA method type '" << type << "'! Abort!" << doocore::io::endmsg; return 0;}

      factory->BookMethod(method_type, name, options);
      summary.Add(name, options);
    }
    else{
      doocore::io::serr << "RegEx matching failed" << doocore::io::endmsg;
    }
  }

  //===========================================================================
  // Train, Test, and Evaluate
  //===========================================================================
  if (debug_mode) doocore::io::serr << "-debug- " << "train, test, and evaluate" << doocore::io::endmsg;

  if (config.getBool("factory.use_optimization")){
    doocore::io::sinfo << "Optimize all methods" << doocore::io::endmsg;
    factory->OptimizeAllMethods();
    doocore::io::sinfo << "Optimize all methods finished" << doocore::io::endmsg;
  }
  if (debug_mode) doocore::io::serr << "-debug- " << "Start training" << doocore::io::endmsg;
  factory->TrainAllMethods();
  if (debug_mode) doocore::io::serr << "-debug- " << "Start testing" << doocore::io::endmsg;
  factory->TestAllMethods();
  if (debug_mode) doocore::io::serr << "-debug- " << "Start evaluation" << doocore::io::endmsg;
  factory->EvaluateAllMethods();

  // Runtime
  sw.Stop();
  doocore::io::sinfo << "RUNTIME: " << sw << doocore::io::endmsg;

  output_file->Close();

  //summary.Print();
  summary.Write((std::string)output_path+"/"+(std::string)job_name+"/"+"summary.log");

  if(config.getBool("general.show_gui")){
    if (debug_mode) doocore::io::serr << "-debug- " << "Start TMVAGui" << doocore::io::endmsg;
    TApplication* theApp;
    theApp = new TApplication("App", &argc, argv);

    TMVAGui(TString(output_path)+"/"+job_name+"/"+"tmva_classification.root");

    theApp->Run();
  }
  if (debug_mode) doocore::io::serr << "-debug- " << "End of program" << doocore::io::endmsg;
}




