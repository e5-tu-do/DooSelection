#ifndef DOOSELECTION_REDUCER_VARIABLECATEGORIZERREDUCER_H
#define DOOSELECTION_REDUCER_VARIABLECATEGORIZERREDUCER_H

// from STL

// from ROOT

// from DooCore

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
 *  VariableCategorizerReducer AReducer("obsTime", "catVar", 20, 0.3, 18.3);
 *  ...
 *  @endcode
 **/
namespace dooselection {
namespace reducer {

class VariableCategorizerReducer : virtual public Reducer {
 public:
  VariableCategorizerReducer(const std::string& variable_name, const std::string& prefix_name, int nbins, double range_min, double range_max);
  virtual ~VariableCategorizerReducer(){}

  void set_nbins(int nbins){nbins_ = nbins;}
  void set_variable_name(const std::string& variable_name) {variable_name_ = variable_name;}
  void set_prefix_name(const std::string& prefix_name) {prefix_name_ = prefix_name;}
  void set_range_min(double range_min){range_min_ = range_min;}
  void set_range_max(double range_max){range_max_ = range_max;}

 protected:
  virtual void CreateSpecialBranches();
  virtual void PrepareSpecialBranches();
  virtual bool EntryPassesSpecialCuts();
  virtual void UpdateSpecialLeaves();
 private:
  int nbins_;
  int* variable_category_;

  double range_min_;
  double range_max_;

  Double_t* variable_;

  std::string variable_name_;
  std::string prefix_name_;
  
  std::vector<double> data_points_;
  std::vector<double> quantiles_;
  
  dooselection::reducer::ReducerLeaf<Int_t>* variable_category_leaf_;
};

} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_VARIABLECATEGORIZERREDUCER_H