#include "Triage.h"

// from STL

// from ROOT
#include "TCanvas.h"
#include "TTree.h"

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

void Triage::ClassifierDistributions(const int nbins){
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "calling void ClassifierDistributions(const int nbins) \t" << doocore::io::endmsg;
  for(TupleList::iterator tuple = tuple_list_.begin(); tuple != tuple_list_.end(); tuple++){
    for(ClassifierList::iterator classifier = classifier_list_.begin(); classifier != classifier_list_.end(); classifier++){
      ClassifierDistribution(*tuple, &(*classifier), nbins);
    }
  }
}

void Triage::ClassifierDistribution(Tuple* tuple, Classifier* classifier, const int nbins){
  if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "calling void Triage::ClassifierDistribution(Tuple* tuple, Classifier* classifier, const int nbins) \t" << doocore::io::endmsg;
  doocore::lutils::setStyle("LHCb");

  if (tuple->is_mc()){
    MCTuple* mctuple = dynamic_cast<MCTuple*>(tuple);
    if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "using MC truth information…" << doocore::io::endmsg;

    std::pair<double,double> minmax = classifier->range();
    if (minmax.first == 0. && minmax.second == 0.){
      minmax = doocore::lutils::MedianLimitsForTuple(mctuple->tree(), classifier->expression());
    }
    
    if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "min " << minmax.first << " max " << minmax.second << doocore::io::endmsg;

    TCanvas* canvas = new TCanvas("canvas", "canvas", 800, 600);
    TH1D* sig_hist = new TH1D("sig_hist", "sig_hist", nbins, minmax.first , minmax.second);
    TH1D* bkg_hist = new TH1D("bkg_hist", "bkg_hist", nbins, minmax.first , minmax.second);

    std::string observable_range = "(" + mctuple->observable_name() + ">" + std::to_string(mctuple->observable_range().first) + ")&&(" + mctuple->observable_name() + "<" + std::to_string(mctuple->observable_range().second) + ")";
    std::string interim_cut_string = observable_range;
    std::string signal_cut_string = interim_cut_string + "&&(" + mctuple->signal_cut() + ")";
    std::string background_cut_string = interim_cut_string + "&&(" + mctuple->background_cut() + ")";

    if (debug_mode_) doocore::io::sdebug << "-debug- \t" << " signal cut string: " << signal_cut_string << doocore::io::endmsg;
    if (debug_mode_) doocore::io::sdebug << "-debug- \t" << " background cut string: " << background_cut_string << doocore::io::endmsg;

    mctuple->tree().Draw(TString(classifier->expression())+">>sig_hist", signal_cut_string.c_str());
    mctuple->tree().Draw(TString(classifier->expression())+">>bkg_hist", background_cut_string.c_str());

    sig_hist->Print();
    bkg_hist->Print();

    sig_hist->SetLineColor(kBlue);
    sig_hist->SetMarkerColor(kBlue);
    sig_hist->SetFillColor(kBlue-9);
    sig_hist->SetFillStyle(3005);
    sig_hist->SetMinimum(0);
    sig_hist->SetXTitle(TString(classifier->expression()));

    bkg_hist->SetLineColor(kRed);
    bkg_hist->SetMarkerColor(kRed);
    bkg_hist->SetFillColor(kRed-9);
    bkg_hist->SetFillStyle(3004);
    bkg_hist->SetMinimum(0);
    bkg_hist->SetXTitle(TString(classifier->expression()));

    doocore::lutils::drawNormalizedOrdered(sig_hist, bkg_hist);
    doocore::lutils::printPlot (canvas, "ClassifierDistribution_"+mctuple->name()+"_"+classifier->name(), "ClassifierDistributions/");

    delete sig_hist;
    delete bkg_hist;
    delete canvas;
  }
  else if (tuple->is_sw()){
    SWTuple* swtuple = dynamic_cast<SWTuple*>(tuple);
    if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "using sweights…" << doocore::io::endmsg;

    double sig_sweight, bkg_sweight, observable_value, classifier_value;

    std::pair<double,double> minmax = doocore::lutils::MedianLimitsForTuple(swtuple->tree(), classifier->expression());
    if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "min " << minmax.first << " max " << minmax.second << doocore::io::endmsg;

    swtuple->tree().SetBranchAddress(TString(swtuple->signal_sweight()), &sig_sweight);
    swtuple->tree().SetBranchAddress(TString(swtuple->background_sweight()), &bkg_sweight);
    swtuple->tree().SetBranchAddress(TString(swtuple->observable_name()), &observable_value);
    swtuple->tree().SetBranchAddress(TString(classifier->expression()), &classifier_value);

    double observable_min = swtuple->observable_range().first;
    double observable_max = swtuple->observable_range().second;

    TH1D* sig_hist = new TH1D("sig_hist", "sig_hist", nbins, minmax.first , minmax.second);
    TH1D* bkg_hist = new TH1D("bkg_hist", "bkg_hist", nbins, minmax.first , minmax.second);

    if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "loop over tree... "  << doocore::io::endmsg;
    int entries = swtuple->tree().GetEntries();
    for (int i = 0; i < entries; ++i){
      if ((i%10000)==0){
        if (debug_mode_) doocore::io::sdebug << "-debug- \t" << "#" << i << doocore::io::endmsg;
      }
      swtuple->tree().GetEvent(i);

      // observable window cut
      if ((observable_value<observable_min) || (observable_value>observable_max)) continue;
      
      sig_hist->Fill(classifier_value, sig_sweight);
      bkg_hist->Fill(classifier_value, bkg_sweight);
    }   
    
    swtuple->tree().ResetBranchAddress(swtuple->tree().GetBranch(TString(swtuple->signal_sweight())));
    swtuple->tree().ResetBranchAddress(swtuple->tree().GetBranch(TString(swtuple->background_sweight())));
    swtuple->tree().ResetBranchAddress(swtuple->tree().GetBranch(TString(swtuple->observable_name())));
    swtuple->tree().ResetBranchAddress(swtuple->tree().GetBranch(TString(classifier->expression())));

    sig_hist->SetLineColor(kBlue);
    sig_hist->SetMarkerColor(kBlue);
    sig_hist->SetFillColor(kBlue-9);
    sig_hist->SetFillStyle(3005);
    sig_hist->SetMinimum(0);
    sig_hist->SetXTitle(TString(classifier->expression()));

    bkg_hist->SetLineColor(kRed);
    bkg_hist->SetMarkerColor(kRed);
    bkg_hist->SetFillColor(kRed-9);
    bkg_hist->SetFillStyle(3004);
    bkg_hist->SetMinimum(0);
    bkg_hist->SetXTitle(TString(classifier->expression()));

    TCanvas* canvas = new TCanvas("canvas", "canvas", 800, 600);
    
    doocore::lutils::drawNormalizedOrdered(sig_hist, bkg_hist);
    doocore::lutils::printPlot (canvas, "ClassifierDistribution_"+swtuple->name()+"_"+classifier->name(), "ClassifierDistributions/");

    delete sig_hist;
    delete bkg_hist;
    delete canvas;
  }
  else if (tuple->is_ft()){
    // TO-DO
  } 
  else{
    doocore::io::serr << "-ERROR- \t" << "void Triage::ClassifierDistribution(Tuple* tuple, Classifier* classifier)" << doocore::io::endmsg;
  }
}

} // namespace triage
} // namespace dooselection
