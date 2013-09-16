#ifndef TRIAGE_CLASSIFIERLIST_H
#define TRIAGE_CLASSIFIERLIST_H

// from STL
#include <vector>

// from ROOT

// from RooFit

// from TMVA

// from BOOST

// from DooCore

// from here
#include "Classifier.h"

// forward declarations

namespace dooselection{
namespace triage{

class ClassifierList{
 public:
  ClassifierList(std::string name):
  name_(name),
  classifier_list_(){}

  ClassifierList():
  name_(""),
  classifier_list_(){}

  void Add(const Classifier& classifier){classifier_list_.push_back(classifier);}
  void CombineAll(){
    std::string combined_cut;
    for(std::vector<Classifier>::const_iterator it = classifier_list_.begin(); it != classifier_list_.end(); it++){
      if (it!=classifier_list_.begin()) combined_cut+="&&";
      combined_cut+=(*it).best_cut_string();
    }
    Classifier combined_classifier(name_+"_combined", combined_cut);
    classifier_list_.push_back(combined_classifier);
  }

  typedef std::vector<Classifier>::iterator iterator;
  typedef std::vector<Classifier>::const_iterator const_iterator;

  iterator begin(){return classifier_list_.begin();}
  const_iterator begin() const{return classifier_list_.begin();}
  
  iterator end(){return classifier_list_.end();}
  const_iterator end() const{return classifier_list_.end();}

 private:
  std::string name_;
  std::vector<Classifier> classifier_list_;
};

} // namespace triage
} // namespace dooselection

#endif // TRIAGE_CLASSIFIERLIST_H
