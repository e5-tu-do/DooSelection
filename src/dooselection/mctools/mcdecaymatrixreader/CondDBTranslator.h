#ifndef DOOSELECTION_MCTOOLS_MCDECAYMATRIXREADER_CondDBTranslator_H
#define DOOSELECTION_MCTOOLS_MCDECAYMATRIXREADER_CondDBTranslator_H

#include "TString.h"

// from STL
#include <map>
#include <string>


// from Project
#include "Particle.h"


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



/** @class CondDBTranslator
 *  @brief This class reads the particle table file derived from the cond db
 *
 *  This class fullfills mainly two purposes
 *  -> translating a given particle MC ID to all related particle properties extracted from the cond db particle table, handing it always back as a string
 *  -> the creation of a particle from a given particle MC ID, setting all of its properties (used by the background evaluator)
 *
 *  @author Ulrich Eitschberger
 */
class CondDBTranslator {
  
  
  
public:
  /**
   *  @brief Default constructor for CondDBTranslator
   *
   *  Reads the particle MC ID table and creates the two maps
   */
  CondDBTranslator();
  
  /**
   *  @brief Destructor of CondDBTranslator
   *
   *  Detailed Description
   *
   */
  ~CondDBTranslator();
  
  /**
   *  @brief Method to translate a given MC ID of a particle into its name, which is returned as a string
   *
   */
  std::string TranslateIDintoName(int ID);

  /**
   *  @brief Method to translate a given MC ID of a particle into its charge, which is returned as a string
   *
   */
  std::string TranslateIDintoCharge(int ID);
  
  /**
   *  @brief Method to translate a given MC ID of a particle into its mass, which is returned as a string
   *
   */
  std::string TranslateIDintoMass(int ID);
  
  /**
   *  @brief Method to translate a given MC ID of a particle into its (c*)Tau/Gamma, which is returned as a string
   *
   */
  std::string TranslateIDintoCTauGamma(int ID);
  
  /**
   *  @brief Method to translate a given MC ID of a particle into its Max Width, which is returned as a string
   *
   */
  std::string TranslateIDintoMaxWidth(int ID);
  
  /**
   *  @brief Method to translate a given MC ID of a particle into its Max Width, which is returned as a string
   *
   */
  std::string TranslateIDintoEvtGenName(int ID);
  
  /**
   *  @brief Method to translate a given MC ID of a particle into its Pythia ID, which is returned as a string
   *
   */
  std::string TranslateIDintoPythiaID(int ID);
  
  /**
   *  @brief Method to translate a given MC ID of a particle into its antiparticles name, which is returned as a string
   *
   */
  std::string TranslateIDintoAntiparticleName(int ID);
  
  /**
   *  @brief Method creates the particle that corresponds to the given ID, all properties are set
   *
   */
  Particle CreateFullPropParticle(int ID);

  
  /**
   *  @brief Method creates the particle that corresponds to the given ID, only the name, the MCID and the antiparticle name are set
   *
   */
  Particle CreateMinimalParticle(int ID);

  
private:
  /**
   *  @brief All complete lines from the particle table are saved in this map and can be accessed through their related mc ids  
   *
   */
  std::map<int, std::string> table_lines_;


};

} //namespace mcdecaymatrixreader
} //namespace mctools
} //namespace dooselection


#endif // DOOSELECTION_MCTOOLS_MCDECAYMATRIXREADER_CondDBTranslator_H
