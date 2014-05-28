#include "BkgCategorizerReducer2.h"

// from STL
#include <string>
#include <map>
#include <utility>

// from ROOT
#include "TMath.h"
#include "TCut.h"
#include "TStopwatch.h"

// from DooCore
#include <doocore/io/MsgStream.h>

// From project
#include "dooselection/mctools/mcdecaymatrixreader/MCDecayMatrixReader.h"
#include "dooselection/mctools/mcdecaymatrixreader/Particle.h"

using std::pair;
using namespace doocore::io;
using namespace dooselection::mctools;

namespace dooselection {
  namespace reducer {
    
    BkgCategorizerReducer2::BkgCategorizerReducer2() :
    background_category_leaf_(NULL),
    background_category_lptr_(NULL),
    decay_matrix_length_leaf_(NULL),
    decay_matrix_length_lptr_(NULL),
    decay_depth_leaf_(NULL),
    decay_counter_(),
    max_number_decays_(40),
    mode_(ChargesIrrel)
    {}
    
    BkgCategorizerReducer2::~BkgCategorizerReducer2() {}
    
    void BkgCategorizerReducer2::PrepareSpecialBranches() {
      sinfo << "Starting analysis of MC associated decays." << endmsg;
      
      if (decay_matrix_length_leaf_ == NULL && decay_matrix_length_leaf_ != NULL) {
        serr << "Decay matrix length not set! BkgCategorizerReducer2 can no longer work. Set via BkgCategorizerReducer2::set_decay_matrix_length_leaf(...)!" << endmsg;
      }
      
      TBranch* br_matrix = interim_tree_->GetBranch(decay_matrix_name_.c_str());
      if (br_matrix != NULL) {
        TStopwatch sw;
        sw.Start();
        
        decay_matrix_length_lptr_ = (Int_t*)decay_matrix_length_leaf_->branch_address();
        decay_matrix_length_lptr_ = (Int_t*)decay_matrix_length_leaf_->branch_address();
        TBranch* br_depth  = interim_tree_->GetBranch(decay_depth_leaf_->name());
        
        interim_tree_->SetBranchStatus("*", false);
        interim_tree_->SetBranchStatus(br_depth->GetName(), true);
        interim_tree_->SetBranchStatus(br_matrix->GetName(), true);
        
        
        //Get column sitze to be able to dynamically allocate memory for decay_matrix_
        for (Int_t ev = 0; ev < interim_tree_->GetEntries(); ev++) {
          interim_tree_->GetEntry(ev);
          TLeaf *l = br_matrix->GetLeaf("B0_DECAYMTRX");
          if (!(decay_depth_leaf_->GetValue() < 1)){
            columns_ = l->GetLen()/decay_matrix_length_leaf_->GetValue();
            break;
          }
        }

        //Allocate Memory and set Branchadress of decay_matrix_
        decay_matrix_ = new Float_t[columns_*rows_];
        br_matrix->SetAddress(decay_matrix_);


        //Create the temporary particle, which is overwritten for each event in the event loop below by the decay_matrix_reader_
        //After the particle is overwritten the DecayStrings are created using methods from the particle class
        dooselection::mctools::mcdecaymatrixreader::Particle tempParticle;

        
        // Fill the map with decay-strings
        for (Int_t ev = 0; ev < interim_tree_->GetEntries(); ev++) {
          interim_tree_->GetEntry(ev);
          // Debug output to check content of decay matrices
          //          std::cout << "DecayMatrix Length: " << decay_matrix_length_leaf_->GetValue() << ", DecayMatrix Depth: " << decay_depth_leaf_->GetValue() << ", Motherparticle: " << decay_matrix_[0][0] << std::endl << std::endl;
          if (!(decay_depth_leaf_->GetValue() < 1)){
            
//            //Debug output to print decay matrices on the terminal
//            for (int i=0; i<*decay_matrix_length_lptr_; ++i){
//              for (int j=0; j<columns_; ++j){
//                std::cout << decay_matrix_[i*columns_+j] << " ";
//              }
//                std::cout << std::endl;
//            }
            
            
            tempParticle = decay_matrix_reader_.createMinimalDecayingParticle(decay_matrix_, *decay_matrix_length_lptr_, columns_, 0, 0);
            std::string decay_string = tempParticle.GetCompleteDecay();
            std::map<std::string,int>::const_iterator iter = decay_counter_.find(decay_string);
            
            if (mode_ == ChargesRel){
              if (iter != decay_counter_.end()){
                decay_counter_[decay_string]++;
              }
              else {
                decay_counter_.insert (pair<std::string,int>(decay_string,1));
                decay_string_referencer.insert(pair<std::string, std::string>(decay_string,decay_string));
              }
            }
            else if (mode_ == ChargesIrrel){
              std::string decay_string_absmcid = tempParticle.GetCompleteAbsMCIDDecay();
              iter = decay_counter_.find(decay_string_absmcid);
              if (iter != decay_counter_.end()){
                decay_counter_[decay_string_absmcid]++;
              }
              else {
                decay_counter_.insert (pair<std::string,int>(decay_string_absmcid,1));
                decay_string_referencer.insert(pair<std::string, std::string>(decay_string_absmcid,decay_string));
              }
            }
            else if (mode_ == ChargesFinalStIrrel){
              std::string decay_string_finalstate_absmcid = tempParticle.GetCompleteFinalStateAbsMCIDDecay();
              iter = decay_counter_.find(decay_string_finalstate_absmcid);
              if (iter != decay_counter_.end()){
                decay_counter_[decay_string_finalstate_absmcid]++;
              }
              else {
                decay_counter_.insert (pair<std::string,int>(decay_string_finalstate_absmcid,1));
                decay_string_referencer.insert(pair<std::string, std::string>(decay_string_finalstate_absmcid,decay_string));
              }
            }
            else if (mode_ == ChargesInStIrrel){
              std::string decay_string_initialstate_conjugated = tempParticle.GetConjugatedInitialStateCompleteDecay();
              iter = decay_counter_.find(decay_string_initialstate_conjugated);
              if (iter != decay_counter_.end()){
                decay_counter_[decay_string_initialstate_conjugated]++;
              }
              else {
                decay_counter_.insert (pair<std::string,int>(decay_string_initialstate_conjugated,1));
                decay_string_referencer.insert(pair<std::string, std::string>(decay_string_initialstate_conjugated,decay_string));
              }
            }
          }
        }
        
        interim_tree_->SetBranchStatus("*", true);
        
        
        
        
        
        
        //Sort Decays and fill two Vectors, one with the most appearing Decays and one with their count value
        std::map<std::string,int>::const_iterator iter;
        std::map<std::string,std::string>::iterator iter_decay_referencer;
        
        sinfo << "Finished analysing most common decays: " << endmsg;
        for (int i=0; i < max_number_decays_ && i<decay_string_referencer.size(); ++i) {
          std::string max_key = "";
          int max_counts = 0;
          for (iter=decay_counter_.begin(); iter!=decay_counter_.end(); iter++) {
            if (iter->second > max_counts) {
              max_counts = iter->second;
              max_key    = iter->first;
            }
          }
          iter_decay_referencer = decay_string_referencer.find(max_key);
          
          decay_vector_.push_back(std::pair<int,std::string>(decay_counter_[max_key],max_key));
          
          sinfo << "Number: " << i+1 << ", Decay: " << iter_decay_referencer->second << ", Count: " << decay_vector_.at(i).first << endmsg;
          decay_counter_.erase(max_key);
        }

                
        background_category_lptr_ = (Int_t*)background_category_leaf_->branch_address();
        sinfo << "Background categories according to this table will be put into leaf " << background_category_leaf_->name() << endmsg;
        
        double time = sw.RealTime();
        sinfo << "Background analysis took " << time << " s." << endmsg;
      } else {
        swarn << "Decay matrix not found. Skipping analysis." << endmsg;
      }
      
      PrepareFurtherSpecialLeaves();
    }
    
    void BkgCategorizerReducer2::UpdateSpecialLeaves() {
      if (decay_depth_leaf_ != NULL) {
        if (!(decay_depth_leaf_->GetValue() < 1)) {
          dooselection::mctools::mcdecaymatrixreader::Particle tempParticle;
          std::string decaystring = "";
          
          tempParticle = decay_matrix_reader_.createMinimalDecayingParticle(decay_matrix_, *decay_matrix_length_lptr_, columns_, 0, 0);
          
          if (mode_ == ChargesRel) {
            decaystring = tempParticle.GetCompleteDecay();
          }
          else if (mode_ == ChargesIrrel) {
            decaystring = tempParticle.GetCompleteAbsMCIDDecay();
          }
          else if (mode_ == ChargesFinalStIrrel) {
            decaystring = tempParticle.GetCompleteFinalStateAbsMCIDDecay();
          }
          else if (mode_ == ChargesInStIrrel) {
            decaystring = tempParticle.GetConjugatedInitialStateCompleteDecay();
          }
          
          int j = 0;
          while(j < max_number_decays_ && j < decay_vector_.size() && decaystring != decay_vector_.at(j).second){
            j++;
          }
          if (j==max_number_decays_){
            *background_category_lptr_ = 50;
          }
          else {
            *background_category_lptr_ = j+1;
          }
        } else {
          *background_category_lptr_ = 100;
        }
      }
      UpdateFurtherSpecialLeaves();
    }
    
  } // namespace reducer
} // namespace dooselection

