#include "performance.h"
#include "SelectionTuple/SelectionTuple.h"
#include "SelectionClassifier/SelectionClassifier.h"

// from STL
#include <vector>
#include <map>
#include <string>

// from ROOT

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
  if (debug_mode) if (cut_string!="") doocore::io::serr << "-debug- " << "starting doocore::performance::NumberOfEvents() with cut '" << cut_string << "'…" << doocore::io::endmsg;
  if (debug_mode) if (cut_string=="") doocore::io::serr << "-debug- " << "starting doocore::performance::NumberOfEvents() without cut" << doocore::io::endmsg;
  std::map<std::string, double> components_and_yield_values;

  if(stuple.use_mc()){
    doocore::io::serr << "-ERROR- \t NumberOfEvents(SelectionTuple &stuple, std::string cut, bool debug_mode)- MC not yet implemented" << doocore::io::endmsg;
    return components_and_yield_values;
  } 
  else if(stuple.use_sweights()){
    doocore::io::serr << "-ERROR- \t NumberOfEvents(SelectionTuple &stuple, std::string cut, bool debug_mode)- sWeights not yet implemented" << doocore::io::endmsg;
    return components_and_yield_values;
  }
  else if(stuple.use_fit()){
    if (debug_mode) doocore::io::serr << "-debug- \t" << "using a fit…" << doocore::io::endmsg;
    if (debug_mode) doocore::io::serr << "-debug- \t" << "with PDF:" << doocore::io::endmsg;
    if (debug_mode) stuple.epdf().Pdf("pdf").printCompactTree();

    // cut on data
    RooAbsData* data;
    if (cut_string==""){
      data = &stuple.dataset();
    }
    else{
      data = stuple.dataset().reduce(TString(cut_string));
    }
    
    // fit
    stuple.epdf().Pdf("pdf").getParameters(data)->readFromFile("StartingValues.in");
    stuple.epdf().Pdf("pdf").getParameters(data)->writeToFile("StartingValues.out");
    stuple.epdf().Pdf("pdf").fitTo(*data, RooFit::Minimizer("Minuit2", "minimize"), RooFit::NumCPU(16), RooFit::Strategy(2), RooFit::Extended());
    stuple.epdf().Pdf("pdf").getParameters(data)->writeToFile(TString("FitResults.out"));

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
  else{

  }
}

// =====================
// FIGURE OF MERIT (FoM)
// =====================
/// calculate FoM for a single cut value
double FoM(SelectionTuple &stuple, std::string signal_component, std::string background_component, std::string cut_string, std::string figure_of_merit, bool debug_mode){
  if (debug_mode) doocore::io::serr << "-debug- " << "starting doocore::performance::FoM()…" << doocore::io::endmsg;
  if ((stuple.map_of_components_and_yields().at(signal_component)==0) || (stuple.map_of_components_and_yields().at(background_component)==0)) doocore::io::serr << "-ERROR- \t" << "Signal or background component to not exist in SelectionTuple '" << stuple.title() << "'!" << doocore::io::endmsg;
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
  
  std::pair<double, double> xy_values;
  std::vector< std::pair<double, double> > vector_of_xy_values;

  std::string cut_string;

  double fom_value = 0;

  for(std::vector<double>::iterator it = steps.begin(); it != steps.end(); it++){
    cut_string=classifier.name()+classifier.cut_operator()+boost::lexical_cast<std::string>(*it);
    if (debug_mode) doocore::io::serr << "-debug- " << "cut string: " << cut_string << doocore::io::endmsg;
  }

  // for (double cut_value = classifier.range_min(); cut_value < classifier.range_max(); cut_value+=1) 
  // {
  //   fom_value = FoM(stuple, signal_component, background_component, cut_string, figure_of_merit, debug_mode);

  //   if (std::isfinite(fom_value)){
  //       xy_values.first = cut_value - 1/2;
  //       xy_values.second = fom_value;
  //       vector_of_xy_values.push_back(xy_values);
  //   }
  // }
  return vector_of_xy_values;
}

} // namespace performance
} // namespace dooselection