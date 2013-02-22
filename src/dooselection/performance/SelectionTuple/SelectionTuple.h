#ifndef DOOSELECTION_PERFORMANCE_SELECTIONTUPLE_SELECTIONTUPLE_H
#define DOOSELECTION_PERFORMANCE_SELECTIONTUPLE_SELECTIONTUPLE_H

// from STL
#include <vector>
#include <map>

// from ROOT
#include "TTree.h"

// from RooFit
#include "RooDataSet.h"

// from TMVA

// from BOOST

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/io/EasyTuple.h"

// from DooFit
#include "doofit/builder/EasyPdf/EasyPdf.h"

// from here

// forward declarations

namespace dooselection{
namespace performance{
  class SelectionTuple{
   public:
    SelectionTuple(std::string name, std::string title, doocore::io::EasyTuple* etuple, std::map<std::string, std::string> map_of_components_and_cuts, bool debug_mode=false):
    debug_mode_(debug_mode),
    use_tagged_events_only_(false),
    name_(name),
    title_(title),
    etuple_(etuple),
    dataset_(),
    dataset_exists_(false),
    use_mc_(true),
    map_of_components_and_cuts_(map_of_components_and_cuts),
    use_sweights_(false),
    map_of_components_and_sweights_(),
    sweight_range_min_(-1),
    sweight_range_max_(1),
    use_fit_(false),
    observable_name_(),
    map_of_components_and_pdfs_(),
    map_of_components_and_yields_(),
    epdf_(),
    fit_range_min_(-1),
    fit_range_max_(1)
    {
    	if (debug_mode_) doocore::io::serr << "-debug- " << "Created new SelectionTuple: '" << title_ << "'..." << doocore::io::endmsg;
    }

    SelectionTuple(std::string name, std::string title, doocore::io::EasyTuple* etuple, std::map<std::string, std::string> map_of_components_and_sweights, double sweight_range_min, double sweight_range_max, bool debug_mode=false):
    debug_mode_(debug_mode),
    use_tagged_events_only_(false),
    name_(name),
    title_(title),
    etuple_(etuple),
    dataset_(),
    dataset_exists_(false),
    use_mc_(false),
    map_of_components_and_cuts_(),
    use_sweights_(true),
    map_of_components_and_sweights_(map_of_components_and_sweights),
    sweight_range_min_(sweight_range_min),
    sweight_range_max_(sweight_range_max),
    use_fit_(false),
    observable_name_(),
    map_of_components_and_pdfs_(),
    map_of_components_and_yields_(),
    epdf_(),
    fit_range_min_(-1),
    fit_range_max_(1)
    {
    	if (debug_mode_) doocore::io::serr << "-debug- " << "Created new SelectionTuple: '" << title_ << "'..." << doocore::io::endmsg;
    }

    SelectionTuple(std::string name, std::string title, doocore::io::EasyTuple* etuple, doofit::builder::EasyPdf* epdf, std::string observable_name, std::map<std::string, std::string> map_of_components_and_pdfs, std::map<std::string, std::string> map_of_components_and_yields, double fit_range_min, double fit_range_max, bool debug_mode=false):
    debug_mode_(debug_mode),
    use_tagged_events_only_(false),
    name_(name),
    title_(title),
    etuple_(etuple),
    dataset_(),
    dataset_exists_(false),
    use_mc_(false),
    map_of_components_and_cuts_(),
    use_sweights_(false),
    map_of_components_and_sweights_(),
    sweight_range_min_(-1),
    sweight_range_max_(1),
    use_fit_(true),
    observable_name_(observable_name),
    map_of_components_and_pdfs_(map_of_components_and_pdfs),
    map_of_components_and_yields_(map_of_components_and_yields),
    epdf_(epdf),
    fit_range_min_(fit_range_min),
    fit_range_max_(fit_range_max)
    {
      if (debug_mode_) doocore::io::serr << "-debug- " << "Created new SelectionTuple: '" << title_ << "'..." << doocore::io::endmsg;
    }
    
    ~SelectionTuple(){};

    // SETTER
    void set_debug_mode(bool debug_mode){
    	debug_mode_=debug_mode;
    }
    void set_use_tagged_events_only(bool use_tagged_events_only){
        use_tagged_events_only_=use_tagged_events_only;
    }
    void set_mc_truth_cuts(std::map<std::string, std::string> map_of_components_and_cuts){
    	map_of_components_and_cuts_=map_of_components_and_cuts;
    	use_mc_=true;
    	use_sweights_=false;
    	use_fit_=false;
    }
    void set_sweights(std::map<std::string, std::string> map_of_components_and_sweights, double sweight_range_min, double sweight_range_max){
      map_of_components_and_sweights_=map_of_components_and_sweights;
      sweight_range_min_=sweight_range_min;
      sweight_range_max_=sweight_range_max;
      use_mc_ = false;
      use_sweights_ = true;
      use_fit_ = false;
    }
    void set_fit_parameter(doofit::builder::EasyPdf* epdf, std::string observable_name, std::map<std::string, std::string> map_of_components_and_pdfs, std::map<std::string, std::string> map_of_components_and_yields, double fit_range_min, double fit_range_max){
    	epdf_=epdf;
      observable_name_=observable_name;
    	map_of_components_and_pdfs_=map_of_components_and_pdfs;
      map_of_components_and_yields_=map_of_components_and_yields;
    	fit_range_min_=fit_range_min;
    	fit_range_max_=fit_range_max;
    	use_mc_ = false;
      use_sweights_ = false;
      use_fit_ = true;
    }
    void set_discriminator(std::string discriminator){
      if (discriminator == "sweights"){
        use_sweights_ = true;
        use_mc_ = false;
        use_fit_ = false;
      }
      else if (discriminator == "mc"){
        use_sweights_ = false;
        use_mc_ = true;
        use_fit_ = false;
      }
      else if (discriminator == "fit"){
        use_sweights_ = false;
        use_mc_ = false;
        use_fit_ = true;
      }
      else{
        doocore::io::serr << "-SelectionTuple- please define if you want to use 'sweights', 'mc' truth information or a 'fit' to discriminate between signal and background!" << doocore::io::endmsg;
      }
    }

    // GETTER
    const bool debug_mode(){
    	return debug_mode_;
    }
    const bool use_tagged_events_only(){
        return use_tagged_events_only_;
    }
    const std::string name(){
    	return name_;
    }
    const std::string title(){
      return title_;
    }
    doocore::io::EasyTuple& etuple(){
    	return *etuple_;
    }
    TTree& tree(){
      return etuple_->tree();
    }
    RooDataSet& dataset(){
      if (dataset_exists_){
        return (etuple_->dataset());
      }
      else{
        dataset_ = &(etuple_->ConvertToDataSet());
        dataset_exists_=true;
        return (etuple_->dataset());
      }
    }
    const bool use_mc(){
      return use_mc_;
    }
    const std::map<std::string, std::string>& map_of_components_and_cuts(){
    	return map_of_components_and_cuts_;
    }
    const bool use_sweights(){
      return use_sweights_;
    }
    const std::map<std::string, std::string>& map_of_components_and_sweights(){
    	return map_of_components_and_sweights_;
    }
    const double sweight_min(){
      return sweight_range_min_;
    }
    const double sweight_max(){
      return sweight_range_max_;
    }
    const bool use_fit(){
      return use_fit_;
    }
    const std::string observable_name(){
      return observable_name_;
    }
    const std::map<std::string, std::string>& map_of_components_and_pdfs() const{
      return map_of_components_and_pdfs_;
    }
    const std::map<std::string, std::string>& map_of_components_and_yields() const{
    	return map_of_components_and_yields_;
    }
    doofit::builder::EasyPdf& epdf(){
    	return *epdf_;
    }
    const double fit_range_min(){
      return fit_range_min_;
    }
    const double fit_range_max(){
      return fit_range_max_;
    }

   private:
    SelectionTuple(const SelectionTuple&){};
    SelectionTuple& operator=  (const SelectionTuple&){};

    // MEMBER VARIABLES
    bool debug_mode_;
    bool use_tagged_events_only_;

    std::string name_;
    std::string title_;

    doocore::io::EasyTuple* etuple_;
    RooDataSet* dataset_;
    bool dataset_exists_;

    // using mc truth information
    bool use_mc_;

    std::map<std::string, std::string> map_of_components_and_cuts_;

    // using sweights
    bool use_sweights_;

    std::map<std::string, std::string> map_of_components_and_sweights_;
    double sweight_range_min_;
    double sweight_range_max_;
    
    // using mass fit
    bool use_fit_;

    std::string observable_name_;
    std::map<std::string, std::string> map_of_components_and_pdfs_;
    std::map<std::string, std::string> map_of_components_and_yields_;
    doofit::builder::EasyPdf* epdf_;
    double fit_range_min_;
    double fit_range_max_;
  };
} // namespace performance
} // namespace dooselection

#endif // DOOSELECTION_PERFORMANCE_SELECTIONTUPLE_SELECTIONTUPLE_H
