#include "SimSPlotReducer.h"

// from Boost
//#include <boost/assign/std/vector.hpp>

// from RooFit
#include "RooDataSet.h"

// from DooCore
#include <doocore/io/MsgStream.h>

//using namespace boost::assign;
using namespace doocore::io;

namespace dooselection {
namespace reducer {

SimSPlotReducer::SimSPlotReducer(RooDataSet data_bkg_sw, RooDataSet data_sig_sw) :
  data_bkg_sw_(data_bkg_sw),
  data_sig_sw_(data_sig_sw)
{
  set_old_style_interim_tree(true);
}
  
void SimSPlotReducer::ProcessInputTree() {
//  swarn << "SimSPlotReducer::ProcessInputTree(): cut string: " << cut_string() << endmsg;
}

  
void SimSPlotReducer::CreateSpecialBranches() {
    
  int argc = 1;
  char* argv[1];
  argv[0] = "";

  interim_tree_->SetBranchStatus("*", 1);
    
  if (data_bkg_sw_.numEntries() != interim_tree_->GetEntries()) {
    doocore::io::serr << "Number of entries in interim tree and sweighted background dataset mismatch!" << doocore::io::endmsg;
    doocore::io::serr << "Background dataset contains:    " << data_bkg_sw_.numEntries() << " entries." << doocore::io::endmsg;;
    doocore::io::serr << "Tree contains:              " << interim_tree_->GetEntries() << " entries." << doocore::io::endmsg;;
  }
  
  if (data_sig_sw_.numEntries() != interim_tree_->GetEntries()) {
    doocore::io::serr << "Number of entries in interim tree and sweighted signal dataset mismatch!" << doocore::io::endmsg;
    doocore::io::serr << "Signal dataset contains:    " << data_sig_sw_.numEntries() << " entries." << doocore::io::endmsg;;
    doocore::io::serr << "Tree contains:              " << interim_tree_->GetEntries() << " entries." << doocore::io::endmsg;;
  }
  
  doocore::io::sinfo << "SimSPlotReducer::CreateSpecialBranches(): Creating leaf for sweight " << "BkgYield_sw" << doocore::io::endmsg;
  sweight_leaves_["Bkg"] = &CreateDoubleLeaf("BkgYield_sw", -1000);
  doocore::io::sinfo << "SimSPlotReducer::CreateSpecialBranches(): Creating leaf for sweight " << "SigYield_sw" << doocore::io::endmsg;
  sweight_leaves_["Sig"] = &CreateDoubleLeaf("SigYield_sw", -1000);
}

void SimSPlotReducer::UpdateSpecialLeaves() {
  const RooArgSet* sweighted_values_bkg = data_bkg_sw_.get(selected_entry_);
  *sweight_leaves_["Bkg"] = data_bkg_sw_.weight();
  const RooArgSet* sweighted_values_sig = data_sig_sw_.get(selected_entry_);
  *sweight_leaves_["Sig"] = data_sig_sw_.weight();
}
} // namespace reducer
} // namespace dooselection
