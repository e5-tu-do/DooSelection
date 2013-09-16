#ifndef TRIAGE_FTTUPLE_H
#define TRIAGE_FTTUPLE_H

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
#include "doofit/builder/EasyPdf/EasyPdf.h"

// from here
#include "Tuple.h"

// forward declarations

namespace dooselection{
namespace triage{

class FTTuple: public Tuple{
 public:
  FTTuple(const std::string& name, const std::string& path, const std::string& filename, const std::string& treename, const RooArgSet& argset, RooAbsPdf* pdf, const RooRealVar& observable, const RooRealVar& signal_yield, const RooRealVar& background_yield, const std::vector<std::string> plot_components):
    Tuple(name, path, filename, treename, argset, observable.GetName(), observable.getMin(), observable.getMax()),
    sim_pdf_(false),
    pdf_(pdf),
    spdf_(NULL),
    observable_(&observable),
    signal_yield_(&signal_yield),
    background_yield_(&background_yield),
    plot_components_(plot_components),
    nocut_startingvalues_file_("")
  {
    is_mc_ = false;
    is_sw_ = false;
    is_ft_ = true;
  }

  FTTuple(const std::string& name, const std::string& path, const std::string& filename, const std::string& treename, const RooArgSet& argset, RooSimultaneous* spdf, const RooRealVar& observable, const RooRealVar& signal_yield, const RooRealVar& background_yield, const std::vector<std::string> plot_components):
    Tuple(name, path, filename, treename, argset, observable.GetName(), observable.getMin(), observable.getMax()),
    sim_pdf_(true),
    pdf_(NULL),
    spdf_(spdf),
    observable_(&observable),
    signal_yield_(&signal_yield),
    background_yield_(&background_yield),
    plot_components_(plot_components),
    nocut_startingvalues_file_("")
  {
    is_mc_ = false;
    is_sw_ = false;
    is_ft_ = true;
  }

  // setter
  void set_nocut_startingvalues_file(std::string nocut_startingvalues_file){nocut_startingvalues_file_=nocut_startingvalues_file;}

  // getter
  bool sim_pdf() {return sim_pdf_;}
  RooAbsPdf* pdf() const{return pdf_;}
  RooSimultaneous* spdf() const{return spdf_;}
  const RooRealVar* observable() const{return observable_;}
  const RooRealVar* signal_yield() const{return signal_yield_;}
  const RooRealVar* background_yield() const{return background_yield_;}
  std::vector<std::string> plot_components() const{return plot_components_;}
  std::string nocut_startingvalues_file() const{return nocut_startingvalues_file_;}  

 private:
  bool sim_pdf_;

  RooAbsPdf* pdf_;
  RooSimultaneous* spdf_;
  const RooRealVar* observable_;
  const RooRealVar* signal_yield_;
  const RooRealVar* background_yield_;
  std::vector<std::string> plot_components_;

  std::string nocut_startingvalues_file_;
};

} // namespace triage
} // namespace dooselection

#endif // TRIAGE_FTTUPLE_H
