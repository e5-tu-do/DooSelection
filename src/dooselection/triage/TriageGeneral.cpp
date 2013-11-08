#include "Triage.h"

// from STL

// from ROOT
#include "TString.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"

// from RooFit

// from TMVA

// from BOOST
#include "boost/lexical_cast.hpp"

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/io/Tools.h"

// from DooFit
#include "doofit/plotting/Plot/Plot.h"
#include "doofit/plotting/Plot/PlotSimultaneous.h"
#include "doofit/plotting/Plot/PlotConfig.h"
#include <doofit/fitter/AbsFitter.h>

// from here
#include "Tuple.h"
#include "MCTuple.h"
#include "SWTuple.h"
#include "FTTuple.h"
#include "AbsFitterTuple.h"

// forward declarations

namespace dooselection{
namespace triage{

void Triage::FillTriageHistContainerList(const int nbins){
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "calling void Triage::ComputeSigAndBkgEventNumbersHistogram() \t" << doocore::io::endmsg;
  for(TupleList::iterator tuple = tuple_list_.begin(); tuple != tuple_list_.end(); tuple++){
    MaximalNumberOfEvents(*tuple);
    for(ClassifierList::iterator classifier = classifier_list_.begin(); classifier != classifier_list_.end(); classifier++){
      hist_container_list_.push_back(TriageHistContainer((*tuple)->name()+"_"+(*classifier).name()+"_"+boost::lexical_cast<std::string>(nbins) ,*tuple, &(*classifier), (*tuple)->max_n_sig_events(), (*tuple)->max_n_bkg_events(), (*classifier).cut_values(), SigAndBkgEventNumbersHistogram(*tuple, &(*classifier), nbins)));
    }
  }
  hist_container_list_filled_ = true;
}

/// compute the maximal number of events and save the number in tuple
void Triage::MaximalNumberOfEvents(Tuple* tuple){
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "calling void Triage::MaximalNumberOfEvents(Tuple* tuple) \t" << " with tuple '" << tuple->name() << "'" << doocore::io::endmsg;
  std::pair<double, double> number_of_events = NumberOfSigAndBkgEvents(tuple, "");
  tuple->set_max_n_sig_events(number_of_events.first);
  tuple->set_max_n_bkg_events(number_of_events.second);
}

/// given a specific cut, use this method to compute the number of events
std::pair<double, double> Triage::NumberOfSigAndBkgEvents(Tuple* tuple, const std::string& cut_string){
  if (debug_mode_) if (cut_string!="") doocore::io::sdebug << "-debug- \t" << "calling std::pair<double, double> Triage::NumberOfSigAndBkgEvents(Tuple* tuple, const std::string& cut_string) with cut '" << cut_string << "'…" << doocore::io::endmsg;
  if (debug_mode_) if (cut_string=="") doocore::io::sdebug << "-debug- \t" << "calling std::pair<double, double> Triage::NumberOfSigAndBkgEvents(Tuple* tuple, const std::string& cut_string) without cut" << doocore::io::endmsg;
  std::pair<double, double> number_of_events;

  if (tuple->is_mc()){
    MCTuple* mctuple = dynamic_cast<MCTuple*>(tuple);
    if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "using MC truth information…" << doocore::io::endmsg;

    std::string observable_range = "(" + mctuple->observable_name() + ">" + boost::lexical_cast<std::string>(mctuple->observable_range().first) + ")&&(" + mctuple->observable_name() + "<" + boost::lexical_cast<std::string>(mctuple->observable_range().second) + ")";
    std::string interim_cut_string = observable_range;
    if (cut_string != "") interim_cut_string += "&&(" + cut_string + ")";
    std::string signal_cut_string = interim_cut_string + "&&(" + mctuple->signal_cut() + ")";
    std::string background_cut_string = interim_cut_string + "&&(" + mctuple->background_cut() + ")";

    if (debug_mode_) doocore::io::sdebug << "-debug- \t" << " signal cut string: " << signal_cut_string << doocore::io::endmsg;
    if (debug_mode_) doocore::io::sdebug << "-debug- \t" << " background cut string: " << background_cut_string << doocore::io::endmsg;

    number_of_events.first = mctuple->tree().GetEntries(TString(signal_cut_string));
    number_of_events.second = mctuple->tree().GetEntries(TString(background_cut_string));
  }
  else if (tuple->is_sw()){
    SWTuple* swtuple = dynamic_cast<SWTuple*>(tuple);
    if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "using sweights…" << doocore::io::endmsg;
    int nbins = swtuple->tree().GetEntries();
    TCanvas canvas("canvas", "canvas", 800, 600);
    TH1D sig_hist("sig_hist", "sig_hist", nbins, swtuple->sweights_range().first, swtuple->sweights_range().second);
    TH1D bkg_hist("bkg_hist", "bkg_hist", nbins, swtuple->sweights_range().first, swtuple->sweights_range().second);

    std::string observable_range = "((" + tuple->observable_name() + ">" + boost::lexical_cast<std::string>(tuple->observable_range().first) + ")&&(" + tuple->observable_name() + "<" + boost::lexical_cast<std::string>(tuple->observable_range().second) + "))";
    std::string interim_cut_string = observable_range;
    if (cut_string != "") interim_cut_string += "&&(" + cut_string + ")";

    swtuple->tree().Draw(TString(swtuple->signal_sweight())+">>sig_hist", TString(interim_cut_string));
    swtuple->tree().Draw(TString(swtuple->background_sweight())+">>bkg_hist", TString(interim_cut_string));

    double sum_of_sig_sweights = 0.;
    double sum_of_bkg_sweights = 0.;
    double bin_content_sig = 0.;
    double bin_content_bkg = 0.;
    double bin_center_sig = 0.;
    double bin_center_bkg = 0.;

    for (unsigned int i = 1;i < nbins; ++i){
      bin_content_sig = sig_hist.GetBinContent(i);
      bin_content_bkg = bkg_hist.GetBinContent(i);
      bin_center_sig = sig_hist.GetBinCenter(i);
      bin_center_bkg = bkg_hist.GetBinCenter(i);
      sum_of_sig_sweights += bin_center_sig*bin_content_sig;
      sum_of_bkg_sweights += bin_center_bkg*bin_content_bkg;
    }
    number_of_events.first = sum_of_sig_sweights;
    number_of_events.second = sum_of_bkg_sweights;
  }
  else if (tuple->is_ft()){
    FTTuple* fttuple = dynamic_cast<FTTuple*>(tuple);
    std::string local_cut_string = cut_string;
    if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "using a fit…" << doocore::io::endmsg;

    RooAbsData* data;
    bool owned_data = false;
    if (local_cut_string==""){
      data = &(fttuple->dataset());
    }
    else{
      data = fttuple->dataset().reduce(TString(local_cut_string));
      owned_data = true;
    }

    /// fit
    std::string handover_filename = "TriageHandoverFitParameter.txt";
    if (fttuple->nocut_startingvalues_file()==""){
      doocore::io::swarn << "-warning- \t" << "No 'no cut' starting values file name set. Using 'StartingValues_NoCut.txt' instead. Make sure this file exists!" << doocore::io::endmsg;
      fttuple->set_nocut_startingvalues_file("StartingValues_NoCut.txt");  
    }

    if (local_cut_string==""){
      doocore::io::tools::ReplaceScientificNotationInFile(fttuple->nocut_startingvalues_file(), debug_mode_);
      if (fttuple->sim_pdf()){
        fttuple->spdf()->getParameters(data)->readFromFile(TString(fttuple->nocut_startingvalues_file())); 
      }
      else{
       fttuple->pdf()->getParameters(data)->readFromFile(TString(fttuple->nocut_startingvalues_file())); 
      }
    }
    else{
      doocore::io::tools::ReplaceScientificNotationInFile(handover_filename, debug_mode_);
      if (fttuple->sim_pdf()){
        fttuple->spdf()->getParameters(data)->readFromFile(TString(handover_filename)); 
      }
      else{
        fttuple->pdf()->getParameters(data)->readFromFile(TString(handover_filename)); 
      }
    }

    // handles too long cut strings, maybe, there is a nicer method
    if (local_cut_string.size() > 50){
      local_cut_string.erase(15,local_cut_string.size());
      local_cut_string.append("…");
    }  

    if (fttuple->sim_pdf()){
      fttuple->spdf()->getParameters(data)->writeToFile("StartingValues.out");
      fttuple->spdf()->fitTo(*data, RooFit::Minimizer("Minuit2", "minimize"), RooFit::NumCPU(16), RooFit::Strategy(2), RooFit::Extended());
      fttuple->spdf()->getParameters(data)->writeToFile(TString(handover_filename));
      fttuple->spdf()->getParameters(data)->writeToFile(TString("FitResults_")+local_cut_string+".out");
    }
    else{
      fttuple->pdf()->getParameters(data)->writeToFile("StartingValues.out");
      fttuple->pdf()->fitTo(*data, RooFit::Minimizer("Minuit2", "minimize"), RooFit::NumCPU(16), RooFit::Strategy(2), RooFit::Extended());
      fttuple->pdf()->getParameters(data)->writeToFile(TString(handover_filename));
      fttuple->pdf()->getParameters(data)->writeToFile(TString("FitResults_")+local_cut_string+".out");
    }

    doocore::io::tools::ReplaceScientificNotationInFile(handover_filename, debug_mode_);

    // Plotting
    doofit::plotting::PlotConfig plot_cfg("plot_cfg");
    if (fttuple->sim_pdf()){
      doofit::plotting::PlotSimultaneous plot(plot_cfg, *fttuple->observable(), *data, *dynamic_cast<RooSimultaneous*>(fttuple->spdf()), fttuple->plot_components(), std::string(fttuple->observable()->GetName())+"_"+cut_string);
      plot.PlotItLogNoLogY();
    }
    else{
      doofit::plotting::Plot plot(plot_cfg, *fttuple->observable(), *data, *dynamic_cast<RooAbsPdf*>(fttuple->pdf()), fttuple->plot_components(), std::string(fttuple->observable()->GetName())+"_"+cut_string);
      plot.PlotItLogNoLogY();
    }

    number_of_events.first = fttuple->signal_yield()->getVal();
    number_of_events.second = fttuple->background_yield()->getVal();

    if (owned_data) delete data;
  }
  else if (tuple->is_absfitter()){
    AbsFitterTuple* afttuple = dynamic_cast<AbsFitterTuple*>(tuple);
    doofit::fitter::AbsFitter& fitter = afttuple->fitter();

    std::string local_cut_string = cut_string;
    if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "using a AbsFitter fit…" << doocore::io::endmsg;

    fitter.set_identifier(std::string("triage_") + afttuple->name() + "_" + cut_string);
    fitter.PrepareFit();

    
    RooAbsData* data;
    bool owned_data = false;
    if (local_cut_string==""){
      data = &(afttuple->dataset());
    }
    else{
      data = afttuple->dataset().reduce(TString(local_cut_string));
      owned_data = true;
    }
    
    /// fit
    std::string handover_filename = "TriageHandoverFitParameter.txt";
    if (afttuple->nocut_startingvalues_file()==""){
      doocore::io::swarn << "-warning- \t" << "No 'no cut' starting values file name set. Using 'StartingValues_NoCut.txt' instead. Make sure this file exists!" << doocore::io::endmsg;
      afttuple->set_nocut_startingvalues_file("StartingValues_NoCut.txt");
    }
    
    if (local_cut_string==""){
      doocore::io::tools::ReplaceScientificNotationInFile(afttuple->nocut_startingvalues_file(), debug_mode_);
      fitter.ReadParametersFile(afttuple->nocut_startingvalues_file());
    }
    else{
      doocore::io::tools::ReplaceScientificNotationInFile(handover_filename, debug_mode_);
      fitter.ReadParametersFile(handover_filename);
    }
    
    // handles too long cut strings, maybe, there is a nicer method
    if (local_cut_string.size() > 50){
      local_cut_string.erase(15,local_cut_string.size());
      local_cut_string.append("…");
    }
    
    fitter.set_dataset(data);
    fitter.Fit();
    fitter.WriteParametersFile(handover_filename);
    fitter.Plot();
    
    doocore::io::tools::ReplaceScientificNotationInFile(handover_filename, debug_mode_);
    
    number_of_events.first = fitter.SignalYield();
    number_of_events.second = fitter.BackgroundYield();
    
    if (owned_data) delete data;
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "std::pair<double, double> Triage::NumberOfSigAndBkgEvents(Tuple* tuple, const std::string& cut_string)" << doocore::io::endmsg;
  }
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "Sum of signal events: " << number_of_events.first << " @cut: " << cut_string << doocore::io::endmsg;
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "Sum of background events: " << number_of_events.second << " @cut: " << cut_string << doocore::io::endmsg;
  return number_of_events;
}

std::pair<TH1D, TH1D> Triage::SigAndBkgEventNumbersHistogram(Tuple* tuple, Classifier* classifier, const int nbins){
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "calling std::pair<TH1D, TH1D> Triage::SigAndBkgEventNumbersHistogram(Tuple* tuple, Classifier* classifier) \t" << doocore::io::endmsg;
  std::pair<TH1D, TH1D> hist_number_sig_bkg_events;
  std::vector<double> cut_values;

  std::string hist_name = tuple->name()+"_"+classifier->name()+"_"+boost::lexical_cast<std::string>(nbins);
  
  TCanvas canvas("c", "c", 800, 600);
  canvas.SetLogy();
  TH1D hist_number_sig_events(TString(hist_name)+"_sig", TString("Cut scan for ")+classifier->expression(), nbins, classifier->range().first, classifier->range().second);
  TH1D hist_number_bkg_events(TString(hist_name)+"_bkg", TString("Cut scan for ")+classifier->expression(), nbins, classifier->range().first, classifier->range().second);
  hist_number_sig_events.GetXaxis()->SetTitle(TString("Cut on ")+classifier->name().c_str());
  hist_number_sig_events.GetYaxis()->SetTitle("# Signal Events");
  hist_number_sig_events.SetStats(false);
  hist_number_bkg_events.GetXaxis()->SetTitle(TString("Cut on ")+classifier->name().c_str());
  hist_number_bkg_events.GetYaxis()->SetTitle("# Background Events");
  hist_number_bkg_events.SetStats(false);

  double cut_value = classifier->range().first;
  double cut_step  = (classifier->range().second-classifier->range().first)/nbins;
  cut_values.push_back(cut_value);

  doocore::io::sinfo << "Signal and background event numbers for tuple '" << tuple->name() << "' and classifier '" << classifier->name().c_str() << "'" << doocore::io::endmsg;
  for (unsigned int i = 1; i<= nbins; ++i){
    if ((i%1) == 0){
      double frac = static_cast<double> (i)/nbins*100.0;
      printf("Progress %.2f %         \xd", frac);
      fflush(stdout);
    }
    std::string cut_string = classifier->expression()+classifier->cut_operator()+boost::lexical_cast<std::string>(cut_value);
    std::pair<double, double> number_sig_bkg_events = NumberOfSigAndBkgEvents(tuple, cut_string);
    hist_number_sig_events.SetBinContent(i, number_sig_bkg_events.first);
    hist_number_bkg_events.SetBinContent(i, number_sig_bkg_events.second);
    cut_value = cut_value + cut_step;
    cut_values.push_back(cut_value);
  }

  classifier->set_cut_values(cut_values);

  hist_number_sig_bkg_events.first = hist_number_sig_events;
  hist_number_sig_bkg_events.second = hist_number_bkg_events;
  hist_number_sig_events.Sumw2();
  hist_number_bkg_events.Sumw2();

  if (debug_mode_){
    hist_number_sig_events.Draw("E1");
    canvas.SaveAs(TString("hist_")+hist_name+"_sig"+".pdf");
    hist_number_bkg_events.Draw("E1");
    canvas.SaveAs(TString("hist_")+hist_name+"_bkg"+".pdf");
  }
  return hist_number_sig_bkg_events;
}


std::vector< std::pair<TH1D, TH1D> > Triage::yield_histograms(){
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "calling std::pair< std::string, std::pair<TH1D, TH1D> > Triage::yield_histograms() \t" << doocore::io::endmsg;
  std::vector< std::pair<TH1D, TH1D> > yield_histograms;
  if (hist_container_list_filled_){
    for(std::vector<TriageHistContainer>::iterator container = hist_container_list_.begin(); container != hist_container_list_.end(); container++){
      std::pair<TH1D, TH1D> efficiency_histogram_pair = container->hist_number_sig_bkg_events();
      yield_histograms.push_back(efficiency_histogram_pair);
    }
  }
  else{
    doocore::io::serr << "-ERROR- \t" << "Efficiency histograms are not yet filled. Call 'InitializeScans()' before!" << doocore::io::endmsg;
    assert(0);
  }
  return yield_histograms;
}

void Triage::save_yield_histograms(std::string filename){
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "calling void Triage::save_yield_histograms(std::string filename) \t" << doocore::io::endmsg;
  TFile * f = new TFile(filename.c_str(), "RECREATE");
  std::vector< std::pair<TH1D, TH1D> > yield_histograms = Triage::yield_histograms();
  for(std::vector< std::pair<TH1D, TH1D> >::const_iterator hists = yield_histograms.begin(); hists != yield_histograms.end(); hists++){
    TH1D sig_hist = (*hists).first;
    sig_hist.Write();
    TH1D bkg_hist = (*hists).second;
    bkg_hist.Write();
  }
  f->Close();
}

} // namespace triage
} // namespace dooselection
