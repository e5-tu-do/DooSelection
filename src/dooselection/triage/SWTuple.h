#ifndef TRIAGE_SWTUPLE_H
#define TRIAGE_SWTUPLE_H

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

class SWTuple: public Tuple{
 public:
  SWTuple(const std::string& name, const std::string& path, const std::string& filename, const std::string& treename, const RooArgSet& argset, const std::string& observable_name, double observable_range_min, double observable_range_max, const std::string& signal_sweight, const std::string& background_sweight, const double sweights_range_min, const double sweights_range_max):
    Tuple(name, path, filename, treename, argset, observable_name, observable_range_min, observable_range_max),
    signal_sweight_(signal_sweight),
    background_sweight_(background_sweight),
    sweights_range_(std::make_pair(sweights_range_min,sweights_range_max))
  {
    is_mc_ = false;
    is_sw_ = true;
    is_ft_ = false;
  }

  // setter
  void set_signal_sweight(const std::string& signal_sweight){signal_sweight_ = signal_sweight;}
  void set_background_sweight(const std::string& background_sweight){background_sweight_ = background_sweight;}
  void set_range(double min, double max){sweights_range_.first = min; sweights_range_.second = max;}
  void set_range(const std::pair< double, double> range){sweights_range_ = range;}

  // getter
  std::string signal_sweight() const{return signal_sweight_;}
  std::string background_sweight() const{return background_sweight_;}
  std::pair< double, double> sweights_range() const{return sweights_range_;}

 private:
  std::string signal_sweight_;
  std::string background_sweight_;
  std::pair< double, double> sweights_range_;
};

} // namespace triage
} // namespace dooselection

#endif // TRIAGE_SWTUPLE_H
