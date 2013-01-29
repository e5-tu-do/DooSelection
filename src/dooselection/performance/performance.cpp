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

std::map<std::string, double> NumberOfEvents(SelectionTuple &stuple, std::string cut_string, bool debug_mode){
  if (debug_mode) if (cut_string!="") doocore::io::serr << "-debug- " << "starting doocore::performance::NumberOfEvents() with cut '" << cut_string << "'..." << doocore::io::endmsg;
  if (debug_mode) if (cut_string=="") doocore::io::serr << "-debug- " << "starting doocore::performance::NumberOfEvents() without cut" << doocore::io::endmsg;
  std::map<std::string, double> components_and_yield_values;
  components_and_yield_values["bla"] = 10.;

  if(stuple.use_mc()){
    doocore::io::serr << "-NumberOfEvents(SelectionTuple &stuple, std::string cut, bool debug_mode)- MC not yet implemented" << doocore::io::endmsg;
    return components_and_yield_values;
  } 
  else if(stuple.use_sweights()){
    doocore::io::serr << "-NumberOfEvents(SelectionTuple &stuple, std::string cut, bool debug_mode)- sWeights not yet implemented" << doocore::io::endmsg;
    return components_and_yield_values;
  }
  else if(stuple.use_fit()){
    if (debug_mode) doocore::io::serr << "-debug- " << "...using a fit..." << doocore::io::endmsg;
    if (debug_mode) doocore::io::serr << "-debug- " << "...with PDF:" << doocore::io::endmsg;
    
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

    doocore::io::serr << "loop over component and pdfs map" << doocore::io::endmsg;
    for(std::map<std::string, std::string>::const_iterator it = stuple.map_of_components_and_pdfs().begin(); it != stuple.map_of_components_and_pdfs().end(); ++it){
      doocore::io::sout << "Component: " << (*it).first << ", Yield: " << (*it).second << doocore::io::endmsg;
      plot_pdfs.add(stuple.epdf().Pdf((*it).second));
    }
    doocore::io::serr << "end of loop" << doocore::io::endmsg;

    doofit::plotting::PlotConfig plot_cfg("plot_cfg");
    doofit::plotting::Plot plot(plot_cfg, stuple.epdf().Var("B0_LOKI_MASS_JpsiKSConstr"), *data, plot_pdfs, "mass_"+cut_string);
    plot.PlotIt();

    doocore::io::serr << "loop over component and yields map" << doocore::io::endmsg;
    for(std::map<std::string, std::string>::const_iterator it = stuple.map_of_components_and_yields().begin(); it != stuple.map_of_components_and_yields().end(); ++it){
      doocore::io::sout << "Component: " << (*it).first << ", PDF: " << (*it).second << doocore::io::endmsg;
      components_and_yield_values[(*it).first]=stuple.epdf().Var((*it).second).getValV();
    }
    doocore::io::serr << "end of loop" << doocore::io::endmsg;
    
    doocore::io::serr << "loop over component and yield values map" << doocore::io::endmsg;
    for(std::map<std::string, double>::const_iterator it = components_and_yield_values.begin(); it != components_and_yield_values.end(); ++it){
      doocore::io::sout << "Component: " << (*it).first << ", Yield Value: " << (*it).second << doocore::io::endmsg;
    }
    doocore::io::serr << "end of loop" << doocore::io::endmsg;

    return components_and_yield_values;
  }
  else{

  }
}

} // namespace performance
} // namespace dooselection