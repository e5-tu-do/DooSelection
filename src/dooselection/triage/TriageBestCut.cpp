#include "Triage.h"

// from STL

// from ROOT
#include "TCanvas.h"
#include "TTree.h"
#include "TStyle.h"

// from RooFit

// from TMVA

// from BOOST

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/lutils/lutils.h"

// from here
#include "Tuple.h"
#include "MCTuple.h"
#include "SWTuple.h"
#include "FTTuple.h"
#include "FoM.h"

// forward declarations

namespace dooselection{
namespace triage{

void Triage::BestCutPerformances(PlotStyle plot_style){
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "calling void Triage::BestCutPerformances() \t" << doocore::io::endmsg;
  for(TupleList::iterator tuple = tuple_list_.begin(); tuple != tuple_list_.end(); tuple++){
    MaximalNumberOfEvents(*tuple);
    for(ClassifierList::iterator classifier = classifier_list_.begin(); classifier != classifier_list_.end(); classifier++){
      BestCutPerformance(*tuple, &(*classifier), plot_style);
    }
  }
}

void Triage::BestCutPerformance(Tuple* tuple, Classifier* classifier, PlotStyle plot_style){
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "calling void Triage::BestCutPerformance(Tuple* tuple, Classifier* classifier) \t" << doocore::io::endmsg;
  doocore::lutils::setStyle("LHCb");

  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "classifier "<< classifier->name() << doocore::io::endmsg;

  std::string cut_string("");

  if (((classifier->cut_operator())=="") && ((classifier->best_cut_value())<=-99998.)){
    cut_string = classifier->expression();
  }
  else{
    cut_string = classifier->expression()+classifier->cut_operator()+std::to_string(classifier->best_cut_value());
  } 
  std::string interim_cutstring = cut_string;

  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "cut string "<< cut_string << doocore::io::endmsg;
  
  double margin = (tuple->observable_range().second-tuple->observable_range().first)*0.1;

  std::string observable_range = "((" + tuple->observable_name() + ">" + std::to_string(tuple->observable_range().first) + ")&&(" + tuple->observable_name() + "<" + std::to_string(tuple->observable_range().second) + "))";
  interim_cutstring = observable_range;
  if (cut_string != "") interim_cutstring += "&&(" + cut_string + ")";

  doocore::io::sdebug << "-debug- \t" << "Check correct mass variable! (" << tuple->observable_name() << ")" << doocore::io::endmsg;
  doocore::io::sdebug << "-debug- \t" << "Check correct mass range! (min: " << tuple->observable_range().first << ", max: " << tuple->observable_range().second << ")" << doocore::io::endmsg;

  TH1D* hist_without_cuts = new TH1D("hist_without_cuts", "hist_without_cuts", 100, tuple->observable_range().first-margin, tuple->observable_range().second+margin);
  TH1D* hist_with_cuts = new TH1D("hist_with_cuts", "hist_with_cuts", 100, tuple->observable_range().first-margin, tuple->observable_range().second+margin);

  std::pair<double, double> number_sig_bkg_events = NumberOfSigAndBkgEvents(tuple, cut_string);

  double n_signal_events = number_sig_bkg_events.first;
  double n_background_events = number_sig_bkg_events.second;

  double signal_efficiency = FoM::SignalEfficiency(n_signal_events, n_background_events, tuple->max_n_sig_events(), tuple->max_n_bkg_events());
  double background_rejection = FoM::BackgroundRejection(n_signal_events, n_background_events, tuple->max_n_sig_events(), tuple->max_n_bkg_events());
  double significance = FoM::Significance(n_signal_events, n_background_events, tuple->max_n_sig_events(), tuple->max_n_bkg_events());
  double sin2beta = FoM::Sin2Beta(n_signal_events, n_background_events, tuple->max_n_sig_events(), tuple->max_n_bkg_events());

  // plot
  TCanvas* canvas= new TCanvas("canvas", "canvas", 800, 600);

  tuple->tree().Draw(TString(tuple->observable_name())+">>hist_without_cuts", TString(observable_range));
  
  hist_without_cuts->SetLineColor(kBlack);
  hist_without_cuts->SetMinimum(0);
  hist_without_cuts->SetXTitle(TString(tuple->observable_label())+" ("+TString(tuple->observable_unit())+")");
  hist_without_cuts->SetYTitle(TString::Format("Candidates / ( %4.2f %s )", hist_without_cuts->GetBinWidth(1), tuple->observable_unit().c_str()));
  hist_without_cuts->GetXaxis()->SetTitleSize(0.055);
  hist_without_cuts->GetXaxis()->SetTitleOffset(1.25);
  hist_without_cuts->GetYaxis()->SetTitleSize(0.05);
  hist_without_cuts->GetYaxis()->SetTitleOffset(1.5);

  tuple->tree().Draw(TString(tuple->observable_name())+">>hist_with_cuts", TString(interim_cutstring), "same");
  
  hist_with_cuts->SetLineColor(kBlue);
  hist_with_cuts->SetLineStyle(3);
  hist_with_cuts->SetFillColor(kBlue);
  hist_with_cuts->SetFillStyle(3003);

  TString label;
  if (plot_style==PlotStyle::All){
    label = Form(TString("#splitline{#splitline{#splitline{#splitline{#splitline{#splitline{#bf{")+ classifier->name() +"}}{Cut: "+cut_string+"}}{Signal events: %.0f}}{Background events: %.0f}}{Signal efficiency: %.2f %}}{Background Rejection: %.2f %}}{#splitline{Significance: %.3f}{sin2#beta FoM: %.3f}}", n_signal_events, n_background_events, signal_efficiency*100, background_rejection*100, significance, sin2beta);
  }
  else if (plot_style==PlotStyle::NameOnly){
    label = Form(TString("#bf{")+classifier->name()+"}");
  }
  TLatex latex_label(0.2, 0.74, label);
  latex_label.SetNDC();
  latex_label.SetTextSize(0.02);
  latex_label.SetTextColor(1);
  latex_label.SetTextAlign(11);
  if (!(plot_style==PlotStyle::Nothing)) latex_label.Draw();

  doocore::lutils::printPlot(canvas, "BestCut_"+tuple->name()+"_"+classifier->name(), "BestCut/");

  // plot logarithmic
  TCanvas* canvas_log = new TCanvas("canvas_log", "canvas_log", 800, 600);
  if (true){
    canvas_log->SetLogy();

    tuple->tree().Draw(TString(tuple->observable_name())+">>hist_without_cuts", TString(observable_range));
    
    hist_without_cuts->SetMinimum(10);

    tuple->tree().Draw(TString(tuple->observable_name())+">>hist_with_cuts", TString(interim_cutstring), "same");

    if (!(plot_style==PlotStyle::Nothing)) latex_label.Draw();

    doocore::lutils::printPlot(canvas_log, "BestCut_"+tuple->name()+"_"+classifier->name()+"_log", "BestCut/");
  }

  delete hist_without_cuts;
  delete hist_with_cuts;
  delete canvas;
  delete canvas_log;
}

} // namespace triage
} // namespace dooselection
