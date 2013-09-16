#ifndef TRIAGE_TUPLELIST_H
#define TRIAGE_TUPLELIST_H

// from STL
#include <vector>

// from ROOT

// from RooFit

// from TMVA

// from BOOST

// from DooCore

// from here
#include "Tuple.h"

// forward declarations

namespace dooselection{
namespace triage{

class TupleList{
 public:
  TupleList():
  tuple_list_(){}

  void Add(Tuple& tuple){tuple_list_.push_back(&tuple);}

  typedef std::vector<Tuple*>::iterator iterator;
  typedef std::vector<Tuple*>::const_iterator const_iterator;

  iterator begin(){return tuple_list_.begin();}
  const_iterator begin() const{return tuple_list_.begin();}
  
  iterator end(){return tuple_list_.end();}
  const_iterator end() const{return tuple_list_.end();}

 private:
  std::vector<Tuple*> tuple_list_;
};

} // namespace triage
} // namespace dooselection

#endif // TRIAGE_TUPLELIST_H
