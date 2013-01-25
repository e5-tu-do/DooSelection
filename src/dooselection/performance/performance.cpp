#include "performance.h"
#include "SelectionTuple/SelectionTuple.h"
#include "SelectionClassifier/SelectionClassifier.h"

// from STL
#include <vector>
#include <string>

// from ROOT

// from RooFit
#include "RooAbsPdf.h"
#include "RooAbsData.h"
#include "RooDataSet.h"

// from TMVA

// from BOOST

// from DooCore
#include "doocore/io/MsgStream.h"

// from DooFit
#include "doofit/builder/EasyPdf/EasyPdf.h"

// from DooSelection

// forward declarations

namespace dooselection{
namespace performance{

std::map<std::string, double> NumberOfEvents(SelectionTuple &stuple, std::string cut_string, bool debug_mode){
  if (debug_mode) doocore::io::serr << "-debug- " << "starting doocore::performance::NumberOfEvents() with cut '" << cut_string << "'..." << doocore::io::endmsg;
  std::map<std::string, double> components_and_yields;

  if(stuple.use_mc()){
    doocore::io::serr << "-NumberOfEvents(SelectionTuple &stuple, std::string cut, bool debug_mode)- MC not yet implemented" << doocore::io::endmsg;
    return components_and_yields;
  } 
  else if(stuple.use_sweights()){
    doocore::io::serr << "-NumberOfEvents(SelectionTuple &stuple, std::string cut, bool debug_mode)- sWeights not yet implemented" << doocore::io::endmsg;
    return components_and_yields;
  }
  else if(stuple.use_fit()){
    if (debug_mode) doocore::io::serr << "-debug- " << "...using a fit..." << doocore::io::endmsg;
    if (debug_mode) doocore::io::serr << "-debug- " << "...with PDF:" << doocore::io::endmsg;
    if (debug_mode) stuple.epdf()->Pdf("pdf").printCompactTree();

    // cut on data
    RooAbsData* data;
    if (cut_string==""){
      data = stuple.dataset();
    }
    else{
      data = stuple.dataset()->reduce(TString(cut_string));
    }
    
    for(std::map<std::string, std::string>::iterator it = stuple.map_of_components_and_pdfs().begin(); it != stuple.map_of_components_and_pdfs().end(); it++){
      doocore::io::sout << "First: " << it->first << " Second " << it->second << doocore::io::endmsg;
    }

    return components_and_yields;
  }
  else{

  }
}

} // namespace performance
} // namespace dooselection