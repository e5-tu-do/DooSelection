#ifndef TRIAGE_CLASSIFIER_H
#define TRIAGE_CLASSIFIER_H

// from STL

// from ROOT
#include "TH1D.h"
#include "TFile.h"
// from RooFit

// from TMVA

// from BOOST
#include "boost/lexical_cast.hpp"
// from DooCore
#include "doocore/io/MsgStream.h"
// from here

// forward declarations

namespace dooselection{
namespace triage{

class Classifier{
 public:
  Classifier(const std::string& name, const std::string& expression):
    name_(name),
    expression_(expression),
    range_(std::make_pair(0.,0.)),
    cut_operator_(""),
    best_cut_value_(-99999.){}

  Classifier(const std::string& name, const std::string& expression, const std::string& cut_operator, double best_cut_value):
    name_(name),
    expression_(expression),
    range_(std::make_pair(0.,0.)),
    cut_operator_(cut_operator),
    best_cut_value_(best_cut_value){}

  Classifier(const std::string& name, const std::string& expression, const double range_min, const double range_max):
    name_(name),
    expression_(expression),
    range_(std::make_pair(range_min,range_max)),
    cut_operator_(""),
    best_cut_value_(-99999.){}

  Classifier(const std::string& name, const std::string& expression, const double range_min, const double range_max, std::string cut_operator):
    name_(name),
    expression_(expression),
    range_(std::make_pair(range_min,range_max)),
    cut_operator_(cut_operator),
    best_cut_value_(-99999.){}

  Classifier(const std::string& name, const std::string& expression, const double range_min, const double range_max, std::string cut_operator, double best_cut_value):
    name_(name),
    expression_(expression),
    range_(std::make_pair(range_min,range_max)),
    cut_operator_(cut_operator),
    best_cut_value_(best_cut_value){}

  // setter
  void set_range(const double min, const double max){range_.first = min; range_.second = max;}
  void set_range(const std::pair< double, double> range){range_ = range;}
  void set_operator(const std::string& cut_operator){cut_operator_ = cut_operator;}
  void set_best_cut_value(const double value){best_cut_value_ = value;}
  void set_cut_values(std::vector<double> cut_values){cut_values_ = cut_values;}

  // getter
  std::string name() const{return name_;}
  std::string expression() const{return expression_;}
  std::pair< double, double> range() const{return range_;}
  std::string cut_operator() const{return cut_operator_;}
  double best_cut_value() const{return best_cut_value_;}
  std::vector<double> cut_values() const{return cut_values_;}
  std::string best_cut_string() const{
    std::string best_cut_string;
    if (cut_operator_=="" || best_cut_value_==-99999.){
      best_cut_string=  expression_;
    }
    else{
       best_cut_string = expression_ + cut_operator_ + std::to_string(best_cut_value_);
    }
    return best_cut_string;
  }

  // print
  void Print() const{
    doocore::io::sinfo << name_ << doocore::io::endmsg;
    doocore::io::sinfo.increment_indent(2);
    doocore::io::sinfo << "Expression: " << expression_ << doocore::io::endmsg;
    doocore::io::sinfo << "Best cut string:" << expression_ << cut_operator_ << best_cut_value_ << doocore::io::endmsg;
    doocore::io::sinfo << "Range:" << range_.first << "-" << range_.second << doocore::io::endmsg;
    doocore::io::sinfo.increment_indent(-2);
  }

 private:

  std::string name_;
  std::string expression_;
  std::pair< double, double> range_;
  std::string cut_operator_;
  double best_cut_value_;
  std::vector<double> cut_values_;
};

} // namespace triage
} // namespace dooselection

#endif // TRIAGE_CLASSIFIER_H
