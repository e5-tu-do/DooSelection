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

namespace dooselection{
namespace performance{

// internal namespace forward declarations
class SelectionTuple;
class SelectionClassifier;

std::map<std::string, double> NumberOfEventsPerComponent(SelectionTuple &stuple, std::string cut_string, bool debug_mode=false);

double FoM(SelectionTuple &stuple, std::string signal_component, std::string background_component, std::string cut_string, std::string figure_of_merit="Significance", bool debug_mode=false);

std::vector< std::pair<double, double> > FoMDistribution(SelectionTuple &stuple, SelectionClassifier &classifier, std::string signal_component, std::string background_component, std::string figure_of_merit="Significance", bool debug_mode=false);

} // namespace performance
} // namespace dooselection

#endif // DOOSELECTION_PERFORMANCE_H
