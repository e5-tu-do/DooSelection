#ifndef TRIAGE_ABSFITTERTUPLE_H
#define TRIAGE_ABSFITTERTUPLE_H

// from STL

// from ROOT

// from RooFit
#include "RooAbsPdf.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

// from TMVA

// from BOOST

// from DooCore

// from DooFit
#include <doofit/builder/EasyPdf/EasyPdf.h>

// from here
#include "Tuple.h"

// forward declarations
namespace doofit { namespace fitter { class AbsFitter; } }

namespace dooselection{
namespace triage{

class AbsFitterTuple: public Tuple{
 public:
  AbsFitterTuple(const std::string& name, const std::string& path, const std::string& filename, const std::string& treename, const RooArgSet& argset, doofit::fitter::AbsFitter& fitter):
    Tuple(name, path, filename, treename, argset, "noObservable", 0, 1),
    fitter_(fitter),
    nocut_startingvalues_file_("")
  {
    is_mc_ = false;
    is_sw_ = false;
    is_ft_ = false;
    is_absfitter_ = true;
  }

  // setter
  void set_nocut_startingvalues_file(std::string nocut_startingvalues_file){nocut_startingvalues_file_=nocut_startingvalues_file;}

  // getter
  doofit::fitter::AbsFitter& fitter() {return fitter_;}
  std::string nocut_startingvalues_file() const{return nocut_startingvalues_file_;}

 private:
  doofit::fitter::AbsFitter& fitter_;

  std::string nocut_startingvalues_file_;
};

} // namespace triage
} // namespace dooselection

#endif // TRIAGE_ABSFITTERTUPLE_H
