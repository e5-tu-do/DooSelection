#ifndef DOOSELECTION_MCTOOLS_MCDECAYMATRIXREADER_MCDECAYMATRIXREADER_H
#define DOOSELECTION_MCTOOLS_MCDECAYMATRIXREADER_MCDECAYMATRIXREADER_H


// from STL


// from project
#include "particle.h"
#include "CondDBTranslator.h"



/**
 * @namespace dooselection::mctools
 * @brief Tools for Monte Carlo (MC)
 *
 * This namespace contains tools used together with Monte Carlo (MC) data. That
 * includes tools used to extract and analyse MC information stored in tuples.
 */

namespace dooselection {
namespace mctools {
  
  /**
   * @namespace dooselection::mctools::mcdecaymatrixreader
   * @brief MCDecayMatrixReader analyzing decay matrices in MC tuples
   *
   * This namespace contains tools used together with Monte Carlo (MC) data. That
   * includes tools used to extract and analyse MC information stored in tuples.
   */

  
namespace mcdecaymatrixreader {



/** @class MCDecayMatrixReader
 *  @brief This class reads a given decay matrix and creates strings from the extracted decays
 *
 *  This class is able to read a given decay matrix that is extracted from a MC tuple.
 *  Furthermore it provides the functionality to use decay strings to count decays.
 * 
 *  It is possible to ingore several particles by using the constructor MCDecayMatrixReader(std::vector<int> IDs_of_ingored_particles).
 *  The vector has to contain the IDs of those particles, that shall be ignored.
 *
 *
 *  @author Ulrich Eitschberger
 */
class MCDecayMatrixReader {
  
  
public:
  
  /**
   *  @brief Default constructor for MCDecayMatrixReader, does not ingore specific particles.
   *
   */
  MCDecayMatrixReader();

  /**
   *  @brief Constructor for MCDecayMatrixReader that enables ingoring specific particles, by handing their IDs inside a vector.
   *
   */
  MCDecayMatrixReader(std::vector<int> IDs_of_ingored_particles);

  /**
   *  @brief Destructor for MCDecayMatrixReader
   *
   *  Main purpose is to delete the instance of the CondDBTranslator object.
   *
   */
  ~MCDecayMatrixReader();
  
  
  /**
   *  @brief This methods checks, if a particle ID is valid or not
   *
   *  If certain particles shall be excluded, than they are treated as if they weren't there.
   *
   */
  int checkparticle (Float_t decaymatrixelement);
  
  
  /**
   *  @brief This method creates a particle (including all of its daugther particles) from the given array derived from the decay matrix.
   *
   *  
   *
   */
  Particle createDecayingParticle(Float_t *decaymatrix, int rows, int columns, int row, int column);

  
  /**
   *  @brief This method creates a particle (including all of its daugther particles) by reading the given array that is derived from the tuple.
   *
   *  The only difference of the 'minimal' method is that it does not save all possible particle properties from the CondDBTranslator for each particle.
   *
   */
  //Particle createMinimalDecayingParticle(Float_t *decaymatrix, int rows, int columns, int row, int column);
  Particle createMinimalDecayingParticle(Float_t decaymatrix[][15], int rows, int columns, int row, int column);

  
private:
  
  /**
   *  @brief Internal Instance of the CondDBTranslator, it is only created once to save memory and time
   *
   *  If the CondDBTranslator object would be created multiple times, it would read out the CondDBparticletable again and again. This would be very ineffective and unnecessary.
   *
   *
   */
  CondDBTranslator *internal_translator_;
  
  /**
   *  @brief Vector, that stores the IDs of the particles that shall be ignored
   *
   *
   */
  std::vector<int> ignoredparticles_;

};
  
} //namespace mcdecaymatrixreader
} //namespace mctools
} //namespace dooselection


#endif // DOOSELECTION_MCTOOLS_MCDECAYMATRIXREADER_MCDECAYMATRIXREADER_H
