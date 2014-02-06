#ifndef TRIAGE_MCTUPLE_H
#define TRIAGE_MCTUPLE_H

// from STL

// from ROOT

// from RooFit

// from TMVA

// from BOOST

// from DooCore

// from here
#include "Tuple.h"

// forward declarations

namespace dooselection{
namespace triage{

class MCTuple: public Tuple{
 public:
  MCTuple(const std::string& name, const std::string& path, const std::string& filename, const std::string& treename, const RooArgSet& argset, const std::string& observable_name, double observable_range_min, double observable_range_max, const std::string& signal_cut, const std::string& background_cut):
    Tuple(name, path, filename, treename, argset, observable_name, observable_range_min, observable_range_max),
    signal_cut_(signal_cut),
    background_cut_(background_cut)
  {
    is_mc_ = true;
    is_sw_ = false;
    is_ft_ = false;
  }

  // setter
  void set_signal_cut(const std::string& signal_cut){signal_cut_ = signal_cut;}
  void set_background_cut(const std::string& background_cut){background_cut_ = background_cut;}

  // getter
  std::string signal_cut() const{return signal_cut_;}
  std::string background_cut() const{return background_cut_;}

 private:
  std::string signal_cut_;
  std::string background_cut_;
};

} // namespace triage
} // namespace dooselection

#endif // TRIAGE_MCTUPLE_H
