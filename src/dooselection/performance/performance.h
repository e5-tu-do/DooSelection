#ifndef DOOSELECTION_PERFORMANCE_H
#define DOOSELECTION_PERFORMANCE_H

// from STL
#include <map>
#include <string>

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

std::map<std::string, double> NumberOfEvents(SelectionTuple &stuple, std::string cut_string, bool debug_mode=false);

} // namespace performance
} // namespace dooselection

#endif // DOOSELECTION_PERFORMANCE_H
