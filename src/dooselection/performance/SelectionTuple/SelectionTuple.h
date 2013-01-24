#ifndef DOOSELECTION_PERFORMANCE_SELECTIONTUPLE_H
#define DOOSELECTION_PERFORMANCE_SELECTIONTUPLE_H

// from STL
#include <vector>

// from ROOT

// from RooFit

// from TMVA

// from BOOST

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/io/EasyTuple.h"

// from DooFit
#include "doofit/builder/EasyPdf/EasyPdf.h"

// from here

// forward declarations
class RooDataSet;

namespace selection{
  class SelectionTuple{
   public:
    SelectionTuple(std::string name, std::string title, doocore::io::EasyTuple* etuple, std::string sig_cut, std::string bkg_cut, bool debug_mode=false):
   	debug_mode_(debug_mode),
   	dataset_exists_(false),
   	name_(name),
    title_(title),
    etuple_(etuple),
    epdf_(NULL),
    dataset_(NULL),
    use_mc_(true),
    sig_cut_(sig_cut),
    bkg_cut_(bkg_cut),
    use_sweights_(false),
    sig_sweight_(""),
    bkg_sweight_(""),
    sweight_range_min_(-1),
    sweight_range_max_(1),
    use_fit_(false),
    massfit_range_min_(-1),
    massfit_range_max_(1),
    components_()
    {
    	if (debug_mode_) doocore::io::serr << "-debug- " << "Created new SelectionTuple: '" << title_ << "'..." << doocore::io::endmsg;
    }

    SelectionTuple(std::string name, std::string title, doocore::io::EasyTuple* etuple, std::string sig_sweight, std::string bkg_sweight, double sweight_range_min, double sweight_range_max, bool debug_mode=false):
   	debug_mode_(debug_mode),
   	dataset_exists_(false),
   	name_(name),
    title_(title),
    etuple_(etuple),
    epdf_(NULL),
    dataset_(NULL),
    use_mc_(false),
    sig_cut_(""),
    bkg_cut_(""),
    use_sweights_(true),
    sig_sweight_(sig_sweight),
    bkg_sweight_(bkg_sweight),
    sweight_range_min_(sweight_range_min),
    sweight_range_max_(sweight_range_max),
    use_fit_(false),
    massfit_range_min_(-1),
    massfit_range_max_(1),
    components_()
    {
    	if (debug_mode_) doocore::io::serr << "-debug- " << "Created new SelectionTuple: '" << title_ << "'..." << doocore::io::endmsg;
    }

    SelectionTuple(std::string name, std::string title, doocore::io::EasyTuple* etuple, doofit::builder::EasyPdf* epdf, double fit_range_min, double fit_range_max, std::vector<std::string> components, bool debug_mode=false):
   	debug_mode_(debug_mode),
   	dataset_exists_(false),
   	name_(name),
    title_(title),
    etuple_(etuple),
    epdf_(epdf),
    dataset_(NULL),
    use_mc_(false),
    sig_cut_(""),
    bkg_cut_(""),
    use_sweights_(false),
    sig_sweight_(""),
    bkg_sweight_(""),
    sweight_range_min_(-1),
    sweight_range_max_(1),
    use_fit_(true),
    massfit_range_min_(fit_range_min),
    massfit_range_max_(fit_range_max),
    components_(components)
    {
    	if (debug_mode_) doocore::io::serr << "-debug- " << "Created new SelectionTuple: '" << title_ << "'..." << doocore::io::endmsg;
    }
    
    ~SelectionTuple(){};

    // SETTER
    void set_debug_mode(bool debug_mode){
    	debug_mode_=debug_mode;
    }
    void set_mc_truth_cuts(std::string sig_cut, std::string bkg_cut){
    	sig_cut_=sig_cut;
    	bkg_cut_=bkg_cut;
    	use_mc_=true;
    	use_sweights_=false;
    	use_fit_=false;
    }
    void set_sweights(std::string sig_sweight, std::string bkg_sweight, double sweight_range_min, double sweight_range_max){
      sig_sweight_=sig_sweight;
      bkg_sweight_=bkg_sweight;
      sweight_range_min_=sweight_range_min;
      sweight_range_max_=sweight_range_max;
      use_mc_ = false;
      use_sweights_ = true;
      use_fit_ = false;
    }
    void set_fit_parameter(doocore::builder::EasyPdf* epdf, double fit_range_min, double fit_range_max, std::vector<std::string> components){
    	epdf_=epdf;
    	fit_range_min_=fit_range_min;
    	fit_range_max_=fit_range_max;
    	components_=components;
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
    bool debug_mode(){
    	return debug_mode_;
    }
    std::string name(){
    	return name_;
    }
    std::string title(){
      return title_;
    }
    doocore::io::EasyTuple* etuple(){
    	return etuple_;
    }
    doocore::builder::EasyPdf* epdf(){
    	return epdf_;
    }
    RooDataSet* dataset(){
      if (dataset_exists_){
        return &(etuple_->dataset());
      }
      else{
        dataset_ = &(etuple_->ConvertToDataSet());
        dataset_exists_=true;
        return &(etuple_->dataset());
      }
    }
    bool use_mc(){
      return use_mc_;
    }
    std::string sig_cut(){
      if (use_mc_){
        return sig_cut_;
      }
      else{
        doocore::io::serr << "-SelectionTuple- no cuts defined!" << doocore::io::endmsg;
        return 0;
      }
    }    
    std::string bkg_cut(){
      if (use_mc_){
        return bkg_cut_;
      }   
      else{
        doocore::io::serr << "-SelectionTuple- no cuts defined!" << doocore::io::endmsg;
        return 0;
      }    
    }
    bool use_sweights(){
      return use_sweights_;
    }
    std::string sig_sweight(){
      if (use_sweights_){
        return sig_sweight_;
      }
      else{
        doocore::io::serr << "-SelectionTuple- no sweights defined!" << doocore::io::endmsg;
        return 0;
      }
    }
    std::string bkg_sweight(){
      if (use_sweights_){
        return bkg_sweight_;
      }
      else{
        doocore::io::serr << "-SelectionTuple- no sweights defined!" << doocore::io::endmsg;
        return 0;
      }
    }
    double sweight_min(){
      return sweight_range_min_;
    }
    double sweight_max(){
      return sweight_range_max_;
    }
    bool use_fit(){
      return use_fit_;
    }
    double fit_range_min(){
      return fit_range_min_;
    }
    double fit_range_max(){
      return fit_range_max_;
    }
    std::vector<std::string> components(){
    	return components_;
    }

   private:
   	// MEMBER VARIABLES
   	bool debug_mode_;
   	bool dataset_exists_;

   	std::string name_;
    std::string title_;

    doocore::io::EasyTuple* etuple_;
    doofit::builder::EasyPdf* epdf_;
    RooDataSet* dataset_;

    // using mc truth information
    bool use_mc_;

    std::string sig_cut_;
    std::string bkg_cut_;

    // using sweights
    bool use_sweights_;

    std::string sig_sweight_;
    std::string bkg_sweight_;

    double sweight_range_min_;
    double sweight_range_max_;
    
    // using mass fit
    bool use_fit_;

    double fit_range_min_;
    double fit_range_max_;

    std::vector<std::string> components_;
  };
} // namespace selection

#endif // DOOSELECTION_PERFORMANCE_SELECTIONTUPLE_H