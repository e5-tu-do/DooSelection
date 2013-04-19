#include "MCDecayMatrixReader.h"

// from STL


// from Project
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



Particle MCDecayMatrixReader::createDecayingParticle(Float_t decaymatrix[][15], int rows, int columns, int row, int column){
  Particle tempParticle = internal_translator_->CreateFullPropParticle(decaymatrix[row][column]);
  if (column+1 < columns){
    for (int i=row+1; checkparticle(decaymatrix[i][column]) !=1 && i<rows; i++){
      if (checkparticle(decaymatrix[i][column+1]) == 1){
        tempParticle.AddDaughterParticle(createDecayingParticle(decaymatrix, rows, columns, i, column+1));
      }
    }
  }
  return tempParticle;
}


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
  if (abs(decaymatrixelement) == 22 || abs(decaymatrixelement) == 13 || abs(decaymatrixelement) == 14)
    return -1;
  else if ((abs(decaymatrixelement) < 10000000) && (abs(decaymatrixelement) > 1))
    return 1;
  else
    return 0;
}

  
} //namespace mcdecaymatrixreader
} //namespace mctools
} //namespace dooselection
