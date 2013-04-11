#include "SPlotterReducer.h"

// from RooFit
#include "RooDataSet.h"

// from DooCore
#include <doocore/io/EasyTuple.h>

// from DooFit
#include <doofit/fitter/splot/SPlotFit2.h>

namespace dooselection {
namespace reducer {

SPlotterReducer::SPlotterReducer(doofit::fitter::splot::SPlotFit2& spf, RooArgSet observables) :
  splotfit_(spf),
  observables_(observables)
{
}
  
void SPlotterReducer::CreateSpecialBranches() {
  doocore::io::EasyTuple etuple(interim_tree_, observables_);
  RooDataSet& data = etuple.ConvertToDataSet();
  
  splotfit_.set_input_data(&data);
  splotfit_.Fit();
  
  interim_tree_->SetBranchStatus("*", 1);
  
  // get sweight maps from SPlotFit2
  sweighted_datasets_ = splotfit_.GetSwDataSets();
  
  if (data.numEntries() != interim_tree_->GetEntries()) {
    doocore::io::serr << "Number of entries in interim tree and sweighted datasets mismatch!" << doocore::io::endmsg;
    doocore::io::serr << "RooFit dataset contains:    " << data.numEntries() << " entries." << doocore::io::endmsg;;
    doocore::io::serr << "Tree contains:              " << interim_tree_->GetEntries() << " entries." << doocore::io::endmsg;;
    
  }

  for (std::map<std::string,RooDataSet*>::const_iterator it = sweighted_datasets_.begin();
       it != sweighted_datasets_.end(); ++it) {
    sweight_leaves_[(*it).first] = &CreateDoubleLeaf((*it).first+"_sw", -1000);
  }
}

void SPlotterReducer::UpdateSpecialLeaves() {
  for (std::map<std::string,RooDataSet*>::const_iterator it = sweighted_datasets_.begin();
       it != sweighted_datasets_.end(); ++it) {
    const RooArgSet* sweighted_values = (*it).second->get(selected_entry_);
    *sweight_leaves_[(*it).first] = (*it).second->weight();
  }
}
} // namespace reducer
} // namespace dooselection
