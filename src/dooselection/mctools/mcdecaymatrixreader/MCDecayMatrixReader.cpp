// from STL
#include <cmath>
#include <iostream>

// from Project
#include "MCDecayMatrixReader.h"
#include "CondDBTranslator.h"


namespace dooselection {
namespace mctools {
namespace mcdecaymatrixreader {


MCDecayMatrixReader::MCDecayMatrixReader(){
  internal_translator_ = new CondDBTranslator();
}

MCDecayMatrixReader::MCDecayMatrixReader(std::vector<int> IDs_of_ingored_particles){
  internal_translator_ = new CondDBTranslator();
  ignoredparticles_ = IDs_of_ingored_particles;
}

MCDecayMatrixReader::~MCDecayMatrixReader(){
  delete internal_translator_;
}



Particle MCDecayMatrixReader::createDecayingParticle(Float_t* decaymatrix, int rows, int columns, int row, int column){
  Particle tempParticle = internal_translator_->CreateFullPropParticle(decaymatrix[row*columns+column]);
  if (column+1 < columns){
    for (int i=row+1; checkparticle(decaymatrix[i*columns+column]) !=1 && i<rows; i++){
      if (checkparticle(decaymatrix[i*columns+column+1]) == 1){
        tempParticle.AddDaughterParticle(createDecayingParticle(decaymatrix, rows, columns, i, column+1));
      }
    }
  }
  return tempParticle;
}


//Particle MCDecayMatrixReader::createMinimalDecayingParticle(Float_t* decaymatrix, int rows, int columns, int row, int column){
//  Particle tempParticle = internal_translator_->CreateMinimalParticle(decaymatrix[row*columns+column]);
//  if (column+1 < columns){
//    for (int i=row+1; checkparticle(decaymatrix[i*columns+column]) !=1 && i<rows; i++){
//      if (checkparticle(decaymatrix[i*columns+column+1]) == 1){
//        tempParticle.AddDaughterParticle(createMinimalDecayingParticle(decaymatrix, rows, columns, i, column+1));
//      }
//    }
//  }
//  return tempParticle;
//}
  
  Particle MCDecayMatrixReader::createMinimalDecayingParticle(Float_t decaymatrix[][15], int rows, int columns, int row, int column){
    Particle tempParticle = internal_translator_->CreateMinimalParticle(decaymatrix[row][column]);
    if (column+1 < columns){
      for (int i=row+1; checkparticle(decaymatrix[i][column]) !=1 && i<rows; i++){
        if (checkparticle(decaymatrix[i][column+1]) == 1){
          tempParticle.AddDaughterParticle(createMinimalDecayingParticle(decaymatrix, rows, columns, i, column+1));
        }
      }
    }
    return tempParticle;
  }


int MCDecayMatrixReader::checkparticle (Float_t decaymatrixelement){
  for(std::vector<int>::iterator it = ignoredparticles_.begin(); it != ignoredparticles_.end(); ++it){
    if (fabs(decaymatrixelement) == *it)
      return -1;
  }
  if ((fabs(decaymatrixelement) < 10000000) && (fabs(decaymatrixelement) > 1))
    return 1;
  else
    return 0;
}

  
} //namespace mcdecaymatrixreader
} //namespace mctools
} //namespace dooselection
