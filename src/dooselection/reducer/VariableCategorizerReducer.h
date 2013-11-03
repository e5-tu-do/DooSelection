#ifndef DOOSELECTION_REDUCER_VARIABLECATEGORIZERREDUCER_H
#define DOOSELECTION_REDUCER_VARIABLECATEGORIZERREDUCER_H

// from STL

// from STL11
#include <tuple>

// from ROOT

// from DooCore

// from BOOST

// from project
#include "Reducer.h"
#include "ReducerLeaf.h"

/** @class dooselection::reducer::VariableCategorizerReducer
 *  @brief Derived Reducer to write new variable categorizing a given variable into N bins with an equal number of entries.
 *
 *  This is a Reducer derived from Reducer. It writes a new variable categorizing a given
 *  variable into N bins with an equal number of entries. To do so, it first calculates the N-quantiles 
 *  for the given variable in the provided variable range. Then it sorts every event into one of the
 *  quantiles. 
 *
 *  @section varcatred_usage Usage
 *
 *  Consider the following example:
 *
 *  @code
 *  VariableCategorizerReducer AReducer("catVar");    // Define the variable prefix
 *  AReducer.set_variable("obsTime", 20, 0.3, 18.3);  // Define variable to be categorized
 *  AReducer.set_variable("obsTime", 10, 0.3, 18.3);  // Multiple entries are possible
 *  AReducer.set_variable("obsEtaAll", 8, 0.0, 0.5);  // Name will be prefix+nbins+"_"+variable_name
 *  ...
 *  @endcode
 **/
namespace dooselection {
namespace reducer {

class VariableCategorizerReducer : virtual public Reducer {
 public:
  VariableCategorizerReducer(const std::string& prefix_name="catVar");
  virtual ~VariableCategorizerReducer(){}

  void set_variable(std::string variable_name, int nbins, double range_min, double range_max);

 protected:
  virtual void CreateSpecialBranches();
  virtual void PrepareSpecialBranches();
  virtual bool EntryPassesSpecialCuts();
  virtual void UpdateSpecialLeaves();
 private:
  /// name of variable prefix
  std::string prefix_name_;

  /// vector containing a tuple with the following entries:
  /// 0 variable_name 
  /// 1 nbins 
  /// 2 range min 
  /// 3 range max 
  /// 4 vector of quantiles 
  /// 5 vector of data points 
  /// 6 int pointer to variable category 
  /// 7 double pointer to variable value 
  /// 8 reducer leaf pointer to variable category leaf
  std::vector< std::tuple< std::string, int, double, double, std::vector<double>, std::vector<double>, int*, Float_t*, dooselection::reducer::ReducerLeaf<Int_t>* > > variables_;
};

} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_VARIABLECATEGORIZERREDUCER_H