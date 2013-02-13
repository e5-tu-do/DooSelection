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
 *  ...
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
  
 protected:
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
};
} // namespace reducer
} // namespace dooselection


#endif // DOOSELECTION_REDUCER_SPLOTTERREDUCER_H
