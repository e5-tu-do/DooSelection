#ifndef TRIAGE_TUPLE_H
#define TRIAGE_TUPLE_H

// from STL

// from ROOT

// from RooFit
#include "RooDataSet.h"
#include "RooArgSet.h"

// from TMVA

// from Boost
#ifdef __GNUG__
#define BOOST_NO_CXX11_SCOPED_ENUMS
#endif
#include <boost/filesystem.hpp>

// from DooCore
#include "doocore/io/EasyTuple.h"

// from here

// forward declarations

namespace dooselection{
namespace triage{

class Tuple{
 public:
  Tuple(const std::string& name, const std::string& path, const std::string& filename, const std::string& treename, const RooArgSet& argset, const std::string& observable_name, double observable_range_min, double observable_range_max):
    name_(name),
    path_(path),
    filename_(filename),
    treename_(treename),
    observable_name_(observable_name),
    observable_range_(std::make_pair(observable_range_min,observable_range_max)),
    max_n_sig_events_(0.),
    max_n_bkg_events_(0.),
    is_mc_(false),
    is_sw_(false),
    is_ft_(false),
    is_absfitter_(false),
    dataset_exists_(false)
  {
    boost::filesystem::path filename_full(boost::filesystem::path(path) / boost::filesystem::path(filename));
    etuple_ = new doocore::io::EasyTuple(filename_full.string(), treename, argset);
  }

  virtual ~Tuple(){}

  // setter
  void set_max_n_sig_events(double max_n_sig_events){max_n_sig_events_ = max_n_sig_events;}
  void set_max_n_bkg_events(double max_n_bkg_events){max_n_bkg_events_ = max_n_bkg_events;}

  // getter
  const std::string& name() const{return name_;}
  const std::string& path() const{return path_;}
  const std::string& filename() const{return filename_;}
  const std::string& treename() const{return treename_;}
  const std::string& observable_name() const{return observable_name_;}
  std::pair<double, double> observable_range() const{return observable_range_;}
  double max_n_sig_events() const{return max_n_sig_events_;}
  double max_n_bkg_events() const{return max_n_bkg_events_;}
  
  doocore::io::EasyTuple& etuple(){return *etuple_;}
  TTree& tree() {return etuple_->tree();}
  const TTree& tree() const{return etuple_->tree();}
  RooDataSet& dataset(){
    if (dataset_exists_){
      return (etuple_->dataset());
    }
    else{
      if (global_cut_.size() > 0) {
        dataset_ = &(etuple_->ConvertToDataSet(RooFit::Cut(global_cut_.c_str())));
      } else {
        dataset_ = &(etuple_->ConvertToDataSet());
      }
      dataset_exists_=true;
      return (etuple_->dataset());
    }
  }

  bool is_mc() const{return is_mc_;};
  bool is_sw() const{return is_sw_;};
  bool is_ft() const{return is_ft_;};
  bool is_absfitter() const{return is_absfitter_;};
  
  void set_global_cut(std::string global_cut) { global_cut_ = global_cut; }
  
 protected:
  bool is_mc_;
  bool is_sw_;
  bool is_ft_;
  bool is_absfitter_;

 private:
  std::string name_;
  std::string path_;
  std::string filename_;
  std::string treename_;
  std::string observable_name_;
  
  std::string global_cut_;

  std::pair<double, double> observable_range_;

  double max_n_sig_events_;
  double max_n_bkg_events_;

  doocore::io::EasyTuple* etuple_;
  RooDataSet* dataset_;
  bool dataset_exists_;
};

} // namespace triage
} // namespace dooselection

#endif // TRIAGE_TUPLE_H
