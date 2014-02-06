#ifndef TRIAGE_TRIAGE_H
#define TRIAGE_TRIAGE_H

// from STL

// from ROOT

// from RooFit

// from TMVA

// from BOOST

// from DooCore

// from here
#include "TupleList.h"
#include "ClassifierList.h"
#include "TriageHistContainer.h"

// forward declarations
class TH1D;

namespace dooselection{
namespace triage{

enum class PlotStyle{
  Nothing,
  All,
  NameOnly
};

class Triage{
 public:
  Triage(Tuple& tuple, Classifier& classifier, bool debug_mode):
    tuple_list_(),
    classifier_list_(),
    nbins_(0),
    hist_container_list_filled_(false),
    debug_mode_(debug_mode)
  {
    tuple_list_.Add(tuple);
    classifier_list_.Add(classifier);
  }

  Triage(Tuple& tuple, Classifier& classifier):
    tuple_list_(),
    classifier_list_(),
    nbins_(0),
    hist_container_list_filled_(false),
    debug_mode_(false)
  {
    tuple_list_.Add(tuple);
    classifier_list_.Add(classifier);
  }

  Triage(Tuple& tuple, ClassifierList& classifier_list, bool debug_mode):
    tuple_list_(),
    classifier_list_(classifier_list),
    nbins_(0),
    hist_container_list_filled_(false),
    debug_mode_(debug_mode)
  {
    tuple_list_.Add(tuple);
  }

  Triage(Tuple& tuple, ClassifierList& classifier_list):
    tuple_list_(),
    classifier_list_(classifier_list),
    nbins_(0),
    hist_container_list_filled_(false),
    debug_mode_(false)
  {
    tuple_list_.Add(tuple);
  }

  Triage(TupleList& tuple_list, Classifier& classifier, bool debug_mode):
    tuple_list_(tuple_list),
    classifier_list_(),
    nbins_(0),
    hist_container_list_filled_(false),
    debug_mode_(debug_mode)
  {
    classifier_list_.Add(classifier);
  }

  Triage(TupleList& tuple_list, Classifier& classifier):
    tuple_list_(tuple_list),
    classifier_list_(),
    nbins_(0),
    hist_container_list_filled_(false),
    debug_mode_(false)
  {
    classifier_list_.Add(classifier);
  }

  Triage(TupleList& tuple_list, ClassifierList& classifier_list, bool debug_mode):
    tuple_list_(tuple_list),
    classifier_list_(classifier_list),
    nbins_(0),
    hist_container_list_filled_(false),
    debug_mode_(debug_mode){}

  Triage(TupleList& tuple_list, ClassifierList& classifier_list):
    tuple_list_(tuple_list),
    classifier_list_(classifier_list),
    nbins_(0),
    hist_container_list_filled_(false),
    debug_mode_(false){}

  ~Triage(){}

  /// {
  /// This method initializes all tuples and classifiers for performance scans.
  /// It fills the TriageHistContainer list with all available TriageHistContainers.
  void InitializeScans(const int nbins);

  /// Plots signal/background distributions for all given classifiers.
  void ClassifierDistributions(const int nbins);

  /// Plots mass distribution for all tuples using the best cut for each classifier.
  /// The plots show the mass distribution before and after the cut. The efficiencies 
  /// for the best cut are calculated.
  void BestCutPerformances(PlotStyle plot_style=PlotStyle::All);
  /// }


  /// { methods for performance evaluation
  /// Plots signal/background distributions for a given classifier
  void ClassifierDistribution(Tuple* tuple, Classifier* classifier, const int nbins);

  /// Plots mass distribution for a tuple using the best cut for a classifier.
  /// The plots show the mass distribution before and after the cut. The efficiencies 
  /// for the best cut are calculated.
  void BestCutPerformance(Tuple* tuple, Classifier* classifier, PlotStyle plot_style=PlotStyle::All);

  /// Calculates and plots the efficiencies, ROC curve, etc. for all tuples and classifiers
  void PerformanceScans();

  /// Plots the ROC curve
  void PlotROC(std::string name, std::pair<TH1D, TH1D> hist_number_sig_bkg_events, double max_n_sig_events, double max_n_bkg_events);
  /// Plots the signal efficiency distribution
  void PlotSignalEfficiency(std::string name, std::pair<TH1D, TH1D> hist_number_sig_bkg_events, std::vector<double> cut_steps, double max_n_sig_events, double max_n_bkg_events);
  /// Plots the background rejection distribution
  void PlotBackgroundRejection(std::string name, std::pair<TH1D, TH1D> hist_number_sig_bkg_events, std::vector<double> cut_steps, double max_n_sig_events, double max_n_bkg_events);
  /// Plots all efficiency distributions
  void PlotEfficiencies(std::string name, std::pair<TH1D, TH1D> hist_number_sig_bkg_events, std::vector<double> cut_steps, double max_n_sig_events, double max_n_bkg_events);
  /// }

  /// { getter
  std::vector< std::pair<TH1D, TH1D> > yield_histograms();
  void save_yield_histograms(std::string filename="yield_histograms.root");
  /// }

  /// { setter
  void set_debug_mode(bool debug_mode) {debug_mode_ = debug_mode;}
  void set_nbins(const int nbins) {nbins_ = nbins;}
  /// }

 private:
  /// This method fills the TriageHistContainer list with all available TriageHistContainers.
  /// A TriageHistContainer is described by a unique name and contains a combination of tuple and classifier. Additionaly the maximal
  /// number of events in this tuple and the vector of scan points/cut values with respect to the classifer are saved.
  /// Most importantly two histograms containing the number of events for every scan point are saved for further evaluation.
  /// To do so, this method calls SigAndBkgEventNumbersHistogram for every classifier and every tuple provided
  void FillTriageHistContainerList(const int nbins);
  
  /// This method computes the maximal number of events and save the number in the tuple
  void MaximalNumberOfEvents(Tuple* tuple); 

  /// MOST BASIC METHOD
  /// Given a specific cut this method computes the number of events in the tuple
  /// WARNING: at the moment this only works with sWeighted data!
  std::pair<double, double> NumberOfSigAndBkgEvents(Tuple* tuple, const std::string& cut_string);
  
  /// This method fills two histograms with the number of signal and background events, depending on a given cut on the classifier
  /// It uses NumberOfSigAndBkgEvents to perform this task.
  std::pair<TH1D, TH1D> SigAndBkgEventNumbersHistogram(Tuple* tuple, Classifier* classifier, const int nbins);

  bool debug_mode_;
  bool hist_container_list_filled_;

  int nbins_;

  TupleList tuple_list_;
  ClassifierList classifier_list_;
  std::vector<TriageHistContainer> hist_container_list_;
};

} // namespace triage
} // namespace dooselection

#endif // TRIAGE_TRIAGE_H
