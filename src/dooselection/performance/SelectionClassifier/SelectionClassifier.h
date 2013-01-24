#ifndef DOOSELECTION_PERFORMANCE_SELECTIONCLASSIFIER_H
#define DOOSELECTION_PERFORMANCE_SELECTIONCLASSIFIER_H

// from STL
#include <vector>

// from ROOT

// from RooFit

// from TMVA

// from BOOST
#include "boost/lexical_cast.hpp"

// from DooCore
#include "doocore/io/MsgStream.h"

// from here

// forward declarations

namespace selection{
  class SelectionClassifier{
   public:
    SelectionClassifier(std::string name, std::string title, double range_min, double range_max, std::string cut_operator, double best_cut_value, int number_of_steps, bool debug_mode=false):
    debug_mode_(debug_mode),
    use_number_of_steps_(true),
    use_step_size_(false),
    use_user_defined_steps_(false),
    name_(name),
    title_(title),
    cut_operator_(cut_operator),
    best_cut_string_(""),
    number_of_steps_(number_of_steps),
    best_cut_value_(best_cut_value),
    range_min_(range_min),
    range_max_(range_max),
    step_size_(1),
		steps_()
    {
    	if (debug_mode_) doocore::io::serr << "-debug- " << "Created new SelectionClassifier '" << title << "'..." << doocore::io::endmsg;
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t range: " << range_min_ << "-" << range_max_ << doocore::io::endmsg;

    	best_cut_string_ = name_+cut_operator_+boost::lexical_cast<std::string>(best_cut_value_);
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t best cut string: " << best_cut_string_ << doocore::io::endmsg;

    	step_size_=(range_max_-range_min_)/number_of_steps;
    	for (int i = 0; i < number_of_steps; ++i)
    	{
    		steps_.push_back(range_min_+i*step_size_);
    	}
    	steps_.push_back(range_max_);
    	std::string temp = "["+boost::lexical_cast<std::string>(range_min_);
    	for(std::vector<std::string>::iterator it = steps_.begin()+1; it != steps_.end(); it++){
  			temp+=boost::lexical_cast<std::string>(*it);
			}
			temp+="]";
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t scan points: " << temp << "'" << doocore::io::endmsg;
    }

    SelectionClassifier(std::string name, std::string title, double range_min, double range_max, std::string cut_operator, double best_cut_value, double step_size, bool debug_mode=false):
    debug_mode_(debug_mode),
    use_number_of_steps_(false),
    use_step_size_(true),
    use_user_defined_steps_(false),
    name_(name),
    title_(title),
    cut_operator_(cut_operator),
    best_cut_string_(""),
    number_of_steps_(10),
    best_cut_value_(best_cut_value),
    range_min_(range_min),
    range_max_(range_max),
    step_size_(step_size),
		steps_()
    {
    	if (debug_mode_) doocore::io::serr << "-debug- " << "Created new SelectionClassifier '" << title << "'..." << doocore::io::endmsg;
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t range: " << range_min_ << "-" << range_max_ << doocore::io::endmsg;

    	best_cut_string_ = name_+cut_operator_+boost::lexical_cast<std::string>(best_cut_value_);
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t best cut string: " << best_cut_string_ << doocore::io::endmsg;

    	for (double i = range_min_; i > range_max_; i+=step_size_)
    	{
    		steps_.push_back(i);
    	}

    	std::string temp = "["+boost::lexical_cast<std::string>(range_min_);
    	for(std::vector<std::string>::iterator it = steps_.begin()+1; it != steps_.end(); it++){
  			temp+=boost::lexical_cast<std::string>(*it);
			}
			temp+="]";
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t scan points: " << temp << "'" << doocore::io::endmsg;
    }

    SelectionClassifier(std::string name, std::string title, double range_min, double range_max, std::string cut_operator, double best_cut_value, std::vector<double> steps, bool debug_mode=false):
    debug_mode_(debug_mode),
    use_number_of_steps_(false),
    use_step_size_(false),
    use_user_defined_steps_(true),
    name_(name),
    title_(title),
    cut_operator_(""),
    best_cut_string_(""),
    number_of_steps_(10),
    best_cut_value_(0),
    range_min_(range_min),
    range_max_(range_max),
    step_size_(1),
		steps_(steps)
    {
      if (debug_mode_) doocore::io::serr << "-debug- " << "Created new SelectionClassifier '" << title << "'..." << doocore::io::endmsg;
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t range: " << range_min_ << "-" << range_max_ << doocore::io::endmsg;

    	best_cut_string_ = name_+cut_operator_+boost::lexical_cast<std::string>(best_cut_value_);
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t best cut string: " << best_cut_string_ << doocore::io::endmsg;

    	std::string temp = "["+boost::lexical_cast<std::string>(range_min_);
    	for(std::vector<std::string>::iterator it = steps_.begin()+1; it != steps_.end(); it++){
  			temp+=boost::lexical_cast<std::string>(*it);
			}
			temp+="]";
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t scan points: " << temp << "'" << doocore::io::endmsg;
    }

    SelectionClassifier(std::string name, std::string title, double range_min, double range_max, std::string best_cut_string, int number_of_steps, bool debug_mode=false):
    debug_mode_(debug_mode),
    use_number_of_steps_(true),
    use_step_size_(false),
    use_user_defined_steps_(false),
    name_(name),
    title_(title),
    cut_operator_(""),
    best_cut_string_(best_cut_string),
    number_of_steps_(number_of_steps),
    best_cut_value_(0),
    range_min_(range_min),
    range_max_(range_max),
    step_size_(1),
		steps_()
    {
    	if (debug_mode_) doocore::io::serr << "-debug- " << "Created new SelectionClassifier '" << title << "'..." << doocore::io::endmsg;
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t range: " << range_min_ << "-" << range_max_ << doocore::io::endmsg;
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t best cut string: " << best_cut_string_ << doocore::io::endmsg;

    	step_size_=(range_max_-range_min_)/number_of_steps;
    	for (int i = 0; i < number_of_steps; ++i)
    	{
    		steps_.push_back(range_min_+i*step_size_);
    	}
    	steps_.push_back(range_max_);
    	std::string temp = "["+boost::lexical_cast<std::string>(range_min_);
    	for(std::vector<std::string>::iterator it = steps_.begin()+1; it != steps_.end(); it++){
  			temp+=boost::lexical_cast<std::string>(*it);
			}
			temp+="]";
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t scan points: " << temp << "'" << doocore::io::endmsg;
    }

    SelectionClassifier(std::string name, std::string title, double range_min, double range_max, std::string best_cut_string, double step_size, bool debug_mode=false):
    debug_mode_(debug_mode),
    use_number_of_steps_(false),
    use_step_size_(true),
    use_user_defined_steps_(false),
    name_(name),
    title_(title),
    cut_operator_(""),
    best_cut_string_(best_cut_string),
    number_of_steps_(10),
    best_cut_value_(0),
    range_min_(range_min),
    range_max_(range_max),
    step_size_(step_size),
		steps_()
    {
    	if (debug_mode_) doocore::io::serr << "-debug- " << "Created new SelectionClassifier '" << title << "'..." << doocore::io::endmsg;
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t range: " << range_min_ << "-" << range_max_ << doocore::io::endmsg;
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t best cut string: " << best_cut_string_ << doocore::io::endmsg;

    	for (double i = range_min_; i > range_max_; i+=step_size_)
    	{
    		steps_.push_back(i);
    	}

    	std::string temp = "["+boost::lexical_cast<std::string>(range_min_);
    	for(std::vector<std::string>::iterator it = steps_.begin()+1; it != steps_.end(); it++){
  			temp+=boost::lexical_cast<std::string>(*it);
			}
			temp+="]";
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t scan points: " << temp << "'" << doocore::io::endmsg;
    }

    SelectionClassifier(std::string name, std::string title, double range_min, double range_max, std::string best_cut_string, std::vector<double> steps, bool debug_mode=false):
    debug_mode_(debug_mode),
    use_number_of_steps_(false),
    use_step_size_(false),
    use_user_defined_steps_(true),
    name_(name),
    title_(title),
    cut_operator_(""),
    best_cut_string_(best_cut_string),
    number_of_steps_(10),
    best_cut_value_(0),
    range_min_(range_min),
    range_max_(range_max),
    step_size_(1),
		steps_(steps)
    {
    	if (debug_mode_) doocore::io::serr << "-debug- " << "Created new SelectionClassifier '" << title << "'..." << doocore::io::endmsg;
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t range: " << range_min_ << "-" << range_max_ << doocore::io::endmsg;
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t best cut string: " << best_cut_string_ << doocore::io::endmsg;

    	std::string temp = "["+boost::lexical_cast<std::string>(range_min_);
    	for(std::vector<std::string>::iterator it = steps_.begin()+1; it != steps_.end(); it++){
  			temp+=boost::lexical_cast<std::string>(*it);
			}
			temp+="]";
    	if (debug_mode_) doocore::io::serr << "-debug- " << "\t scan points: " << temp << "'" << doocore::io::endmsg;
    }

    ~SelectionClassifier(){};

    // SETTER
    void set_debug_mode(bool debug_mode){
    	debug_mode_=debug_mode;
    }
    void set_cut_operator(std::string cut_operator){
      cut_operator_=cut_operator;
    }
    void set_best_cut_string(std::string best_cut_string){
    	best_cut_string=best_cut_string_;
    }
    void set_number_of_steps(int number_of_steps){
    	number_of_steps_=number_of_steps;
    }
    void set_best_cut_value(double best_cut_value){
    	best_cut_value_=best_cut_value;
    }
    void set_range_min(double range_min){
    	range_min_=range_min;
    }
    void set_range_max(double range_max){
    	range_max_=range_max;
    }
    void set_step_size(double step_size){
    	step_size_=step_size;
    }
    void set_steps(std::vector<double> steps){
    	steps_=steps;
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
    std::string cut_operator(){
    	return cut_operator_;
    }
    std::string best_cut_string(){
    	return best_cut_string_;
    }
    int number_of_steps(){
    	return number_of_steps_;
    }
    double best_cut_value(){
    	return best_cut_value_;
    }
    double range_min(){
    	return range_min_;
    }
    double range_max(){
    	return range_max_;
    }
    double step_size(){
    	return step_size_;
    }
    std::vector<double> steps(){
    	return steps_;
    }

   private:
    // MEMBER VARIABLES
    bool debug_mode_;
    bool use_number_of_steps_;
    bool use_step_size_;
    bool use_user_defined_steps_;

    std::string name_;
    std::string title_;
    std::string cut_operator_;
    std::string best_cut_string_;
    
    int number_of_steps_;

    double best_cut_value_;
    double range_min_;
    double range_max_;
    double step_size_;

    std::vector<double> steps_;

  };
} // namespace selection

#endif // DOOSELECTION_PERFORMANCE_SELECTIONCLASSIFIER_H