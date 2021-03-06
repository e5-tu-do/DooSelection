#ifndef DOOSELECTION_REDUCER_SPLOTTERREDUCER_H
#define DOOSELECTION_REDUCER_SPLOTTERREDUCER_H

// from STL
#include <map>
#include <string>

// from ROOT

// from RooFit
#include "RooArgSet.h"

// from DooCore
#include "doocore/io/MsgStream.h"

// from project
#include "Reducer.h"

// forward declarations
namespace doofit { namespace fitter { namespace splot {
class SPlotFit2;
}}}
class RooDataSet;

/** @class dooselection::reducer::SPlotterReducer
 *  @brief Derived Reducer to perform an sPlot fit and write sweights into the output tuple
 *
 *  This is a Reducer derived from Reducer. It can be used to perform a fit 
 *  based on a given PDF, do an sPlot and extract sweights that are written 
 *  into the output tuple. doofit::fitter::splot::SPlotFit2 is used for sPlots.
 *
 *  @section splotred_usage Usage
 *
 *  Consider the following example:
 *
 *  @code
 *  doofit::fitter::splot::SPlotFit2 spf(my_pdfs, dummy_dataset);
 *  spf.set_num_cpu(8);
 *  spf.set_use_minos(true);
 *
 *  dooselection::reducer::SPlotterReducer spr(spf, my_observables_argset);
 *  spr.set_input_file_path(argv[1]);
 *  spr.set_input_tree_path(argv[2]);
 *  spr.set_output_file_path("test.root");
 *  spr.set_output_tree_path("Bd2JpsiKS");
 *  spr.set_cut_string("B0_BKGCAT==20");
 *  spr.Initialize();
 *  spr.Run();
 *  spr.Finalize();
 *  @endcode
 **/
namespace dooselection {
namespace reducer {
class SPlotterReducer : virtual public Reducer {
 public:
  /**
   *  @brief Constructor
   *
   *  Constructor based on configured SPlotFit2 object.
   *
   *  @param spf associated SPlotFit2 object
   *  @param observables observables to fit in
   */
  SPlotterReducer(doofit::fitter::splot::SPlotFit2& spf, RooArgSet observables);
  
  /**
   *  @brief Set components to plot for doofit::plotting::Plot
   *
   *  @param components_plot components to plot as regular expressions
   */
  void set_components_plot(const std::vector<std::string>& components_plot) {components_plot_ = components_plot;}
  
  /**
   *  @brief Set plot output directory
   *
   *  @param plot_directory plot output directory (default: PlotSPlotterReducer)
   */
  void set_plot_directory(const std::string& plot_directory) {plot_directory_ = plot_directory;}
  
  /**
   *  @brief Set external fitting arguments
   *
   *  @param ext_fit_args list of external fitting arguments
   */
  void set_fit_args(RooLinkedList* ext_fit_args = NULL) {ext_fit_args_ = ext_fit_args;}
  
 protected:
  virtual void ProcessInputTree();
  virtual void UpdateSpecialLeaves();
  virtual void CreateSpecialBranches();
  
 private:
  /**
   *  @brief Associated SPlotFit2 object
   */
  doofit::fitter::splot::SPlotFit2& splotfit_;
  
  /**
   *  @brief Observables to fit in
   */
  RooArgSet observables_;
  
  /**
   *  @brief Map of sweighted datasets
   */
  std::map<std::string,RooDataSet*> sweighted_datasets_;
  
  /**
   *  @brief Map of leaves for sweights
   */
  std::map<std::string,ReducerLeaf<Double_t>*> sweight_leaves_;
  
  /**
   *  @brief Components to plot for doofit::plotting::Plot
   */
  std::vector<std::string> components_plot_;
  
  /**
   *  @brief Plot output directory
   */
  std::string plot_directory_;
  
  /**
   *  @brief External fitting arguments
   */
  RooLinkedList* ext_fit_args_;
};
} // namespace reducer
} // namespace dooselection


#endif // DOOSELECTION_REDUCER_SPLOTTERREDUCER_H
