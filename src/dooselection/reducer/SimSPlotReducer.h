#ifndef DOOSELECTION_REDUCER_SIMSPLOTREDUCER_H
#define DOOSELECTION_REDUCER_SIMSPLOTREDUCER_H

// from STL
#include <map>
#include <string>

// from ROOT

// from RooFit
#include "RooDataSet.h"

// from DooCore
#include "doocore/io/MsgStream.h"

// from project
#include "Reducer.h"

// forward declarations
class RooDataSet;

/** @class dooselection::reducer::SimSPlotReducer
 *  @brief Derived Reducer to write sweights into the output tuple
 *
 *  This is a Reducer derived from Reducer. It can be used to write sweights 
 *  into the output tuple.
 *
 *  @section splotred_usage Usage
 *
 *  Consider the following example:
 *
 *  @code
 *
 *  dooselection::reducer::SimSPlotReducer simsplotreducer(data_bkg, data_sig);
 *  simsplotreducer.set_input_file_path(argv[1]);
 *  simsplotreducer.set_input_tree_path(argv[2]);
 *  simsplotreducer.set_output_file_path("test.root");
 *  simsplotreducer.set_output_tree_path("Bd2JpsiKS");
 *  simsplotreducer.set_cut_string("B0_BKGCAT==20");
 *  simsplotreducer.Initialize();
 *  simsplotreducer.Run();
 *  simsplotreducer.Finalize();
 *  @endcode
 **/
namespace dooselection {
namespace reducer {
class SimSPlotReducer : virtual public Reducer {
 public:
  /**
   *  @brief Constructor
   *
   *  Constructor based on datasets with sweights calculated before.
   *
   *  @param data_bkg_sw RooDataSet with background weights
   *  @param data_sig_sw RooDataSet with signal weights
   */
  SimSPlotReducer(RooDataSet data_bkg_sw, RooDataSet data_sig_sw);

 protected:
  virtual void ProcessInputTree();
  virtual void UpdateSpecialLeaves();
  virtual void CreateSpecialBranches();
  
 private:
  /**
   *  @brief RooDataSet with background weights
   */
  RooDataSet data_bkg_sw_;
  
  /**
   *  @brief RooDataSet with signal weights
   */
  RooDataSet data_sig_sw_;
  
  /**
   *  @brief Map of leaves for sweights
   */
  std::map<std::string,ReducerLeaf<Double_t>*> sweight_leaves_;
  
};
} // namespace reducer
} // namespace dooselection


#endif // DOOSELECTION_REDUCER_SIMSPLOTREDUCER_H
