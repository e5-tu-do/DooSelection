#ifndef DOOSELECTION_PERFORMANCE_H
#define DOOSELECTION_PERFORMANCE_H

// from STL
#include <map>
#include <string>
#include <vector>

// from ROOT

// from RooFit

// from TMVA

// from BOOST

// from DooCore

// from DooFit

// from DooSelection

// forward declarations

/**
 * @namespace dooselection::performance
 * @brief Tooles for selection classifier performance measurements
 * 
 */

namespace dooselection{
namespace performance{

// internal namespace forward declarations
class SelectionTuple;
class SelectionClassifier;

// ==============================
// NUMBER OF EVENTS PER COMPONENT
// ==============================
std::map<std::string, double> NumberOfEventsPerComponent(SelectionTuple &stuple, std::string cut_string, bool debug_mode=false);

// =======================
// CLASSIFIER DISTRIBUTION
// =======================
void PlotClassifierDistribution(SelectionTuple& stuple, SelectionClassifier& classifier, int nbins=100, bool debug_mode=false);
void PlotClassifierDistributionOLD(SelectionTuple& stuple, SelectionClassifier& classifier, int nbins=100, bool debug_mode=false);

// ===============
//  CUT EFFICIENCY
// ===============
void PlotCutEfficiency(SelectionTuple& stuple, SelectionClassifier& classifier, std::string cut_string, std::string signal_component, std::string background_component, std::string figure_of_merit="Significance", std::string output_prefix="", int nbins=100, bool logscale=false, bool debug_mode=false);
void PlotCutEfficiency(SelectionTuple& stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, std::string figure_of_merit="Significance", int nbins=100, bool logscale=false, bool debug_mode=false);
void PlotCutEfficiencyScan(SelectionTuple& stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, std::string figure_of_merit="Significance", int nbins=100, bool logscale=false, bool debug_mode=false);

// ==============
// FIGURE OF MERIT (FoM)
// ==============
/// calculate FoM for a single cut value
double FoM(SelectionTuple& stuple, std::string signal_component, std::string background_component, std::string cut_string, std::string figure_of_merit="Significance", bool debug_mode=false);

/// calculate FoM distribution for different cut values in a given range
std::vector< std::pair<double, double> > FoMDistribution(SelectionTuple& stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, std::string figure_of_merit="Significance", bool debug_mode=false);

/// plot FoM distribution in a given range
void PlotFoMDistribution(std::vector< std::pair<double, double> > fom_distribution, SelectionClassifier& classifier, std::string figure_of_merit="Significance", std::string output_prefix="", bool debug_mode=false);
void PlotFoMDistribution(SelectionTuple& stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, std::string figure_of_merit="Significance", bool debug_mode=false);

// ===========================
// RECEIVER OPERATING CHARACTERISTIC (ROC)
// ===========================
/// calculate ROC for a given classifier
std::vector< std::pair<double, double> > ROC(SelectionTuple& stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, bool debug_mode=false);

/// plot ROC Curve
void PlotROCCurve(std::vector< std::pair<double, double> > roc, std::string classifier_name, std::string output_prefix="", bool debug_mode=false);
void PlotROCCurve(SelectionTuple& stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, bool debug_mode=false);

} // namespace performance
} // namespace dooselection

#endif // DOOSELECTION_PERFORMANCE_H
