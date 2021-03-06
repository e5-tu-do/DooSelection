#include "SPlotterReducer.h"

// from Boost
#include <boost/assign/std/vector.hpp>

// from RooFit
#include "RooDataSet.h"
#include "RooAbsPdf.h"
#include "RooSimultaneous.h"

// from DooCore
#include <doocore/io/EasyTuple.h>
#include <doocore/io/MsgStream.h>

// from DooFit
#include <doofit/fitter/splot/SPlotFit2.h>
#include <doofit/plotting/Plot/Plot.h>
#include <doofit/plotting/Plot/PlotSimultaneous.h>
#include <doofit/plotting/Plot/PlotConfig.h>

using namespace boost::assign;
using namespace doocore::io;
using namespace doofit::plotting;

namespace dooselection {
namespace reducer {

SPlotterReducer::SPlotterReducer(doofit::fitter::splot::SPlotFit2& spf, RooArgSet observables) :
  splotfit_(spf),
  observables_(observables),
  plot_directory_("PlotSPlotterReducer"),
  ext_fit_args_(NULL)
{
  set_old_style_interim_tree(true);
}
  
void SPlotterReducer::ProcessInputTree() {
//  swarn << "SPlotterReducer::ProcessInputTree(): cut string: " << cut_string() << endmsg;
}

  
void SPlotterReducer::CreateSpecialBranches() {
  doocore::io::EasyTuple etuple(interim_tree_, observables_);
  RooDataSet& data = etuple.ConvertToDataSet();
    
  splotfit_.set_input_data(&data);
  splotfit_.Fit(ext_fit_args_);
  
//  int argc = 1;
//  
//  char ** argv;
//  argv = new char[1];
//  argv[0] = new char[10];
//  argv[0] = "";
  
//  char* argv[1];
//  strcpy( argv[0], "" );
//  argv[0] = "";
  
  PlotConfig cfg_plot("cfg_plot");
  cfg_plot.InitializeOptions();
  cfg_plot.set_plot_directory(plot_directory_);
  
  bool sim_pdf = dynamic_cast<const RooSimultaneous*>(&splotfit_.pdf()) != NULL;
  
  TIterator* it_observables = observables_.createIterator();
  TObject* object = NULL;
  while ((object = it_observables->Next())) {
    RooRealVar* observable = dynamic_cast<RooRealVar*>(object);
    if (observable != NULL && splotfit_.pdf().dependsOn(*observable)) {
      if (sim_pdf) {
        PlotSimultaneous myplot(cfg_plot, *observable, data, dynamic_cast<const RooSimultaneous&>(splotfit_.pdf()), components_plot_);
        myplot.PlotIt();
      } else {
        Plot myplot(cfg_plot, *observable, data, splotfit_.pdf(), components_plot_);
        myplot.PlotIt();
      }
    }
  }

  interim_tree_->SetBranchStatus("*", 1);
  
  // get sweight maps from SPlotFit2
  sweighted_datasets_ = splotfit_.GetSwDataSets();
  
  if (data.numEntries() != interim_tree_->GetEntries()) {
    doocore::io::serr << "Number of entries in interim tree and sweighted datasets mismatch!" << doocore::io::endmsg;
    doocore::io::serr << "RooFit dataset contains:    " << data.numEntries() << " entries." << doocore::io::endmsg;;
    doocore::io::serr << "Tree contains:              " << interim_tree_->GetEntries() << " entries." << doocore::io::endmsg;;
    
    throw 314;
  }

  for (std::map<std::string,RooDataSet*>::const_iterator it = sweighted_datasets_.begin();
       it != sweighted_datasets_.end(); ++it) {
    doocore::io::sinfo << "SPlotterReducer::CreateSpecialBranches(): Creating leaf for sweights " << (*it).first+"_sw" << doocore::io::endmsg;
    sweight_leaves_[(*it).first] = &CreateDoubleLeaf((*it).first+"_sw", -1000);
  }
}

void SPlotterReducer::UpdateSpecialLeaves() {
  for (std::map<std::string,RooDataSet*>::const_iterator it = sweighted_datasets_.begin(), end = sweighted_datasets_.end();
       it != end; ++it) {
    (*it).second->get(selected_entry_);
    *sweight_leaves_[(*it).first] = (*it).second->weight();
  }
}
} // namespace reducer
} // namespace dooselection
