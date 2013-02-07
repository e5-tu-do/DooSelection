#include "performance.h"
#include "SelectionTuple/SelectionTuple.h"
#include "SelectionClassifier/SelectionClassifier.h"

// from STL
#include <vector>
#include <map>
#include <string>

// from ROOT
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1D.h"
#include "TTree.h"

// from RooFit
#include "RooAbsPdf.h"
#include "RooArgList.h"
#include "RooAbsData.h"
#include "RooDataSet.h"
#include "RooAbsArg.h"

// from TMVA

// from BOOST

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/lutils/lutils.h"
#include "doocore/io/Tools.h"
#include "doocore/system/Tools.h"

// from DooFit
#include "doofit/builder/EasyPdf/EasyPdf.h"
#include "doofit/plotting/Plot/Plot.h"
#include "doofit/plotting/Plot/PlotConfig.h"

// from DooSelection

// forward declarations

namespace dooselection{
namespace performance{

// ==============================
// NUMBER OF EVENTS PER COMPONENT
// ==============================
std::map<std::string, double> NumberOfEventsPerComponent(SelectionTuple &stuple, std::string cut_string, bool debug_mode){
  if (debug_mode) if (cut_string!="") doocore::io::serr << "-debug- " << "starting doocore::performance::NumberOfEventsPerComponent() with cut '" << cut_string << "'…" << doocore::io::endmsg;
  if (debug_mode) if (cut_string=="") doocore::io::serr << "-debug- " << "starting doocore::performance::NumberOfEventsPerComponent() without cut" << doocore::io::endmsg;
  std::map<std::string, double> components_and_yield_values;

  if(stuple.use_mc()){
    doocore::io::serr << "-ERROR- \t" << "NumberOfEventsPerComponent(SelectionTuple &stuple, std::string cut_string, bool debug_mode)- MC not yet implemented" << doocore::io::endmsg;
    return components_and_yield_values;
  } 
  else if(stuple.use_sweights()){
    if (debug_mode) doocore::io::serr << "-debug- \t" << "using sweights…" << doocore::io::endmsg;
    /// number of bins used
    int nbins = 100;

    /// loop over components
    for(std::map< std::string, std::string >::const_iterator it = stuple.map_of_components_and_sweights().begin(); it != stuple.map_of_components_and_sweights().end(); it++){
      double number_of_events = 0.;

      /// set sweight for current component
      std::string sweight = (*it).second;
      if (debug_mode) doocore::io::serr << "-debug- \t" << "sweight: " << sweight << doocore::io::endmsg;
      
      TH1D* hist = new TH1D("hist", "hist", nbins, stuple.sweight_min(), stuple.sweight_max());

      TCanvas * canvas = new TCanvas("canvas", "canvas", 800, 600);
      stuple.tree()->Draw(TString(sweight)+">>hist", TString(cut_string));

      double sum_of_sweights = 0.;
      double bin_content = 0.;
      double bin_center = 0.;

      for (int i = 1; i < nbins; ++i)
      {
        bin_content = hist->GetBinContent(i);
        bin_center = hist->GetBinCenter(i);
        sum_of_sweights += bin_center*bin_content;
      }
      number_of_events = sum_of_sweights;
      components_and_yield_values[(*it).first]=number_of_events;

      if (debug_mode) doocore::io::serr << "-debug- \t" << "Sum of " << (*it).first << " events: " << number_of_events << " @cut: " << cut_string << doocore::io::endmsg;

      delete hist;
      delete canvas;
    }
    return components_and_yield_values;
  }
  else if(stuple.use_fit()){
    if (debug_mode) doocore::io::serr << "-debug- \t" << "using a fit…" << doocore::io::endmsg;
    // if (debug_mode) doocore::io::serr << "-debug- \t" << "with PDF:" << doocore::io::endmsg;
    // if (debug_mode) stuple.epdf().Pdf("pdf").printCompactTree();

    /// cut on data
    RooAbsData* data;
    if (cut_string==""){
      data = &stuple.dataset();
    }
    else{
      data = stuple.dataset().reduce(TString(cut_string));
    }
    
    // fit
    std::string handover_filename = "FitParameter.txt";
    std::string nocut_startingvalues_file = "StartingValues_NoCut.txt";

    if (cut_string==""){
      stuple.epdf().Pdf("pdf").getParameters(data)->readFromFile(TString(nocut_startingvalues_file));
    }
    else{
      stuple.epdf().Pdf("pdf").getParameters(data)->readFromFile(TString(handover_filename));
    }
    stuple.epdf().Pdf("pdf").fitTo(*data, RooFit::Minimizer("Minuit2", "minimize"), RooFit::NumCPU(16), RooFit::Strategy(2), RooFit::Extended());
    stuple.epdf().Pdf("pdf").getParameters(data)->writeToFile(TString(handover_filename));
    stuple.epdf().Pdf("pdf").getParameters(data)->writeToFile(TString("FitResults_")+cut_string+".out");
    if (cut_string!="") doocore::io::tools::ReplaceScientificNotationInFile(handover_filename, debug_mode);

    // plot
    RooArgList plot_pdfs;
    plot_pdfs.add(stuple.epdf().Pdf("pdf"));

    for(std::map<std::string, std::string>::const_iterator it = stuple.map_of_components_and_pdfs().begin(); it != stuple.map_of_components_and_pdfs().end(); ++it){
      if (debug_mode) doocore::io::serr << "-debug- \t" << "Component: " << (*it).first << ", Yield: " << (*it).second << doocore::io::endmsg;
      plot_pdfs.add(stuple.epdf().Pdf((*it).second));
    }

    doofit::plotting::PlotConfig plot_cfg("plot_cfg");
    doofit::plotting::Plot plot(plot_cfg, stuple.epdf().Var(stuple.observable_name()), *data, plot_pdfs, stuple.observable_name()+"_"+cut_string);
    plot.PlotIt();

    for(std::map<std::string, std::string>::const_iterator it = stuple.map_of_components_and_yields().begin(); it != stuple.map_of_components_and_yields().end(); ++it){
      if (debug_mode) doocore::io::serr << "-debug- \t" << "Component: " << (*it).first << ", PDF: " << (*it).second << doocore::io::endmsg;
      components_and_yield_values[(*it).first]=stuple.epdf().Var((*it).second).getValV();
    }
    
    for(std::map<std::string, double>::const_iterator it = components_and_yield_values.begin(); it != components_and_yield_values.end(); ++it){
      if (debug_mode) doocore::io::serr << "-debug- \t" << "Component: " << (*it).first << ", Yield value: " << (*it).second << doocore::io::endmsg;
    }

    return components_and_yield_values;
  }
}

// =======================
// CLASSIFIER DISTRIBUTION
// =======================
void PlotClassiferDistribution(SelectionTuple& stuple, SelectionClassifier& classifier, int nbins, bool debug_mode){
  if (debug_mode) doocore::io::serr << "-debug- " << "starting doocore::performance::PlotClassiferDistribution()…" << doocore::io::endmsg;
  doocore::lutils::setStyle("LHCb");

  if (stuple.use_mc()){
      doocore::io::serr << "-ERROR- \t PlotClassiferDistribution(SelectionTuple &stuple, SelectionClassifier classifier, int nbins, bool debug_mode)- MC not yet implemented" << doocore::io::endmsg;
  } 
  else if (stuple.use_sweights()){
    if (debug_mode) doocore::io::serr << "-debug- \t" << "using sweigths…" << doocore::io::endmsg;
    /// number of bins
    nbins = 100;

    if (debug_mode) doocore::io::serr << "-debug- " << "1" << doocore::io::endmsg;
    /// TTree
    TTree * tree = stuple.tree();
    int nentries = tree->GetEntries();

    if (debug_mode) doocore::io::serr << "-debug- " << "2" << doocore::io::endmsg;

    /// calculate range of classifier distribution
    std::pair<double,double> minmax = doocore::lutils::MedianLimitsForTuple(*tree, classifier.name());
    // std::pair<double,double> minmax;
    // minmax.first = 3.23303;
    // minmax.second = 6483.23;

    if (debug_mode) doocore::io::serr << "-debug- \t" << classifier.title() << ": min " << minmax.first << " max " << minmax.second << doocore::io::endmsg;

    if (debug_mode) doocore::io::serr << "-debug- " << "3" << doocore::io::endmsg;
    /// prepare TTree variable handling
    float classifier_value;    
    tree->SetBranchAddress(TString(classifier.name()), &classifier_value);

    /// vector of histogram pointers
    std::vector<TH1*> vector_of_histograms;
    std::vector<double*> vector_of_doubles;

    /// loop over components and fill vector with histograms
    for(std::map< std::string, std::string >::const_iterator it = stuple.map_of_components_and_sweights().begin(); it != stuple.map_of_components_and_sweights().end(); it++){
      std::string name = (*it).first;
      std::string sweight = (*it).second;

      if (debug_mode) doocore::io::serr << "-debug- \t" << "sweight: " << sweight << doocore::io::endmsg;

      double* nsweight;
      if (debug_mode) doocore::io::serr << "-debug- " << "4" << doocore::io::endmsg;
      tree->SetBranchAddress(TString(sweight), nsweight);

      TH1D* hist = new TH1D(TString("hist_")+name, TString("hist_")+name, nbins, minmax.first , minmax.second);
      vector_of_histograms.push_back(hist);
      vector_of_doubles.push_back(nsweight);
    }

    /// loop over tree and fill histogramms with entries
    for (int i = 0; i < nentries; ++i)
    {
      if (debug_mode) doocore::io::serr << "-debug- \t" << "event #" << i << doocore::io::endmsg;
      tree->GetEvent(i);

      /// loop over histograms and components

      for (unsigned int i = 0; i < vector_of_histograms.size(); ++i){
        if (debug_mode) doocore::io::serr << "-debug- " << "5" << doocore::io::endmsg;
        vector_of_histograms.at(i)->Fill(classifier_value, *vector_of_doubles.at(i));

        int color = 201+i*4;
        vector_of_histograms.at(i)->SetLineColor(color);
        vector_of_histograms.at(i)->SetMarkerColor(color);
        vector_of_histograms.at(i)->SetFillColor(color+3);
        vector_of_histograms.at(i)->SetFillStyle(3005);
        vector_of_histograms.at(i)->SetMinimum(0);
        vector_of_histograms.at(i)->SetXTitle(TString(classifier.name()));
        vector_of_histograms.at(i)->Draw();
      }
    }

    TCanvas* canvas = new TCanvas("canvas", "canvas", 800, 600);
    // doocore::lutils::drawNormalizedOrdered(vector_of_histograms);
    doocore::lutils::printPlot(canvas, classifier.title()+"_distribution", stuple.title()+"/ClassifierDistribution/");
  }
  else if (stuple.use_fit()){
    doocore::io::serr << "-ERROR- \t PlotClassiferDistribution(SelectionTuple &stuple, SelectionClassifier classifier, int nbins, bool debug_mode)- Fit not yet implemented" << doocore::io::endmsg;
  }
  else{
    doocore::io::serr << "-PlotClassiferDistribution(SelectionTuple &stuple, SelectionClassifier classifier, int nbins, bool debug_mode)- error hello67" << doocore::io::endmsg;
  }
}

void PlotClassiferDistributionOLD(SelectionTuple& stuple, SelectionClassifier& classifier, int nbins, bool debug_mode){
  if (debug_mode) doocore::io::serr << "-debug- " << "starting selection::PlotClassiferDistributionOLD() ..." << doocore::io::endmsg;
  doocore::lutils::setStyle("LHCb");
  if (stuple.use_sweights()){
    
    TTree * tree = stuple.tree();

    double sig_sweight, bkg_sweight;
    float classifier_value;
    int nentries = tree->GetEntries();
    std::pair<double,double> minmax = doocore::lutils::MedianLimitsForTuple(*tree, classifier.name());
    if (debug_mode) doocore::io::serr << "-debug- " << "min " << minmax.first << " max " << minmax.second << doocore::io::endmsg;

    tree->SetBranchAddress(TString("sweight_sig"), &sig_sweight);
    tree->SetBranchAddress(TString("sweight_bkg"), &bkg_sweight);
    tree->SetBranchAddress(TString(classifier.name()), &classifier_value);
    
    TH1D* sig_hist = new TH1D("sig_hist", "sig_hist", nbins, minmax.first , minmax.second);
    TH1D* bkg_hist = new TH1D("bkg_hist", "bkg_hist", nbins, minmax.first , minmax.second);

    for (int i = 0; i < nentries; ++i)
    {
      tree->GetEvent(i);

      sig_hist->Fill(classifier_value, sig_sweight);
      bkg_hist->Fill(classifier_value, bkg_sweight);
    }    

    sig_hist->SetLineColor(kBlue);
    sig_hist->SetMarkerColor(kBlue);
    sig_hist->SetFillColor(kBlue-9);
    sig_hist->SetFillStyle(3005);
    sig_hist->SetMinimum(0);
    sig_hist->SetXTitle(TString(classifier.name()));

    bkg_hist->SetLineColor(kRed);
    bkg_hist->SetMarkerColor(kRed);
    bkg_hist->SetFillColor(kRed-9);
    bkg_hist->SetFillStyle(3004);
    bkg_hist->SetMinimum(0);
    bkg_hist->SetXTitle(TString(classifier.name()));

    TCanvas* canvas = new TCanvas("canvas", "canvas", 800, 600);
  
    doocore::lutils::drawNormalizedOrdered(sig_hist, bkg_hist);
    doocore::lutils::printPlot (canvas, classifier.title()+"_distribution", stuple.title()+"/ClassifierDistribution/");

    delete sig_hist;
    delete bkg_hist;
    delete canvas;
  }
  else if (stuple.use_mc()){
    doocore::io::serr << "-SelectionTuple- not yet implemented" << doocore::io::endmsg;
  } 
  else if (stuple.use_fit()){
    doocore::io::serr << "-SelectionTuple- not yet implemented" << doocore::io::endmsg;
  }
  else{
    doocore::io::serr << "-SelectionTuple- error hello67" << doocore::io::endmsg;
  }
}

// ===============
//  CUT EFFICIENCY
// ===============
void PlotCutEfficiency(SelectionTuple& stuple, SelectionClassifier& classifier, std::string figure_of_merit, int nbins, bool debug_mode){
  doocore::lutils::setStyle("LHCb");
  if (debug_mode) doocore::io::serr << "-debug- " << "starting selection::PlotCutEfficiency() ..." << doocore::io::endmsg;
  if (debug_mode) doocore::io::serr << "-debug- \t" << "classifier "<< classifier.title() << doocore::io::endmsg;
  if (debug_mode) doocore::io::serr << "-debug- \t" << "cut string "<< classifier.best_cut_string() << doocore::io::endmsg;
  
  double massrange_min = 5220.;
  double massrange_max = 5500.;
  doocore::io::swarn << "-dooselection::performance::PlotCutEfficiency- \t" << "Check correct mass range! (min: " << massrange_min << ", max: " << massrange_max << ")" << doocore::io::endmsg;
  TH1D* hist_without_cuts = new TH1D("hist_without_cuts", "hist_without_cuts", 100, massrange_min, massrange_max);
  TH1D* hist_with_cuts = new TH1D("hist_with_cuts", "hist_with_cuts", 100, massrange_min, massrange_max);

  std::map<std::string, double> events_per_component = NumberOfEventsPerComponent(stuple, classifier.best_cut_string(), debug_mode);

  double signal_events = events_per_component["signal"];
  double background_events = events_per_component["background"];
  double signal_efficiency = FoM(stuple, "signal", "background", classifier.best_cut_string(), "Signal Efficiency", debug_mode);
  double background_rejection = FoM(stuple, "signal", "background", classifier.best_cut_string(), "Background Rejection", debug_mode);
  double fom_value = FoM(stuple, "signal", "background", classifier.best_cut_string(), figure_of_merit, debug_mode);

  TCanvas* canvas= new TCanvas("canvas", "canvas", 800, 600);

  std::string mass_variable = "B0_LOKI_MASS_JpsiKSConstr";
  doocore::io::swarn << "-dooselection::performance::PlotCutEfficiency- \t" << "Check correct mass variable! (" << mass_variable << ")" << doocore::io::endmsg;

  stuple.tree()->Draw(TString(mass_variable)+">>hist_without_cuts");
  hist_without_cuts->SetLineColor(kBlack);
  hist_without_cuts->SetMinimum(0);
  hist_without_cuts->SetXTitle(TString(mass_variable));

  stuple.tree()->Draw(TString(mass_variable)+">>hist_with_cuts", TString(classifier.best_cut_string()), "same");
  hist_with_cuts->SetLineColor(kBlue);
  hist_with_cuts->SetLineStyle(3);
  hist_with_cuts->SetFillColor(kBlue);
  hist_with_cuts->SetFillStyle(3003);
  
  TString label = Form(TString("#splitline{#splitline{#splitline{#bf{")+ classifier.title() +"}}{Signal events: %.0f}}{Background events: %.0f}}{#splitline{#splitline{Signal efficiency: %.2f %}{Background Rejection: %.2f %}}{"+figure_of_merit+": %.3f}}",signal_events, background_events, signal_efficiency*100, background_rejection*100, fom_value);
  TLatex latex_label(0.5, 0.84, label);
  latex_label.SetNDC();
  latex_label.SetTextSize(0.02);
  latex_label.SetTextColor(1);
  latex_label.SetTextAlign(11);
  latex_label.Draw();

  doocore::lutils::printPlot(canvas, classifier.title(), stuple.title()+"/CutEfficiency/");
  delete hist_without_cuts;
  delete hist_with_cuts;
  delete canvas;
}

// =====================
// FIGURE OF MERIT (FoM)
// =====================
/// calculate FoM for a single cut value
double FoM(SelectionTuple &stuple, std::string signal_component, std::string background_component, std::string cut_string, std::string figure_of_merit, bool debug_mode){
  if (debug_mode) doocore::io::serr << "-debug- " << "starting doocore::performance::FoM()…" << doocore::io::endmsg;
  std::map<std::string, std::string> map_of_components_and_yields = stuple.map_of_components_and_yields();
  if ((map_of_components_and_yields[signal_component]==0) || (map_of_components_and_yields[background_component]==0)) doocore::io::serr << "-ERROR- \t" << "Signal or background component do not exist in SelectionTuple '" << stuple.title() << "'!" << doocore::io::endmsg;
  if (debug_mode) doocore::io::serr << "-debug- \t" << "cut '" << cut_string << "'…" << doocore::io::endmsg;
  if (debug_mode) doocore::io::serr << "-debug- \t" << "figure of merit '" << figure_of_merit << "'…" << doocore::io::endmsg;
  if (debug_mode) doocore::io::serr << "-debug- \t" << "signal component '" << signal_component << "'…" << doocore::io::endmsg;
  if (debug_mode) doocore::io::serr << "-debug- \t" << "background component '" << background_component << "'…" << doocore::io::endmsg;

  double fom_value;
  std::map<std::string, double> number_of_events_per_component = NumberOfEventsPerComponent(stuple, cut_string, debug_mode);
  std::map<std::string, double> max_number_of_events_per_component;

  double number_of_signal_events = number_of_events_per_component[signal_component];
  double number_of_background_events = number_of_events_per_component[background_component];
  double maximal_number_of_signal_events;
  double maximal_number_of_background_events;

  if ((figure_of_merit == "Signal Efficiency") || (figure_of_merit == "Background Rejection")){
    max_number_of_events_per_component = NumberOfEventsPerComponent(stuple, "", debug_mode);
    maximal_number_of_signal_events = max_number_of_events_per_component[signal_component];
    maximal_number_of_background_events = max_number_of_events_per_component[background_component];
  }

  if (figure_of_merit == "Significance"){
    fom_value = number_of_signal_events/sqrt(number_of_signal_events+number_of_background_events);
  }
  else if (figure_of_merit == "Weighted Significance"){
    const double alpha = 100;
    number_of_signal_events = number_of_signal_events/alpha;
    fom_value = number_of_signal_events/sqrt(number_of_signal_events+number_of_background_events);
  }
  else if (figure_of_merit == "Purity"){
    fom_value = number_of_signal_events/(number_of_signal_events+number_of_background_events);
  }
  else if (figure_of_merit == "Punzi"){
    const double alpha = 2;
    fom_value = number_of_signal_events/((alpha/2)+sqrt(number_of_background_events));
  }
  else if (figure_of_merit == "Signal Efficiency"){
    fom_value = number_of_signal_events/maximal_number_of_signal_events;
  }
  else if (figure_of_merit == "Background Rejection"){
    fom_value = (1-(number_of_background_events/maximal_number_of_background_events));
  }
  else{
    doocore::io::serr << "Please define a valid figure of merit!" << doocore::io::endmsg;
    doocore::io::serr << "Possible FoMs are: Significance, Weighted Significance, Purity, Punzi, Signal Efficiency, and Background Rejection" << doocore::io::endmsg;
    abort;
  }
  if (debug_mode) doocore::io::serr << "-debug- \t" << figure_of_merit << ": " << fom_value << doocore::io::endmsg;
  return fom_value;
}

/// calculate FoM distribution for different cut values in a given range
std::vector< std::pair<double, double> > FoMDistribution(SelectionTuple &stuple, SelectionClassifier &classifier, std::string signal_component, std::string background_component, std::string figure_of_merit, bool debug_mode){
  if (debug_mode) doocore::io::serr << "-debug- " << "starting doocore::performance::FoMDistribution() ..." << doocore::io::endmsg;
  std::vector<double> steps = classifier.steps();
  double step_size = steps.at(1)-steps.at(0);

  std::pair<double, double> xy_values;
  std::vector< std::pair<double, double> > vector_of_xy_values;

  std::string cut_string;

  double fom_value = 0;

  for(std::vector<double>::iterator it = steps.begin(); it != steps.end(); it++){
    cut_string=classifier.name()+classifier.cut_operator()+boost::lexical_cast<std::string>(*it);
    fom_value = FoM(stuple, signal_component, background_component, cut_string, figure_of_merit, debug_mode);

    if (std::isfinite(fom_value)){
        xy_values.first = (*it) - step_size/2;
        xy_values.second = fom_value;
        vector_of_xy_values.push_back(xy_values);
    }
  }

  return vector_of_xy_values;
}

/// plot FoM distribution in a given range
void PlotFoMDistribution(std::vector< std::pair<double, double> > fom_distribution, SelectionClassifier& classifier, std::string figure_of_merit, std::string output_prefix, bool debug_mode){
  if (debug_mode) doocore::io::serr << "-debug- " << "starting selection::PlotFoMDistribution() ..." << doocore::io::endmsg;
  doocore::lutils::setStyle();

  int entries = fom_distribution.size();

  double x_values[entries];
  double y_values[entries];

  // fill arrays
  for(unsigned int i = 0; i < entries; ++i) {
    x_values[i] = fom_distribution.at(i).first;
    y_values[i] = fom_distribution.at(i).second;
  }

  TCanvas canvas("canvas", "canvas", 800, 600);
  TGraph xy_graph(entries, x_values, y_values);

  xy_graph.SetTitle("");
  xy_graph.GetXaxis()->SetTitle(TString(classifier.name()));
  xy_graph.GetYaxis()->SetTitle(TString(figure_of_merit));

  xy_graph.Draw("ac*");

  doocore::lutils::printPlot (&canvas, figure_of_merit+"_"+classifier.title(), output_prefix+"/FigureOfMerit/");
}

/// plot FoM distribution in a given range
void PlotFoMDistribution(SelectionTuple& stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, std::string figure_of_merit, bool debug_mode){
  PlotFoMDistribution(FoMDistribution(stuple, classifier, signal_component, background_component, figure_of_merit, debug_mode), classifier, figure_of_merit, stuple.title(), debug_mode);
}

// =======================================
// RECEIVER OPERATING CHARACTERISTIC (ROC)
// =======================================
/// calculate ROC for a given classifier
std::vector< std::pair<double, double> > ROC(SelectionTuple &stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, bool debug_mode){
  if (debug_mode) doocore::io::serr << "-debug- " << "starting doocore::performance::ROC ..." << doocore::io::endmsg;
  std::vector<double> steps = classifier.steps();
  double step_size = steps.at(1)-steps.at(0);

  std::pair<double, double> xy_values;
  std::vector< std::pair<double, double> > vector_of_xy_values;

  std::string cut_string;

  double signal_efficiency = 0.;
  double background_rejection = 0.;

  for(std::vector<double>::iterator it = steps.begin(); it != steps.end(); it++){
    cut_string=classifier.name()+classifier.cut_operator()+boost::lexical_cast<std::string>(*it);
    signal_efficiency = FoM(stuple, signal_component, background_component, cut_string, "Signal Efficiency", debug_mode);
    background_rejection = FoM(stuple, signal_component, background_component, cut_string, "Background Rejection", debug_mode);

    if (std::isfinite(signal_efficiency) || (std::isfinite(background_rejection))){
        xy_values.first = signal_efficiency;
        xy_values.second = background_rejection;
        vector_of_xy_values.push_back(xy_values);
    }
  }
  return vector_of_xy_values;
}

// plot ROC Curve
void PlotROCCurve(std::vector< std::pair<double, double> > roc, std::string classifier_name, std::string output_prefix, bool debug_mode){
  if (debug_mode) doocore::io::serr << "-debug- " << "starting doocore::performance::PlotROCCurve() ..." << doocore::io::endmsg;
  doocore::lutils::setStyle();

  int entries = roc.size();

  double x_values[entries];
  double y_values[entries];

  // fill arrays
  for(unsigned int i = 0; i < entries; ++i) {
    x_values[i] = roc.at(i).first;
    y_values[i] = roc.at(i).second;
  }

  TCanvas canvas("canvas", "canvas", 800, 600);
  TGraph xy_graph(entries, x_values, y_values);

  xy_graph.SetTitle("ROC Curve");
  xy_graph.GetXaxis()->SetTitle("Signal Efficiency");
  xy_graph.GetYaxis()->SetTitle("Background Rejection");

  xy_graph.Draw("ac*");

  doocore::lutils::printPlot (&canvas, "ROC_"+classifier_name, output_prefix+"/FigureOfMerit/");
}

void PlotROCCurve(SelectionTuple &stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, bool debug_mode){
  PlotROCCurve(ROC(stuple, classifier, signal_component, background_component, debug_mode), classifier.title(), stuple.title(), debug_mode);
}

} // namespace performance
} // namespace dooselection