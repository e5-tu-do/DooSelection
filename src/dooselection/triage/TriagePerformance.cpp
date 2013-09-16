#include "Triage.h"

// from STL

// from ROOT
#include "TStyle.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TText.h"

// from RooFit

// from TMVA

// from BOOST
#ifdef __GNUG__
#define BOOST_NO_CXX11_SCOPED_ENUMS
#endif
#include <boost/filesystem.hpp>

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/config/Summary.h"
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

namespace fs = boost::filesystem;
  
void Triage::InitializeScans(const int nbins){
  set_nbins(nbins);
  FillTriageHistContainerList(nbins);
}

void Triage::PerformanceScans(){
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "calling void Triage::Performance() \t" << doocore::io::endmsg;
  for(std::vector<TriageHistContainer>::iterator hists_sig_bkg_event = hist_container_list_.begin(); hists_sig_bkg_event != hist_container_list_.end(); hists_sig_bkg_event++){
    PlotROC((*hists_sig_bkg_event).name(), (*hists_sig_bkg_event).hist_number_sig_bkg_events(), (*hists_sig_bkg_event).max_n_sig_events(), (*hists_sig_bkg_event).max_n_bkg_events());
    PlotSignalEfficiency((*hists_sig_bkg_event).name(), (*hists_sig_bkg_event).hist_number_sig_bkg_events(), (*hists_sig_bkg_event).cut_values(), (*hists_sig_bkg_event).max_n_sig_events(), (*hists_sig_bkg_event).max_n_bkg_events());
    PlotBackgroundRejection((*hists_sig_bkg_event).name(), (*hists_sig_bkg_event).hist_number_sig_bkg_events(), (*hists_sig_bkg_event).cut_values(), (*hists_sig_bkg_event).max_n_sig_events(), (*hists_sig_bkg_event).max_n_bkg_events());
    PlotEfficiencies((*hists_sig_bkg_event).name(), (*hists_sig_bkg_event).hist_number_sig_bkg_events(), (*hists_sig_bkg_event).cut_values(), (*hists_sig_bkg_event).max_n_sig_events(), (*hists_sig_bkg_event).max_n_bkg_events());
  }
}

void Triage::PlotROC(std::string name, std::pair<TH1D, TH1D> hist_number_sig_bkg_events, double max_n_sig_events, double max_n_bkg_events){
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "calling void Triage::PlotROC(std::string name, std::pair<TH1D, TH1D> hist_number_sig_bkg_events, double max_n_sig_events, double max_n_bkg_events) \t" << doocore::io::endmsg;
  double signal_efficiency[nbins_];
  double background_rejection[nbins_];
  for (unsigned int i = 1; i < nbins_; ++i){
    double n_signal_events = hist_number_sig_bkg_events.first.GetBinContent(i);
    double n_background_events = hist_number_sig_bkg_events.second.GetBinContent(i);   
    signal_efficiency[i] = FoM::SignalEfficiency(n_signal_events, n_background_events, max_n_sig_events, max_n_bkg_events);
    background_rejection[i] = FoM::BackgroundRejection(n_signal_events, n_background_events, max_n_sig_events, max_n_bkg_events);
  }
  TCanvas canvas("canvas", "canvas", 800, 600);
  TGraph roc_graph(nbins_, signal_efficiency, background_rejection);

  roc_graph.SetMarkerColor(kBlue+2);
  roc_graph.SetMarkerStyle(kFullDotMedium);

  roc_graph.SetTitle(TString("ROC Curve for ")+name);
  roc_graph.GetXaxis()->SetTitle("Signal Efficiency");
  roc_graph.GetXaxis()->SetLimits(0.,1.);
  roc_graph.GetYaxis()->SetTitle("Background Rejection");
  roc_graph.GetYaxis()->SetLimits(0.,1.);

  roc_graph.Draw("ap");

  fs::path filename = fs::path("PerformanceScans/pdf") / fs::path(std::string("ROC_")+name+".pdf");
  doocore::config::Summary::GetInstance().AddFile(filename);
  doocore::lutils::printPlot (&canvas, "ROC_"+name, "PerformanceScans/");
}

void Triage::PlotSignalEfficiency(std::string name, std::pair<TH1D, TH1D> hist_number_sig_bkg_events, std::vector<double> cut_values, double max_n_sig_events, double max_n_bkg_events){
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "calling void Triage::PlotSignalEfficiency(std::string name, std::pair<TH1D, TH1D> hist_number_sig_bkg_events, std::vector<double> cut_values, double max_n_sig_events, double max_n_bkg_events) \t" << doocore::io::endmsg;
  double signal_efficiency[nbins_];
  double cut_value[nbins_];
  double n_background_events(0.); // dummy
  for (unsigned int i = 1; i < nbins_; ++i){
    double n_signal_events = hist_number_sig_bkg_events.first.GetBinContent(i);
    signal_efficiency[i] = FoM::SignalEfficiency(n_signal_events, n_background_events, max_n_sig_events, max_n_bkg_events);
    cut_value[i] = cut_values.at(i-1);
  }
  TCanvas canvas("canvas", "canvas", 800, 600);
  TGraph sigeff_graph(nbins_, cut_value, signal_efficiency);

  sigeff_graph.SetMarkerColor(kBlue+2);
  sigeff_graph.SetMarkerStyle(kFullDotMedium);

  sigeff_graph.SetTitle(TString("Signal Efficiency for ")+name);
  sigeff_graph.GetXaxis()->SetTitle("Classifier");
  sigeff_graph.GetYaxis()->SetTitle("Signal Efficiency");
  sigeff_graph.GetYaxis()->SetLimits(0.,1.);

  sigeff_graph.Draw("ap");

  fs::path filename = fs::path("PerformanceScans/pdf") / fs::path(std::string("SignalEfficiency_")+name+".pdf");
  doocore::config::Summary::GetInstance().AddFile(filename);
  doocore::lutils::printPlot (&canvas, "SignalEfficiency_"+name, "PerformanceScans/");
}

void Triage::PlotBackgroundRejection(std::string name, std::pair<TH1D, TH1D> hist_number_sig_bkg_events, std::vector<double> cut_values, double max_n_sig_events, double max_n_bkg_events){
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "calling void Triage::PlotBackgroundRejection(std::string name, std::pair<TH1D, TH1D> hist_number_sig_bkg_events, std::vector<double> cut_values, double max_n_sig_events, double max_n_bkg_events) \t" << doocore::io::endmsg;
  double background_rejection[nbins_];
  double cut_value[nbins_];
  double n_signal_events(0.); // dummy
  for (unsigned int i = 1; i < nbins_; ++i){
    double n_background_events = hist_number_sig_bkg_events.second.GetBinContent(i);
    background_rejection[i] = FoM::BackgroundRejection(n_signal_events, n_background_events, max_n_sig_events, max_n_bkg_events);
    
    using namespace doocore::io;
    swarn << "Background Rejection[" << i << "]: n_bkg = " << n_background_events << "/" << max_n_bkg_events << ", bkg_rej = " << background_rejection[i] << endmsg;
    
    cut_value[i] = cut_values.at(i-1);
  }
  TCanvas canvas("canvas", "canvas", 800, 600);
  TGraph bkgrej_graph(nbins_, cut_value, background_rejection);

  bkgrej_graph.SetMarkerColor(kRed+2);
  bkgrej_graph.SetMarkerStyle(kFullDotMedium);

  bkgrej_graph.SetTitle(TString("Background Rejection for ")+name);
  bkgrej_graph.GetXaxis()->SetTitle("Classifier");
  bkgrej_graph.GetYaxis()->SetTitle("Background Rejection");
  bkgrej_graph.GetYaxis()->SetLimits(0.,1.);

  bkgrej_graph.Draw("ap");

  fs::path filename = fs::path("PerformanceScans/pdf") / fs::path(std::string("BackgroundRejection_")+name+".pdf");
  doocore::config::Summary::GetInstance().AddFile(filename);
  doocore::lutils::printPlot (&canvas, "BackgroundRejection_"+name, "PerformanceScans/");
}

void Triage::PlotEfficiencies(std::string name, std::pair<TH1D, TH1D> hist_number_sig_bkg_events, std::vector<double> cut_values, double max_n_sig_events, double max_n_bkg_events){
  double signal_efficiency[nbins_];
  double background_efficiency[nbins_];
  double significance[nbins_];
  double wsignificance[nbins_];
  double purity[nbins_];
  double sin2beta[nbins_];
  double cut_value[nbins_];

  // FoM value and corresponding cut value
  std::pair<double, double> significance_max;
  std::pair<double, double> wsignificance_max;
  std::pair<double, double> purity_max;
  std::pair<double, double> sin2beta_max;

  for (unsigned int i = 1; i < nbins_; ++i){
    double n_signal_events = hist_number_sig_bkg_events.first.GetBinContent(i);
    double n_background_events = hist_number_sig_bkg_events.second.GetBinContent(i);

    signal_efficiency[i] = FoM::SignalEfficiency(n_signal_events, n_background_events, max_n_sig_events, max_n_bkg_events);
    background_efficiency[i] = FoM::BackgroundEfficiency(n_signal_events, n_background_events, max_n_sig_events, max_n_bkg_events);
    significance[i] = FoM::Significance(n_signal_events, n_background_events, max_n_sig_events, max_n_bkg_events);
    wsignificance[i] = FoM::WeightedSignificance(n_signal_events, n_background_events, max_n_sig_events, max_n_bkg_events);
    purity[i] = FoM::Purity(n_signal_events, n_background_events, max_n_sig_events, max_n_bkg_events);
    sin2beta[i] = FoM::Sin2Beta(n_signal_events, n_background_events, max_n_sig_events, max_n_bkg_events);

    cut_value[i] = cut_values.at(i-1);

    if (significance[i]>significance_max.first){
      significance_max.first = significance[i];
      significance_max.second = cut_value[i];
    }
    if (wsignificance[i]>wsignificance_max.first){
      wsignificance_max.first = wsignificance[i];
      wsignificance_max.second = cut_value[i];
    }
    if (purity[i]>purity_max.first){
      purity_max.first = purity[i];
      purity_max.second = cut_value[i];
    }
    if (sin2beta[i]>sin2beta_max.first){
      sin2beta_max.first = sin2beta[i];
      sin2beta_max.second = cut_value[i];
    }
  }

  for (int i = 0; i < nbins_; ++i){
    significance[i] = significance[i]/significance_max.first;
    wsignificance[i] = wsignificance[i]/wsignificance_max.first;
    purity[i] = purity[i]/purity_max.first;
    sin2beta[i] = sin2beta[i]/sin2beta_max.first;
  }

  TCanvas canvas("canvas", "canvas", 800, 700);
  canvas.Divide(1,3);
  canvas.GetPad(1)->SetPad(0,0.9,1,1);
  canvas.GetPad(2)->SetPad(0,0.8,1,0.9);
  canvas.GetPad(3)->SetPad(0,0,1,0.8);
  canvas.GetPad(3)->SetTopMargin(0.);
  TGraph significance_graph(nbins_, cut_value, significance);
  TGraph purity_graph(nbins_, cut_value, purity);
  TGraph sin2beta_graph(nbins_, cut_value, sin2beta);
  TGraph sigeff_graph(nbins_, cut_value, signal_efficiency);
  TGraph bkgeff_graph(nbins_, cut_value, background_efficiency);

  canvas.cd(3);
  significance_graph.Draw("a*");
  purity_graph.Draw("same *");
  sin2beta_graph.Draw("same *");
  sigeff_graph.Draw("same *");
  bkgeff_graph.Draw("same *");

  significance_graph.SetName("significance");
  significance_graph.SetTitle("");
  significance_graph.GetXaxis()->SetTitle("Classifier");
  significance_graph.GetYaxis()->SetTitle("Efficiency");
  significance_graph.SetMarkerColor(kGreen+2);
  significance_graph.SetMarkerStyle(kFullDotMedium);

  purity_graph.SetMarkerColor(kYellow+2);
  purity_graph.SetMarkerStyle(kFullDotMedium);

  sin2beta_graph.SetMarkerColor(kMagenta+2);
  sin2beta_graph.SetMarkerStyle(kFullDotMedium);

  sigeff_graph.SetMarkerColor(kBlue+2);
  sigeff_graph.SetMarkerStyle(kFullDotMedium);

  bkgeff_graph.SetMarkerColor(kRed+2);
  bkgeff_graph.SetMarkerStyle(kFullDotMedium);

  double rightx = significance_graph.GetHistogram()->GetXaxis()->GetXmax();

  TGaxis *unit_axis = new TGaxis(rightx,0,rightx,1.1,0,significance_max.first,40408,"+");
  unit_axis->SetTitle("Significance");
  unit_axis->SetLineColor(kGreen+2);
  unit_axis->SetLabelColor(kGreen+2);
  unit_axis->SetLabelOffset(0.04);
  unit_axis->SetTextColor(kGreen+2);
  unit_axis->Draw();

  canvas.cd(2);
  TLegend* legend = new TLegend(0.1,0.1,0.9,0.9);
     // legend->SetHeader(TString("Cut efficiencies for ")+name);
     legend->SetBorderSize(0);
     legend->SetFillColor(kWhite);
     legend->SetNColumns(2);
     legend->AddEntry(&sigeff_graph, "Signal Efficiency","p");
     legend->AddEntry(&bkgeff_graph, "Background Efficiency","p");
     legend->AddEntry(&significance_graph, TString("Significance (")+boost::lexical_cast<std::string>(significance_max.first)+" @cut value: "+boost::lexical_cast<std::string>(significance_max.second)+")","p");
     legend->AddEntry(&significance_graph, TString("Weighted Significance (")+boost::lexical_cast<std::string>(wsignificance_max.first)+" @cut value: "+boost::lexical_cast<std::string>(wsignificance_max.second)+")","p");
     legend->AddEntry(&purity_graph, TString("Purity (")+boost::lexical_cast<std::string>(purity_max.first)+" @cut value: "+boost::lexical_cast<std::string>(purity_max.second)+")","p");
     legend->AddEntry(&sin2beta_graph, TString("sin2#beta FoM (")+boost::lexical_cast<std::string>(sin2beta_max.first)+" @cut value: "+boost::lexical_cast<std::string>(sin2beta_max.second)+")","p");
     legend->Draw();

  canvas.cd(1);
  TText* title = new TText(0.1,0.1, TString("Cut efficiencies for ")+name);
  title->SetTextSize(0.3);
  title->Draw();
  
  fs::path filename = fs::path("PerformanceScans/pdf") / fs::path(std::string("FoMs_")+name+".pdf");
  doocore::config::Summary::GetInstance().AddFile(filename);
  doocore::lutils::printPlot(&canvas, "FoMs_"+name, "PerformanceScans/");
}


} // namespace triage
} // namespace dooselection
