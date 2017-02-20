#ifndef TRIAGE_FOM_H
#define TRIAGE_FOM_H

// from STL
#include <string>
// from ROOT

// from RooFit

// from TMVA

// from BOOST

// from DooCore

// from here

// forward declarations

namespace dooselection{
namespace triage{

class FoM{
 public: 
  static double FigureOfMerit(const std::string& figure_of_merit, double n_sig_events, double n_bkg_events, double max_n_sig_events, double max_n_bkg_events);
  static double SignalYield(double n_sig_events);
  static double SignalEfficiency(double n_sig_events, double max_n_sig_events);
  static double BackgroundEfficiency(double n_bkg_events, double max_n_bkg_events);
  static double BackgroundRejection(double n_bkg_events, double max_n_bkg_events);
  static double Significance(double n_sig_events, double n_bkg_events);
  static double WeightedSignificance(double n_sig_events, double n_bkg_events);
  static double Purity(double n_sig_events, double n_bkg_events);
  static double Punzi(double n_sig_events, double n_bkg_events, double max_n_sig_events);
  static double Sin2Beta(double n_sig_events, double n_bkg_events);
};

} // namespace triage
} // namespace dooselection

#endif // TRIAGE_FOM_H
