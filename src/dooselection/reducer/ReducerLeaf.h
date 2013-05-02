#ifndef DOOSELECTION_REDUCER_REDUCERLEAF_H
#define DOOSELECTION_REDUCER_REDUCERLEAF_H

// from STL
#include <iostream>
#include <string>

// from ROOT
#include "TString.h"
#include "TTree.h"
#include "TTreeFormula.h"

// from DooCore
#include "doocore/io/MsgStream.h"

// forward decalarations
class TLeaf;

enum ReducerLeafOperations {
  kNoneOperation,
  kAddLeaves,
  kMultiplyLeaves,
  kDivideLeaves,
  kEqualLeaf,
};

// Haha, this is why I like C++: The function below needs to be declared for the
// class definition (to give friend access). Therefore, the class ReducerLeaf<T>
// needs to be declared before the function declaration.
namespace dooselection {
namespace reducer {
template <class T>
class ReducerLeaf;
}
}

namespace doocore {
namespace io {
/**
 *  @brief Function to output ReducerLeaves directly and nicely into MsgStreams
 *
 *  This function just prints a ReducerLeaf with all necessary information
 */
template <class T>
inline MsgStream& operator<<(MsgStream& lhs, const dooselection::reducer::ReducerLeaf<T>& leaf);
} // namespace utils
} // namespace doofit


namespace dooselection {
namespace reducer {
/**
 * ReducerLeaf class
 *
 * This helper class represents a leaf in a tree. Two modes of functionality are
 * available (corresponding two constructors):
 *
 * 1. Copying existing leaves via ReducerLeaf<T>::ReducerLeaf(TLeaf* leaf)
 *    T is not important here and can be set to whatever you fancy (it's not 
 *    used anyway). It will take the branch address, name etc. of the existing
 *    leaf. 
 *
 * 2. New leaf mode with templating.
 *    T defines the data type for the leaf. The leaf has to be initialized with 
 *    a TTree pointer of another tree that will be copied and which defines the 
 *    leaf's value. The leaf can be either a conditional one with given cuts on 
 *    leaf's of the reference tree and corresponding values or be a combination 
 *    of reference tree leaves (like sum or ratio and such).
 */
template <class T>
class ReducerLeaf {
public:
  ReducerLeaf(TLeaf* leaf);
  ReducerLeaf(TString name, TString title, TString type, TTree* tree, T default_value=T());
  ReducerLeaf(const ReducerLeaf<T>& r);
  
  ~ReducerLeaf() {
    //std::cout << "destructing " << this << " " << name_ << "|" << &name_ << std::endl;
    if (branch_address_templ_ != NULL) delete branch_address_templ_;
  }
  
  const TString& name() const { return name_; }
  const TString& set_name(const TString& new_name) { name_=new_name; title_=new_name+"/"+type_; return name_; }
  
  const TString& title() const { return title_; }
  const TString& type() const { return type_; }
  TTree* tree() const { return tree_; }
  TBranch* branch() { return leaf_->GetBranch(); }
  
  TString LeafString() const ;   ///< return leaf string for branch creation
  void* branch_address() const { 
    //std::cout << "  ReducerLeaf::branch_address() for " << name_ << "|" << this << std::endl;
    
    // check which type of mode we are
    if (branch_address_ == NULL) {
      return branch_address_templ_;
    } else {
      return branch_address_; 
    }
  }
  
  /**
   *  @brief Get current length of leaf (if array based)
   *
   *  @return length of the array in the leaf (1 if not array based)
   */
  int Length() const;
  
  /**
   *  @brief Get name of leaf containing array length
   *
   *  @return name of the leaf determining the array length
   */
  std::string LengthLeafName() const;
  
  /**
   *  @brief Set value of leaf
   *
   *  @param v value to set the leaf to
   *  @return reference to *this
   */
  ReducerLeaf<T>& operator=(const T& v) {
    *((T*)branch_address()) = v;
    return *this;
  }
  
  void set_branch_address(void* ptr) { branch_address_ = ptr; }
  
  /**
   *  @brief Get value of leaf
   *
   *  Get the value stored in the branch address correctly cast to T (if it's
   *  stored in branch_address_ we have to check for value and cast accordingly).
   *
   *  @param i (optional) index parameter if leaf is array-based
   *  @return leaf value of correct type
   */
  T GetValue(int i=0) const;
  
  /**
   * Create a branch in tree for this leaf. In copy mode it will just use the 
   * old tree's branch address and will work out of the box. In template new 
   * leaf mode EvalConditions() or ... must be used for each entry to 
   * recalculate the values.
   */
  void CreateBranch(TTree * tree) const { tree->Branch(name(), branch_address(), LeafString()); }
  
  ///< add a new condition for new tree leaf/branch
  ///< condition_name is just a name for the TTreeFormula, condition the cut string.
  ///< value the value corresponding to this cut.
  void AddCondition(TString condition_name, TString condition, T value) { 
    conditions_map_.push_back(std::pair<TTreeFormula*,T>(new TTreeFormula(condition_name,condition,tree_),value)); 
  }
  void SetDefaultValue(T value) { 
    default_value_ = value;
  }
  
  /**
   * update the leaf value according to a set conditions map or operation on 
   * other leaves.
   */
  bool UpdateValue();
  
  /**
   * check all conditions for a match and set branch value accordingly
   * returns true if at least one condition matched
   */
  bool EvalConditions();
  
  /**
   * Setup leaf to be basic arithmatic operation of two leaves: c1*l1+c2*l2, l1*l2, l1/l2
   */
  template<class T1, class T2>
  void SetOperation(const ReducerLeaf<T1>& l1, const ReducerLeaf<T2>& l2, ReducerLeafOperations operation, double c1=1.0, double c2=1.0);
  template<class T1, class T2>
  void Add(const ReducerLeaf<T1>& l1, const ReducerLeaf<T2>& l2, double c1=1.0, double c2=1.0) {
    SetOperation<T1,T2>(l1,l2,kAddLeaves,c1,c2);
  }
  template<class T1, class T2>
  void Multiply(const ReducerLeaf<T1>& l1, const ReducerLeaf<T2>& l2, double c1=1.0, double c2=1.0) {
    SetOperation<T1,T2>(l1,l2,kMultiplyLeaves,c1,c2);
  }
  template<class T1, class T2>
  void Divide(const ReducerLeaf<T1>& l1, const ReducerLeaf<T2>& l2, double c1=1.0, double c2=1.0) {
    SetOperation<T1,T2>(l1,l2,kDivideLeaves,c1,c2);
  }
  template<class T1>
  void Equal(const ReducerLeaf<T1>& l, double c=1.0) {
    SetOperation<T1,T1>(l,l,kEqualLeaf,c,c);
  }
  
  template<class T1>
  friend doocore::io::MsgStream& doocore::io::operator<<(doocore::io::MsgStream& lhs, const dooselection::reducer::ReducerLeaf<T1>& leaf);
  
private:
  TLeaf* leaf_;
  TString name_;
  TString title_;
  TString type_;
  void * branch_address_;           ///< address of branch contents 
  ///< for non-templating copy use
  T * branch_address_templ_;        ///< address of branch contents
  ///< for templating new leaf use
  
  /**
   * members needed for new leaves/branches
   *
   */
  TTree* tree_;                                 ///< pointer to our tree
  std::vector<std::pair<TTreeFormula*,T> > conditions_map_; 
  ///< Conditions map for new 
  ///< leaves. Based on given cut
  ///< formulas different values 
  ///< written into the tree.
  T                                         default_value_;

  /**
   *  @brief Pointer to first other leaf for operations
   */
  ReducerLeaf<T>* leaf_pointer_one_;

  /**
   *  @brief Pointer to first other leaf for operations
   */
  ReducerLeaf<T>* leaf_pointer_two_;
  
  double leaf_factor_one_;            ///< leaf factor for basic arithmatic ops
  double leaf_factor_two_;            ///< leaf factor for basic arithmatic ops
  
  ReducerLeafOperations leaf_operation_;  ///< which operation to perform on 
  ///< other leaves
};

template <class T>
ReducerLeaf<T>::ReducerLeaf(TString name, TString title, TString type, TTree* tree, T default_value)
: leaf_(NULL),
name_(name),
title_(title),
type_(type),
branch_address_(NULL),
tree_(tree),
default_value_(default_value),
leaf_pointer_one_(NULL),
leaf_pointer_two_(NULL),
leaf_operation_(kNoneOperation) {
  branch_address_templ_ = new T();
  //std::cout << "regular constructor: " << this << ", name: " << name_ << "|" << &name_ << " (untemplated): " << branch_address_ << ", (templated): " << branch_address_templ_ << std::endl;
}

template <class T>
ReducerLeaf<T>::ReducerLeaf(TLeaf * leaf)
: leaf_(leaf), 
name_(leaf->GetName()),
title_(leaf->GetTitle()),
type_(leaf->GetTypeName()),
branch_address_(leaf->GetValuePointer()),
branch_address_templ_(NULL),
tree_(NULL),
leaf_pointer_one_(NULL),
leaf_pointer_two_(NULL),
leaf_operation_(kNoneOperation) {
}

template <class T>
ReducerLeaf<T>::ReducerLeaf(const ReducerLeaf<T>& r) 
: leaf_(r.leaf_),
name_(r.name_),
title_(r.title_),
type_(r.type_), 
branch_address_(r.branch_address_),
branch_address_templ_(r.branch_address_templ_),
tree_(r.tree_),
leaf_pointer_one_(r.leaf_pointer_one_),
leaf_pointer_two_(r.leaf_pointer_two_),
leaf_factor_one_(r.leaf_factor_one_),
leaf_factor_two_(r.leaf_factor_two_),
leaf_operation_(r.leaf_operation_) {        
  //std::cout << "copy    constructor: " << &r << " -> " << this << ", name: " << name_ << "|" << &name_ << " (untemplated): " << branch_address_ << ", (templated): " << branch_address_templ_ << std::endl;
}

template <class T>
T ReducerLeaf<T>::GetValue(int i) const {
  // too high performance impact
//  if (i>=Length()) {
//    doocore::io::serr << "ERROR in T ReducerLeaf<T>::GetValue(int): Trying to access array element " << i << " in leaf " << name_ << ". Length is " << Length() << ". Will return first element." << doocore::io::endmsg;
//    i=0;
//  }
  
  // in case we have stored our value in branch_address_templ_ it's simple and 
  // we know that the type is correct.
  if (branch_address_ == NULL) {
    return branch_address_templ_[i];
  } else {
    if (type_ == "Int_t") {
      // tricky cast:
      // void* -> Int_t* -> Int_t -> T
      return static_cast<T>(static_cast<Int_t*>(branch_address_)[i]);
    } else if (type_ == "Float_t") {
      return static_cast<T>(static_cast<Float_t*>(branch_address_)[i]);
    } else if (type_ == "Double_t") {
      return static_cast<T>(static_cast<Double_t*>(branch_address_)[i]);
    } else if (type_ == "ULong64_t") {
      return static_cast<T>(static_cast<ULong64_t*>(branch_address_)[i]);
    } else if (type_ == "Long64_t") {
      return static_cast<T>(static_cast<Long64_t*>(branch_address_)[i]);
    } else if (type_ == "UInt_t") {
      return static_cast<T>(static_cast<UInt_t*>(branch_address_)[i]);
    } else if (type_ == "Bool_t") {
      return static_cast<T>(static_cast<Bool_t*>(branch_address_)[i]);
    } else if (type_ == "Short_t") {
      return static_cast<T>(static_cast<Short_t*>(branch_address_)[i]);
    } else if (type_ == "UShort_t") {
      return static_cast<T>(static_cast<UShort_t*>(branch_address_)[i]);
    } else if (type_ == "Char_t") {
      return static_cast<T>(static_cast<Char_t*>(branch_address_)[i]);
    } else if (type_ == "UChar_t") {
      return static_cast<T>(static_cast<UChar_t*>(branch_address_)[i]);
    } else {
      doocore::io::serr << "ERROR in T ReducerLeaf<T>::GetValue(int): Leaf type " << type_ << " in leaf " << name_ << " is unknown. Do not know how to handle that." << doocore::io::endmsg;
      return T();
    }
  }
}

template <class T>
bool ReducerLeaf<T>::EvalConditions() {
  bool matched = false;
  if (conditions_map_.empty()) {
    std::cerr << "ERROR in ReducerLeaf<T>::EvalConditions(): conditions map is empty." << std::endl;
  } else {
    for (typename std::vector<std::pair<TTreeFormula*,T> >::const_iterator it = conditions_map_.begin(); it != conditions_map_.end(); ++it) {
      // check if this condition matches
      if ((*it).first->EvalInstance()) {
        *branch_address_templ_ = (*it).second;
        matched = true;
      }
    }
    // if no match is found use the default value
    if (!matched) {
      *branch_address_templ_ = default_value_;
    }
  }
  return matched;
}

template <class T>
bool ReducerLeaf<T>::UpdateValue() {
  bool matched = false;
  if (!conditions_map_.empty()) {
    return EvalConditions();
  } else if (leaf_operation_ != kNoneOperation) {
    // update our leaf pointers which could itself be depending on operations.
    leaf_pointer_one_->UpdateValue();
    leaf_pointer_two_->UpdateValue();
    
    switch (leaf_operation_) {
      case kAddLeaves:
        *branch_address_templ_ = leaf_factor_one_*leaf_pointer_one_->GetValue()+leaf_factor_two_*leaf_pointer_two_->GetValue();
        matched = true;
        break;
      case kMultiplyLeaves:
        *branch_address_templ_ = leaf_factor_one_*leaf_pointer_one_->GetValue()*leaf_factor_two_*leaf_pointer_two_->GetValue();
        matched = true;
        break;
      case kDivideLeaves:
        *branch_address_templ_ = leaf_factor_one_*leaf_pointer_one_->GetValue()/(leaf_factor_two_*leaf_pointer_two_->GetValue());
        matched = true;
        break;
      case kEqualLeaf:
        *branch_address_templ_ = leaf_factor_one_*leaf_pointer_one_->GetValue();
        break;
      default:
        break;
    }
  } else {
    *branch_address_templ_ = default_value_;
  }
  return matched;
}

template <class T> template <class T1, class T2>
void ReducerLeaf<T>::SetOperation(const ReducerLeaf<T1>& l1, const ReducerLeaf<T2>& l2, ReducerLeafOperations operation, double c1, double c2) {
  switch (operation) {
    case kAddLeaves:
      std::cout << "Leaf " << name() << " = (" << c1 << ")*" << l1.name() << "+(" << c2 << ")*" << l2.name() << std::endl;
      break;
    case kMultiplyLeaves:
      std::cout << "Leaf " << name() << " = (" << c1 << ")*" << l1.name() << "*(" << c2 << ")*" << l2.name() << std::endl;
      break;
    case kDivideLeaves:
      std::cout << "Leaf " << name() << " = (" << c1 << ")*" << l1.name() << "/(" << c2 << ")*" << l2.name() << std::endl;
      break;
    case kEqualLeaf:
      std::cout << "Leaf " << name() << " = (" << c1 << ")*" << l1.name() << std::endl;
      break;
    default:
      break;
  }
  
  leaf_factor_one_ = c1;
  leaf_factor_two_ = c2;
  
  // leaf_pointer_* need to be ReducerLeaf<T>* with T to be the same as the mother's
  // T. Even if T1 != T. Although not very nice, this does not hurt. leaf_pointer_*
  // will be treated as generic ReducerLeafs where templating does not matter.
  // Their value is returned via ReducerLeaf<T>::GetValue() which checks type_ 
  // entry and casts accordingly.
  leaf_pointer_one_ = new ReducerLeaf<T>(l1.name(), l1.title(), l1.type(), l1.tree());
  leaf_pointer_one_->branch_address_ = l1.branch_address();
  
  leaf_pointer_two_ = new ReducerLeaf<T>(l2.name(), l2.title(), l2.type(), l2.tree());
  leaf_pointer_two_->branch_address_ = l2.branch_address();
  
  leaf_operation_ = operation;
}

template <class T>
TString ReducerLeaf<T>::LeafString() const {
  TString postfix = "";
  if (type_ == "Float_t") {
    postfix = "/F";
  } else if (type_ == "Double_t") {
    postfix = "/D";
  } else if (type_ == "Int_t") {
    postfix = "/I";
  } else if (type_ == "UInt_t") {
    postfix = "/i";
  } else if (type_ == "Bool_t") {
    postfix = "/O";
  } else if (type_ == "Long64_t") {
    postfix = "/L";
  } else if (type_ == "ULong64_t") {
    postfix = "/l";
  } else if (type_ == "Short_t") {
    postfix = "/S";
  } else if (type_ == "UShort_t") {
    postfix = "/s";
  } else if (type_ == "Char_t") {
    postfix = "/B";
  } else if (type_ == "UChar_t") {
    postfix = "/b";
  } else {
    doocore::io::serr << "Error in ReducerLeaf<T>::LeafString(): Encountered unknown leaf type " << type_ << " in leaf " << name_ << doocore::io::endmsg;
  }
  
  TString leaf_string = title_ + postfix;
  
  return leaf_string;
}

template <class T>
int ReducerLeaf<T>::Length() const {
  if (leaf_ != NULL) {
    return leaf_->GetLen();
  } else {
    return 1;
  }
}

template <class T>
std::string ReducerLeaf<T>::LengthLeafName() const {
  if (leaf_ != NULL && leaf_->GetLeafCount() != NULL) {
    return leaf_->GetLeafCount()->GetName();
  } else {
    return "";
  }
}
  
} // namespace reducer
} // namespace dooselection


namespace doocore {
namespace io {
template <class T>
inline MsgStream& operator<<(MsgStream& lhs, const dooselection::reducer::ReducerLeaf<T>& leaf) {
  lhs.stream() << "ReducerLeaf<" << leaf.type() << "> " << leaf.name() << " (" << leaf.title() << ") = " << leaf.GetValue();
  
  for (int i=1; i<leaf.Length(); ++i) {
    lhs.stream() << ", " << leaf.GetValue(i);
  }
  
  return lhs;
}
} // namespace utils
} // namespace doofit

#endif // DOOSELECTION_REDUCER_REDUCERLEAF_H
