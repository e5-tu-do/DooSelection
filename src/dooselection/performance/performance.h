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
 * @section ec_Usage Usage
 * 
 * Example code:
 * 
 * @code
 *  // from DooFit
 *  #include "doofit/builder/EasyPdf/EasyPdf.h"
 * 
 *  // from DooSelection
 *  #include "dooselection/performance/SelectionTuple.h"
 *  #include "dooselection/performance/SelectionClassifier.h"
 *  #include "dooselection/performance/performance.h"
 * 
 *  // EasyPdf
 *  doofit::builder::EasyPdf epdf;
 *  …
 * 
 *  // input tuple
 *  doocore::io::EasyTuple etuple("/fhgfs/groups/e5/lhcb/NTuples/data.root", "Bd2JpsiKS", RooArgList(…));
 * 
 *  // SelectionTuple
 *  std::map<std::string, std::string> map_of_components_and_yields;
 *  map_of_components_and_yields["signal"]="yieldSig";
 *  map_of_components_and_yields["background"]="yieldBkg";
 *  
 *  std::map<std::string, std::string> map_of_components_and_pdfs;
 *  map_of_components_and_pdfs["signal"]="pdfDoubleGauss";
 *  map_of_components_and_pdfs["background"]="pdfExponential";
 *  
 *  dooselection::performance::SelectionTuple stuple("stuple", "stuple", &etuple, &epdf, "varMass", map_of_components_and_pdfs, map_of_components_and_yields, epdf.Var("varMass").getMin(), epdf.Var("varMass").getMax());
 *  
 *  // SelectionClassifier
 *  dooselection::performance::SelectionClassifier BDT("BDT_classifier", "BDT_classifier", -0.1, 0.1, ">", 20, 0.);
 *  
 *  
 *  // Classifier distribution
 *  dooselection::performance::PlotClassifierDistributionOLD(stuple, BDT);
 *  // Cut efficiency
 *  dooselection::performance::PlotCutEfficiency(stuple, BDT, "", "signal", "background", "Significance");
 *
 * @endcode 
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
/** @name Number of events per component
 *  Functions to compute the number of events in a given tuple.
 */
/**
///@{
 *  @brief Computes the number of events in a given SelectionTuple with respect to a given cut string
 *
 *  @param stuple SelectionTuple
 *	@param cut_string cut string
 *  @return std::map containing std::string and double pair for component and number of events for this component
**/
std::map<std::string, double> NumberOfEventsPerComponent(SelectionTuple &stuple, std::string cut_string, bool debug_mode=false);
///@}

// =======================
// CLASSIFIER DISTRIBUTION
// =======================
/** @name Plotting classifier distributions
 *  Functions to plot classifier distributions
 */
/**
///@{
 *  @brief Plots distribution of a given SelectionClassifier in a given SelectionTuple
 *
 *	WARNING: Use PlotClassifierDistributionOLD since this version isn't stable yet.
 *
 *  @param stuple SelectionTuple
 *	@param classifier cut SelectionClassifier
**/
void PlotClassifierDistribution(SelectionTuple& stuple, SelectionClassifier& classifier, int nbins=100, bool debug_mode=false);
/**
 *  @brief Plots distribution of a given SelectionClassifier in a given SelectionTuple
 *
 *  @param stuple SelectionTuple
 *	@param classifier cut SelectionClassifier
**/
void PlotClassifierDistributionOLD(SelectionTuple& stuple, SelectionClassifier& classifier, int nbins=100, bool debug_mode=false);
///@}

// ===============
//  CUT EFFICIENCY
// ===============
/** @name Plotting cut efficiencies
 *  Functions to plot a mass distribution after applying a cut and compute cut efficiencies
 */
///@{
/**
 *  @brief Plots mass distributions after applying cut on SelectionClassifier for given SelectionTuple
 *
 *	WARNING: The mass variable is fixed to: B0_LOKI_MASS_JpsiKSConstr
 *
 *  @param stuple SelectionTuple
 *	@param classifier cut SelectionClassifier
 *	@param cut_string cut string
 *	@param signal_component signal component
 *	@param background_component background component
**/
void PlotCutEfficiency(SelectionTuple& stuple, SelectionClassifier& classifier, std::string cut_string, std::string signal_component, std::string background_component, std::string figure_of_merit="Significance", std::string output_prefix="", int nbins=100, bool logscale=false, bool debug_mode=false);
/**
 *  @brief Plots mass distributions after applying cut on SelectionClassifier for given SelectionTuple
 *
 *	This implementation uses the 'best cut string' deposited in the SelectionClassifier
 *
 *	WARNING: The mass variable is fixed to: B0_LOKI_MASS_JpsiKSConstr
 *
 *  @param stuple SelectionTuple
 *	@param classifier cut SelectionClassifier
 *	@param signal_component signal component
 *	@param background_component background component
**/
void PlotCutEfficiency(SelectionTuple& stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, std::string figure_of_merit="Significance", int nbins=100, bool logscale=false, bool debug_mode=false);
/**
 *  @brief Plots mass distributions after applying cut on SelectionClassifier for given SelectionTuple
 *
 *	This implementation scans the predefined range of the classifer deposited in the SelectionClassifier
 *
 *	WARNING: The mass variable is fixed to: B0_LOKI_MASS_JpsiKSConstr
 *
 *  @param stuple SelectionTuple
 *	@param classifier cut SelectionClassifier
 *	@param signal_component signal component
 *	@param background_component background component
**/
void PlotCutEfficiencyScan(SelectionTuple& stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, std::string figure_of_merit="Significance", int nbins=100, bool logscale=false, bool debug_mode=false);
///@}

// ==============
// FIGURE OF MERIT (FoM)
// ==============
/** @name Figure of Merit (FoM)
 *  Functions to calculate a FoM for a given cut string or plot the FoM distribution for different cut strings
 * 
 *	Available FoMs (S: number of signal events, B: number of background events):
 *	- Significance: S/sqrt(S+B)
 *	- Weighted Significance: (S/alpha)/sqrt(S/alpha+B)
 *	- Purity: S/(S+B)
 *	- Punzi: S/(alpha/2+sqrt(B))
 *	- Signal Efficiency: S/max(S)
 *	- Background Rejection: 1-B/max(B)
 *	- Signal Yield: S
 *	- Sin2BetaFoM: 1.818/sqrt(epsilon_eff*S) + 0.00057*(B/S)
 *	- - determined by Frank Meier using toys (epsilon_eff is the effective tagging efficiency)
 *
 */
///@{
/**
 *  @brief Computes the FoM for a given cut string
 *
 *  @param stuple SelectionTuple
 *	@param signal_component signal component
 *	@param background_component background component
 *	@param cut_string cut string
 *	@param figure_of_merit Figure of Merit, default is 'Significance'
 *
 *	@return FoM value
**/
double FoM(SelectionTuple& stuple, std::string signal_component, std::string background_component, std::string cut_string, std::string figure_of_merit="Significance", bool debug_mode=false);

/**
 *  @brief Computes the FoM distribution
 *
 *  @param stuple SelectionTuple
 *	@param classifier SelectionClassifier
 *	@param signal_component signal component
 *	@param background_component background component
 *	@param figure_of_merit Figure of Merit, default is 'Significance'
 *
 *	@return std::vector containing std::pairs of doubles as x and y values of the FoM distribution
**/
std::vector< std::pair<double, double> > FoMDistribution(SelectionTuple& stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, std::string figure_of_merit="Significance", bool debug_mode=false);

/**
 *  @brief Plots the FoM distribution
 *
 *  @param fom_distribution std::vector containing std::pairs of doubles as x and y values of the FoM distribution
 *	@param classifier SelectionClassifier
 *	@param figure_of_merit Figure of Merit, default is 'Significance'
 **/
void PlotFoMDistribution(std::vector< std::pair<double, double> > fom_distribution, SelectionClassifier& classifier, std::string figure_of_merit="Significance", std::string output_prefix="", bool debug_mode=false);

/**
 *  @brief Plots the FoM distribution
 *
 *  @param stuple SelectionTuple
 *	@param classifier SelectionClassifier
 *	@param signal_component signal component
 *	@param background_component background component
 *	@param figure_of_merit Figure of Merit, default is 'Significance'
 **/
void PlotFoMDistribution(SelectionTuple& stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, std::string figure_of_merit="Significance", bool debug_mode=false);
///@}

// ===========================
// RECEIVER OPERATING CHARACTERISTIC (ROC)
// ===========================
/** @name Receiver Operating Characteristic (ROC)
 *  Functions to calculate and plot ROC curves
 *
 */
///@{
/**
 *  @brief Computes the ROC curve
 *
 *  @param stuple SelectionTuple
 *	@param classifier SelectionClassifier
 *	@param signal_component signal component
 *	@param background_component background component
 *
 *	@return std::vector containing std::pairs of doubles as x and y values of the ROC curve
**/
std::vector< std::pair<double, double> > ROC(SelectionTuple& stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, bool debug_mode=false);

/**
 *  @brief Plots the ROC curve
 *
 *  @param roc std::vector containing std::pairs of doubles as x and y values of the ROC curve
 *	@param classifier_name name of the used classifier
 *
 *	@return std::vector containing std::pairs of doubles as x and y values of the ROC curve
**/
void PlotROCCurve(std::vector< std::pair<double, double> > roc, std::string classifier_name, std::string output_prefix="", bool debug_mode=false);

/**
 *  @brief Plots the ROC curve
 *
 *  @param stuple SelectionTuple
 *	@param classifier SelectionClassifier
 *	@param signal_component signal component
 *	@param background_component background component
 *
 *	@return std::vector containing std::pairs of doubles as x and y values of the ROC curve
**/
void PlotROCCurve(SelectionTuple& stuple, SelectionClassifier& classifier, std::string signal_component, std::string background_component, bool debug_mode=false);
///@}

} // namespace performance
} // namespace dooselection

#endif // DOOSELECTION_PERFORMANCE_H
