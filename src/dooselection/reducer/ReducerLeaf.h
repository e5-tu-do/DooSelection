#ifndef DOOSELECTION_REDUCER_REDUCERLEAF_H
#define DOOSELECTION_REDUCER_REDUCERLEAF_H

// from STL
#include <iostream>
#include <string>

// from Boost
#include <boost/algorithm/string.hpp>

// from ROOT
#include "TString.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TRandom.h"

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
  kRandomizeLeaf,
  kMinimum,
  kMaximum,
  kConditionsMap,
  kDoNotUpdate
};

// Haha, this is why I like C++: The function below needs to be declared for the
// class definition (to give friend access). Therefore, the class ReducerLeaf<T>
// needs to be declared before the function declaration.
namespace dooselection {
namespace reducer {
  
enum LeafDataType {
  kDouble,
  kInt,
  kFloat,
  kULong64,
  kLong64,
  kUInt,
  kBool,
  kShort,
  kUShort,
  kChar,
  kUChar,
  kUnknownType,
};
  
template <class T>
class ReducerLeaf;
  
template <class T>
class KinematicReducerLeaf;
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

/**
 * @class dooselection::reducer::ReducerLeaf
 *
 * @brief General Reducer leaf
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

namespace dooselection {
namespace reducer {
template <class T>
class ReducerLeaf {
  friend class KinematicReducerLeaf<T>;
  
public:
  ReducerLeaf(TLeaf* leaf);
  ReducerLeaf(TString name, TString title, TString type, TTree* tree, T default_value=T());
  ReducerLeaf(const ReducerLeaf<T>& r);
  
  virtual ~ReducerLeaf() {
    //std::cout << "destructing " << this << " " << name_ << "|" << &name_ << std::endl;
    if (branch_address_templ_ != NULL) delete branch_address_templ_;
  }
  
  /** @name Leaf properties
   *  These functions access global leaf properties.
   */
  ///@{
  const TString& name() const { return name_; }
  const TString& title() const { return title_; }
  const TString& type() const { return type_; }
  
  LeafDataType l_type() const { return l_type_; }
  
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
   *  @brief Get name of leaf containing array length
   *
   *  @return name of the leaf determining the array length
   */
  std::string LengthLeafName() const;
  ///@}
  
  /** @name Set Leaf properties
   *  These functions set global leaf properties.
   */
  ///@{
  const TString& set_name(const TString& new_name) { name_=new_name; title_=new_name+"/"+type_; return name_; }
  void set_branch_address(void* ptr) { branch_address_ = ptr; }
  void SetDefaultValue(T value) {
    default_value_ = value;
  }
  ///@}
  
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
  
  /** @name Leaf value access
   *  These functions help accessing leaf values
   */
  ///@{
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
   *  @brief Get current length of leaf (if array based)
   *
   *  @return length of the array in the leaf (1 if not array based)
   */
  int Length() const;
  ///@}
  
  /**
   * Create a branch in tree for this leaf. In copy mode it will just use the 
   * old tree's branch address and will work out of the box. In template new 
   * leaf mode EvalConditions() or ... must be used for each entry to
   * recalculate the values.
   */
  void CreateBranch(TTree * tree) const { tree->Branch(name(), branch_address(), LeafString()); }
    
  /** @name Leaf value determination
   *  These functions assure setting the leaves value correctly.
   */
  ///@{
  /**
   * @brief Update the leaf value according to a set conditions map or operation on other leaves for the current event.
   *
   * Due to virtuality, higher level ReducerLeaves can implement other 
   * operations as well.
   *
   * @return whether the any operation set the value
   */
  virtual bool UpdateValue();
    
  /**
   * Check all conditions for a match and set branch value accordingly
   * returns true if at least one condition matched
   */
  bool EvalConditions();
  ///@}
  
  /** @name Conditional leaf operations
   *  These functions set the leaf to be based on a conditional table
   */
  ///@{
  /**
   * @brief Add a new condition for new tree leaf/branch
   * @param condition_name is just a name for the TTreeFormula
   * @param condition the cut string.
   * @param value the value corresponding to this cut.
   */
  void AddCondition(TString condition_name, TString condition, T value) {
    leaf_operation_ = kConditionsMap;
    conditions_map_.push_back(std::pair<TTreeFormula*,T>(new TTreeFormula(condition_name,condition,tree_),value));
  }

  ///@}
  
  /** @name Artihmetic leaf operations
   *  These functions set the leaf to be based on a arithmetic operation
   */
  ///@{
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
  template<class T1, class T2>
  void Minimum(const ReducerLeaf<T1>& l1, const ReducerLeaf<T2>& l2) {
    SetOperation<T1,T2>(l1,l2,kMinimum,1.0,1.0);
  }
  template<class T1, class T2>
  void Maximum(const ReducerLeaf<T1>& l1, const ReducerLeaf<T2>& l2) {
    SetOperation<T1,T2>(l1,l2,kMaximum,1.0,1.0);
  }

  /**
   *  @brief Set leaf to contain random values
   *
   *  Based on a supplied random generator, the leaf will contain random 
   *  numbers, based on the supplied random generator. Each time the value of
   *  the leaf is updated, a new random number is stored internally. Random 
   *  numbers are drawn uniquely between 0 and 2^30.
   *
   *  @param random_generator a random generator to use.
   */
  void Randomize(TRandom* random_generator) {
    random_generator_ = random_generator;
    SetOperation<T,T>(*this,*this,kRandomizeLeaf,1.0,1.0);
  }
  ///@}
  
  /** @name Access dependent leaves
   *  These functions help to access dependent leaves
   */
  ///@{
  /**
   *  @brief Get first dependent leaf
   */
  const ReducerLeaf<T>* leaf_pointer_one() const {
    return leaf_pointer_one_;
  }

  /**
   *  @brief Get second dependent leaf
   */
  const ReducerLeaf<T>* leaf_pointer_two() const {
    return leaf_pointer_two_;
  }
  ///@}
  
  /**
   *  @brief Activate all leaves in conditions map for a given tree
   *
   *  @param tree the TTree to actiavte dependent leaves in
   */
  void ActivateDependentConditionLeaves(TTree* tree) const {
    for (auto condition : conditions_map_) {
      const TTreeFormula* formula = condition.first;
      
      // split formula string into tokens to iterate through these and reactivate all necessary leaves
      std::string cut_string(formula->GetTitle());
      std::vector<std::string> vector_split;
      boost::split(vector_split, cut_string, boost::is_any_of("(())&|<>="), boost::token_compress_on);
      for (auto token : vector_split) {
        TLeaf* leaf = tree->GetLeaf(token.c_str());
        if (leaf != NULL) {
          //doocore::io::sdebug << "Reactivating " << token << " (needed in conditions for " << name() << ")" << doocore::io::endmsg;
          tree->SetBranchStatus(token.c_str(), 1);
        }
      }
    }
  }
  
  template<class T1>
  friend doocore::io::MsgStream& doocore::io::operator<<(doocore::io::MsgStream& lhs, const dooselection::reducer::ReducerLeaf<T1>& leaf);
  
protected:
  T * branch_address_templ_;        ///< address of branch contents
                                    ///< for templating new leaf use

  T default_value_;

private:
  
  /**
   *  @brief Set leaf type enum according to leaf type string
   *
   */
  void SetLeafType();
  
  TLeaf* leaf_;
  TString name_;
  TString title_;
  TString type_;
  
  /**
   *  @brief Leaf type enum
   */
  LeafDataType l_type_;
  
  void * branch_address_;           ///< address of branch contents 
  ///< for non-templating copy use
    
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
  
  /**
   *  @brief Connected random generator
   */
  TRandom* random_generator_;
};

template <class T>
ReducerLeaf<T>::ReducerLeaf(TString name, TString title, TString type, TTree* tree, T default_value)
: leaf_(NULL),
name_(name),
title_(title),
type_(type),
l_type_(kUnknownType),
branch_address_(NULL),
tree_(tree),
default_value_(default_value),
leaf_pointer_one_(NULL),
leaf_pointer_two_(NULL),
leaf_operation_(kNoneOperation),
random_generator_(NULL)
{
  SetLeafType();
  branch_address_templ_ = new T();
  //std::cout << "regular constructor: " << this << ", name: " << name_ << "|" << &name_ << " (untemplated): " << branch_address_ << ", (templated): " << branch_address_templ_ << std::endl;
}

template <class T>
ReducerLeaf<T>::ReducerLeaf(TLeaf * leaf)
: leaf_(leaf), 
name_(leaf->GetName()),
title_(leaf->GetTitle()),
type_(leaf->GetTypeName()),
l_type_(kUnknownType),
branch_address_(leaf->GetValuePointer()),
branch_address_templ_(NULL),
tree_(NULL),
leaf_pointer_one_(NULL),
leaf_pointer_two_(NULL),
leaf_operation_(kNoneOperation),
random_generator_(NULL)
{
  SetLeafType();
}

template <class T>
ReducerLeaf<T>::ReducerLeaf(const ReducerLeaf<T>& r) 
: leaf_(r.leaf_),
name_(r.name_),
title_(r.title_),
type_(r.type_),
l_type_(r.l_type_),
branch_address_(r.branch_address_),
branch_address_templ_(r.branch_address_templ_),
tree_(r.tree_),
leaf_pointer_one_(r.leaf_pointer_one_),
leaf_pointer_two_(r.leaf_pointer_two_),
leaf_factor_one_(r.leaf_factor_one_),
leaf_factor_two_(r.leaf_factor_two_),
leaf_operation_(r.leaf_operation_),
random_generator_(NULL)
{        
  //std::cout << "copy    constructor: " << &r << " -> " << this << ", name: " << name_ << "|" << &name_ << " (untemplated): " << branch_address_ << ", (templated): " << branch_address_templ_ << std::endl;
}

template <class T>
void ReducerLeaf<T>::SetLeafType() {
  if (type_ == "Double_t") {
    l_type_ = kDouble;
  } else if (type_ == "Int_t") {
    l_type_ = kInt;
  } else if (type_ == "Float_t") {
    l_type_ = kFloat;
  } else if (type_ == "ULong64_t") {
    l_type_ = kULong64;
  } else if (type_ == "Long64_t") {
    l_type_ = kLong64;
  } else if (type_ == "UInt_t") {
    l_type_ = kUInt;
  } else if (type_ == "Bool_t") {
    l_type_ = kBool;
  } else if (type_ == "Short_t") {
    l_type_ = kShort;
  } else if (type_ == "UShort_t") {
    l_type_ = kUShort;
  } else if (type_ == "Char_t") {
    l_type_ = kChar;
  } else if (type_ == "UChar_t") {
    l_type_ = kUChar;
  } else {
    doocore::io::serr << "ERROR in T ReducerLeaf<T>::SetLeafType(): Leaf type " << type_ << " in leaf " << name_ << " is unknown. Do not know how to handle that." << doocore::io::endmsg;
    l_type_ = kUnknownType;
  }
}
  
template <class T>
T ReducerLeaf<T>::GetValue(int i) const {
  // too high performance impact
//  if (i>=Length()) {
//    doocore::io::serr << "ERROR in T ReducerLeaf<T>::GetValue(int): Trying to access array element " << i << " in leaf " << name_ << ". Length is " << Length() << ". Will return first element." << doocore::io::endmsg;
//    i=0;
//  }
  
//  doocore::io::sdebug << "Get leaf value for " << name() << doocore::io::endmsg;
  
  // in case we have stored our value in branch_address_templ_ it's simple and 
  // we know that the type is correct.
  if (branch_address_ == NULL) {
    return branch_address_templ_[i];
  } else {
    switch (l_type_) {
      case kFloat:
        return static_cast<T>(static_cast<Float_t*>(branch_address_)[i]);
        break;
      case kDouble:
        return static_cast<T>(static_cast<Double_t*>(branch_address_)[i]);
        break;
      case kInt:
        return static_cast<T>(static_cast<Int_t*>(branch_address_)[i]);
        break;
      case kULong64:
        return static_cast<T>(static_cast<ULong64_t*>(branch_address_)[i]);
        break;
      case kLong64:
        return static_cast<T>(static_cast<Long64_t*>(branch_address_)[i]);
        break;
      case kUInt:
        return static_cast<T>(static_cast<UInt_t*>(branch_address_)[i]);
        break;
      case kBool:
        return static_cast<T>(static_cast<Bool_t*>(branch_address_)[i]);
        break;
      case kShort:
        return static_cast<T>(static_cast<Short_t*>(branch_address_)[i]);
        break;
      case kChar:
        return static_cast<T>(static_cast<Char_t*>(branch_address_)[i]);
        break;
      case kUChar:
        return static_cast<T>(static_cast<UChar_t*>(branch_address_)[i]);
        break;
        
      default:
        doocore::io::serr << "ERROR in T ReducerLeaf<T>::GetValue(int): Leaf type " << type_ << " in leaf " << name_ << " is unknown. Do not know how to handle that." << doocore::io::endmsg;
        return T();
        break;
    }
    
//    if (type_ == "Double_t") {
//      return static_cast<T>(static_cast<Double_t*>(branch_address_)[i]);
//    } else if (type_ == "Int_t") {
//      // tricky cast:
//      // void* -> Int_t* -> Int_t -> T
//      return static_cast<T>(static_cast<Int_t*>(branch_address_)[i]);
//    } else if (type_ == "Float_t") {
//      return static_cast<T>(static_cast<Float_t*>(branch_address_)[i]);
//    } else if (type_ == "ULong64_t") {
//      return static_cast<T>(static_cast<ULong64_t*>(branch_address_)[i]);
//    } else if (type_ == "Long64_t") {
//      return static_cast<T>(static_cast<Long64_t*>(branch_address_)[i]);
//    } else if (type_ == "UInt_t") {
//      return static_cast<T>(static_cast<UInt_t*>(branch_address_)[i]);
//    } else if (type_ == "Bool_t") {
//      return static_cast<T>(static_cast<Bool_t*>(branch_address_)[i]);
//    } else if (type_ == "Short_t") {
//      return static_cast<T>(static_cast<Short_t*>(branch_address_)[i]);
//    } else if (type_ == "UShort_t") {
//      return static_cast<T>(static_cast<UShort_t*>(branch_address_)[i]);
//    } else if (type_ == "Char_t") {
//      return static_cast<T>(static_cast<Char_t*>(branch_address_)[i]);
//    } else if (type_ == "UChar_t") {
//      return static_cast<T>(static_cast<UChar_t*>(branch_address_)[i]);
//    } else {
//      doocore::io::serr << "ERROR in T ReducerLeaf<T>::GetValue(int): Leaf type " << type_ << " in leaf " << name_ << " is unknown. Do not know how to handle that." << doocore::io::endmsg;
//      return T();
//    }
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
//  doocore::io::sdebug << "Updating leaf value for " << name() << doocore::io::endmsg;
  
  bool matched = false;
  if (leaf_operation_ != kNoneOperation) {
    // update our leaf pointers which could itself be depending on operations.
    
    switch (leaf_operation_) {
      case kAddLeaves:
        leaf_pointer_one_->UpdateValue();
        leaf_pointer_two_->UpdateValue();
        *branch_address_templ_ = leaf_factor_one_*leaf_pointer_one_->GetValue()+leaf_factor_two_*leaf_pointer_two_->GetValue();
        matched = true;
        break;
      case kMultiplyLeaves:
        leaf_pointer_one_->UpdateValue();
        leaf_pointer_two_->UpdateValue();
        *branch_address_templ_ = leaf_factor_one_*leaf_pointer_one_->GetValue()*leaf_factor_two_*leaf_pointer_two_->GetValue();
        matched = true;
        break;
      case kDivideLeaves:
        leaf_pointer_one_->UpdateValue();
        leaf_pointer_two_->UpdateValue();
        *branch_address_templ_ = leaf_factor_one_*leaf_pointer_one_->GetValue()/(leaf_factor_two_*leaf_pointer_two_->GetValue());
        matched = true;
        break;
      case kEqualLeaf:
        leaf_pointer_one_->UpdateValue();
        *branch_address_templ_ = leaf_factor_one_*leaf_pointer_one_->GetValue();
        matched = true;
        break;
      case kMinimum:
        leaf_pointer_one_->UpdateValue();
        leaf_pointer_two_->UpdateValue();
        *branch_address_templ_ = TMath::Min(leaf_factor_one_*leaf_pointer_one_->GetValue(), leaf_factor_two_*leaf_pointer_two_->GetValue());
        matched = true;
        break;
      case kMaximum:
        leaf_pointer_one_->UpdateValue();
        leaf_pointer_two_->UpdateValue();
        *branch_address_templ_ = TMath::Max(leaf_factor_one_*leaf_pointer_one_->GetValue(), leaf_factor_two_*leaf_pointer_two_->GetValue());
        matched = true;
        break;
      case kRandomizeLeaf:
        *branch_address_templ_ = random_generator_->Rndm()*1073741824.0;
        matched = true;
        break;
      case kConditionsMap:
        if (!conditions_map_.empty()) {
          return EvalConditions();
        } else {
          *branch_address_templ_ = default_value_;
        }
        break;
      case kDoNotUpdate:
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
    case kMinimum:
      std::cout << "Leaf " << name() << " = min(" << l1.name() << ", " << l2.name() << ")" << std::endl;
      break;
    case kMaximum:
      std::cout << "Leaf " << name() << " = max(" << l1.name() << ", " << l2.name() << ")" << std::endl;
      break;
    case kRandomizeLeaf:
      std::cout << "Leaf " << name() << " = random number" << std::endl;
      break;
    default:
      break;
  }
  
  leaf_factor_one_ = c1;
  leaf_factor_two_ = c2;
  
  // leaf_pointer_* need to be ReducerLeaf<T>* with T to be the same as the
  // mother's T. Even if T1 != T. Although not very nice, this does not hurt.
  // leaf_pointer_* will be treated as generic ReducerLeafs where templating
  // does not matter.
  // Their value is returned via ReducerLeaf<T>::GetValue() which checks type_ 
  // entry and casts accordingly.
  if (operation != kRandomizeLeaf) {
    leaf_pointer_one_ = new ReducerLeaf<T>(l1.name(), l1.title(), l1.type(), l1.tree());
    leaf_pointer_one_->branch_address_ = l1.branch_address();
    
    leaf_pointer_two_ = new ReducerLeaf<T>(l2.name(), l2.title(), l2.type(), l2.tree());
    leaf_pointer_two_->branch_address_ = l2.branch_address();
  }
  
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
} // namespace reducer
} // namespace dooselection

#endif // DOOSELECTION_REDUCER_REDUCERLEAF_H
