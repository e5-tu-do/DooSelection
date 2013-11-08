#ifndef TRIAGE_TRIAGEHISTCONTAINER_H
#define TRIAGE_TRIAGEHISTCONTAINER_H

// from STL

// from ROOT
#include "TH1D.h"
// from RooFit

// from TMVA

// from BOOST

// from DooCore

// from here
#include "Tuple.h"
#include "Classifier.h"
// forward declarations

namespace dooselection{
namespace triage{

class TriageHistContainer{
 public:
  TriageHistContainer(std::string name, const Tuple* tuple, const Classifier* classifier, double max_n_sig_events, double max_n_bkg_events, std::vector<double> cut_values, std::pair<TH1D, TH1D> hist_number_sig_bkg_events):
    name_(name),
    tuple_(tuple),
    classifier_(classifier),
    max_n_sig_events_(max_n_sig_events),
    max_n_bkg_events_(max_n_bkg_events),
    cut_values_(cut_values),
    hist_number_sig_bkg_events_(hist_number_sig_bkg_events){}

  // getter
  std::string name() const{return name_;}
  const Tuple* tuple() const{return tuple_;}
  std::string tuple_name() const{return tuple_->name();}
  const Classifier* classifier() const{return classifier_;}
  std::string classifier_name() const{return classifier_->name();}
  double max_n_sig_events() const{return max_n_sig_events_;}
  double max_n_bkg_events() const{return max_n_bkg_events_;}
  std::vector<double> cut_values() const{return cut_values_;}
  std::pair<TH1D, TH1D> hist_number_sig_bkg_events() {return hist_number_sig_bkg_events_;}

 private:
  std::string name_;
  const Tuple* tuple_;
  const Classifier* classifier_;
  double max_n_sig_events_;
  double max_n_bkg_events_;
  std::vector<double> cut_values_;
  std::pair<TH1D, TH1D> hist_number_sig_bkg_events_;
};

} // namespace triage
} // namespace dooselection

#endif // TRIAGE_TRIAGEHISTCONTAINER_H
