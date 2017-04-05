#include "FoM.h"

// from STL
#include <iostream>
#include <math.h> 

// from DooCore
#include "doocore/io/MsgStream.h"

namespace dooselection{
namespace triage{

double FoM::FigureOfMerit(const std::string& figure_of_merit, double n_sig_events, double n_bkg_events, double max_n_sig_events, double max_n_bkg_events){
  if (figure_of_merit == "SignalYield"){
    return SignalYield(n_sig_events);
  }
  else if(figure_of_merit == "SignalEfficiency"){
    return SignalEfficiency(n_sig_events, max_n_sig_events);
  }
  else if(figure_of_merit == "BackgroundEfficiency"){
    return SignalEfficiency(n_bkg_events, max_n_bkg_events);
  }
  else if(figure_of_merit == "BackgroundRejection"){
    return BackgroundRejection(n_bkg_events, max_n_bkg_events);
  }
  else if(figure_of_merit == "Significance"){
    return Significance(n_sig_events, n_bkg_events);
  }
  else if(figure_of_merit == "WeightedSignificance"){
    return WeightedSignificance(n_sig_events, n_bkg_events);
  }
  else if(figure_of_merit == "Purity"){
    return Purity(n_sig_events, n_bkg_events);
  }
  else if(figure_of_merit == "Punzi"){
    return Punzi(n_sig_events, n_bkg_events, max_n_sig_events);
  }
  else if(figure_of_merit == "Sin2Beta"){
    return Sin2Beta(n_sig_events, n_bkg_events);
  }
  else{
    return SignalYield(n_sig_events);
  }
}

double FoM::SignalYield(double n_sig_events){
  double fom_value = n_sig_events;
  return fom_value;
}

double FoM::SignalEfficiency(double n_sig_events, double max_n_sig_events){
  double fom_value = n_sig_events/max_n_sig_events;
  return fom_value;
}

double FoM::BackgroundEfficiency(double n_bkg_events, double max_n_bkg_events){
  double fom_value = n_bkg_events/max_n_bkg_events;
  return fom_value;
}

double FoM::BackgroundRejection(double n_bkg_events, double max_n_bkg_events){
  double fom_value = 1.-BackgroundEfficiency(n_bkg_events, max_n_bkg_events);
  return fom_value;
}

double FoM::Significance(double n_sig_events, double n_bkg_events){
  double fom_value = n_sig_events/sqrt(n_sig_events+n_bkg_events);
  return fom_value;
}

double FoM::WeightedSignificance(double n_sig_events, double n_bkg_events){
  const double alpha = 100.;
  n_sig_events = n_sig_events/alpha;
  double fom_value = n_sig_events/sqrt(n_sig_events+n_bkg_events);
  return fom_value;
}

double FoM::Purity(double n_sig_events, double n_bkg_events){
  double fom_value = n_sig_events/(n_sig_events+n_bkg_events);
  return fom_value;
}

double FoM::Punzi(double n_sig_events, double n_bkg_events, double max_n_sig_events){
  const double alpha = 2.;
  double fom_value = (n_sig_events/max_n_sig_events)/((alpha/2)+sqrt(n_bkg_events));
  return fom_value;
}

double FoM::Sin2Beta(double n_sig_events, double n_bkg_events){
  /// FoM derived by Frank Meier for the sin2beta analysis. Only correct if the mass range is 5200–5500 MeV!
  doocore::io::swarn << "-warning- \t" << "Your are using the sin2beta FoM. The numbers are only correct if the mass range is 5200-5500 MeV!" << doocore::io::endmsg;
  const double epsilon_eff = 0.0238;
  double fom_value = 1.818/sqrt(epsilon_eff*n_sig_events) + 0.00057*(n_bkg_events/n_sig_events);
  fom_value = 1/fom_value;
  return fom_value;
}

} // namespace triage
} // namespace dooselection
