#ifndef DOOSELECTION_REDUCER_BKGCATEGORIZERREDUCER2_H
#define DOOSELECTION_REDUCER_BKGCATEGORIZERREDUCER2_H

#include "Reducer.h"

// from STL
#include <string>
#include <map>
#include <utility>

// from DooSelection
#include "dooselection/mctools/mcdecaymatrixreader/MCDecayMatrixReader.h"



/** \class dooselection::reducer::BkgCategorizerReducer2
 *  \brief Derived Reducer to generate background categories based on TupleToolMCDecayTree
 *
 *  This is a Reducer derived from Reducer. It implements
 *  Reducer::UpdateSpecialLeaves() to determine the background category of each
 *  given event. The BkgCategorizerReducer2 can be derived again to a custom made
 *  Reducer implementing sample specific requirements and tree leaves.
 *
 *  A virtual function BkgCategorizerReducer2::UpdateFurtherSpecialLeaves() can
 *  be implemented in the derived class to implement further complex leaves.
 *  This implementation follows the Template Method Design Pattern.
 **/

namespace dooselection {
  namespace reducer {
    
    enum modes {ChargesRel, ChargesIrrel, ChargesFinalStIrrel, ChargesInStIrrel};
    
    class BkgCategorizerReducer2 : virtual public Reducer {
    public:
      /**
       *  @brief Default constructor
       **/
      BkgCategorizerReducer2();
      
      /**
       *  @brief Virtual destructor
       **/
      virtual ~BkgCategorizerReducer2();

      /**
       *  @brief Setter for the mode that handles how decays are counted, that contain the same but charge conjugated particles, Default is ChargesIrrel, explanation below
       *  There are four Modes:
       *  ChargesRel:          Charges are relevant = taken into account (Decays, that are in any way charge conjuagted, are counted independently)
       *  ChargesIrrel:        Charges are completely irrelevant (Decays, that are in any way charge conjuagted, are added)
       *  ChargesFinalStIrrel: Charges in the final state are irrelevant, i.e. Decays with the same charge in the initial state but conjugated final state are added
       *  ChargesInStIrrel:    Charges in the initial state are irrelevant, i.e. Decays with same charges in the final state but conjugated initial state are added
       *
       *  Default is ChargesIrrel! This is the setter method that allows to change the mode!
       **/
      void set_mode(modes newmode) { mode_=newmode; }
      /**
       *  @brief Setter for the vector in the MCDecayMatrixReader that tells the Reader which Particles have to be ignored when evaluating the decay strings
       **/
      void set_vec_of_ignored_particles(std::vector<int> ids_of_ignored_particles) { decay_matrix_reader_.set_IDs_of_ignored_particles(ids_of_ignored_particles); }
      /**
       *  @brief Setter for decay_matrix_name_
       **/
      void set_decay_matrix_name(std::string name) { decay_matrix_name_ = name; }
      /**
       *  @brief Setter for background_category_leaf_
       **/
      void set_background_category_leaf(ReducerLeaf<Int_t>& leaf) { background_category_leaf_ = &leaf; }
      /**
       *  @brief Setter for decay_matrix_length_
       **/
      template<class T>
      void set_decay_matrix_length_leaf(const ReducerLeaf<T>& leaf) {
        decay_matrix_length_leaf_ = new ReducerLeaf<Int_t>(leaf.name(), leaf.title(), leaf.type(), leaf.tree());
        decay_matrix_length_leaf_->set_branch_address(leaf.branch_address());
      }
      /**
       *  @brief Setter for decay_depth_leaf_
       **/
      template<class T>
      void set_decay_depth_leaf(const ReducerLeaf<T>& leaf) {
        decay_depth_leaf_ = new ReducerLeaf<Int_t>(leaf.name(), leaf.title(), leaf.type(), leaf.tree());
        decay_depth_leaf_->set_branch_address(leaf.branch_address());
      }
      /**
       *  @brief Setter for decay_products_leaf_
       **/
      template<class T>
      void set_decay_products_leaf(const ReducerLeaf<T>& leaf) {
        decay_products_leaf_ = new ReducerLeaf<Int_t>(leaf.name(), leaf.title(), leaf.type(), leaf.tree());
        decay_products_leaf_->set_branch_address(leaf.branch_address());
      }
      
    protected:
      /**
       *  @brief Implementation of Reducer::PrepareSpecialBranches()
       *
       *  This function is called after copying to the interim tree and before
       *  starting the event loop.
       */
      virtual void PrepareSpecialBranches();
      
      /**
       *  @brief Implementation of Reducer::UpdateSpecialLeaves()
       *
       *  This function is called each iteration in the event loop of Reducer and
       *  is responsible of filling the background category into the tree.
       */
      virtual void UpdateSpecialLeaves();
      
      /**
       *  @brief Virtual function for preparation of further special leaves in derived classes
       *
       *  This function can be implemented in derived classes to prepare special
       *  leaves apart from the background category. It will be automatically called
       *  by Reducer.
       */
      virtual void PrepareFurtherSpecialLeaves() {}
      
      /**
       *  @brief Virtual function for updating of further special leaves in derived classes
       *
       *  This function can be implemented in derived classes to update special
       *  leaves apart from the background category. It will be automatically called
       *  in the event loop.
       */
      virtual void UpdateFurtherSpecialLeaves() {}
      
    private:
      /**
       *  @brief Leaf for background category to be inserted into tuple
       **/
      ReducerLeaf<Int_t>*               background_category_leaf_;
      /**
       *  @brief Pointer for contents of leaf background_category_leaf_ (for convienence)
       **/
      Int_t*                            background_category_lptr_;
      /**
       *  @brief Leaf for decay matrix length from TupleToolMCDecayTree
       **/
      ReducerLeaf<Int_t>*               decay_matrix_length_leaf_;
      /**
       *  @brief Pointer for contents of leaf decay_matrix_length_leaf_ (for performance)
       **/
      Int_t*                            decay_matrix_length_lptr_;
      /**
       *  @brief Leaf for decay depth from TupleToolMCDecayTree
       **/
      ReducerLeaf<Int_t>*               decay_depth_leaf_;
      /**
       *  @brief Leaf for decay products from TupleToolMCDecayTree
       **/
      ReducerLeaf<Int_t>*               decay_products_leaf_;
      /**
       *  @brief Static row array size from TupleToolMCDecayTree
       **/
      static const int      rows_ 		= 35;
      /**
       *  @brief Static column array size from TupleToolMCDecayTree
       **/
      static const int      columns_ 	= 25;
      /**
       *  @brief Array to be linked to matrix from TupleToolMCDecayTree
       **/
      Float_t               decay_matrix_[rows_][columns_];
      /**
       *  @brief Name of decay matrix in tuple
       **/
      std::string                       decay_matrix_name_;
      /**
       *  @brief Map for decay counting
       **/
      std::map<std::string,int>         decay_counter_;
      /**
       *  @brief Map to reference decay strings based on particle MC IDs to the related decay strings with particle names
       **/
      std::map<std::string,std::string> decay_string_referencer;
      /**
       *  @brief Sorted vector with background categories and string representation
       **/
      std::vector<std::pair<int,std::string> > decay_vector_;
      /**
       *  @brief Maximum number of decays to categorize
       **/
      int                               max_number_decays_;
      /**
       *  @brief MCDecayMatrixReader object, it provides functionality to read a given decay matrix to create particles with fully setted properties
       **/
      dooselection::mctools::mcdecaymatrixreader::MCDecayMatrixReader decay_matrix_reader_;
      /**
       *  @brief Run Mode, that handles how decays are counted, that contain the same but charge conjugated particles, Default is ChargesIrrel, explanation below
       *
       *  There are four Modes:
       *  ChargesRel:          Charges are relevant = taken into account (Decays, that are in any way charge conjuagted, are counted independently)
       *  ChargesIrrel:        Charges are completely irrelevant (Decays, that are in any way charge conjuagted, are added)
       *  ChargesFinalStIrrel: Charges in the final state are irrelevant, i.e. Decays with the same charge in the initial state but conjugated final state are added
       *  ChargesInStIrrel:    Charges in the initial state are irrelevant, i.e. Decays with same charges in the final state but conjugated initial state are added
       *
       *  Default is ChargesIrrel! There is a setter method to change the mode!
       *  
       **/
      modes mode_ ;

    };
    
  } // namespace reducer
} // namespace dooselection


#endif // DOOSELECTION_REDUCER_BKGCATEGORIZERREDUCER2_H

