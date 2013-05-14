#ifndef DOOSELECTION_MCTOOLS_MCDECAYMATRIXREADER_PARTICLE_H
#define DOOSELECTION_MCTOOLS_MCDECAYMATRIXREADER_PARTICLE_H

#include "TString.h"

// from STL
#include <vector>
#include <string>

// from Project

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
      

/** @class Particle
 *  @brief Particles can be created with this class using only their MCID number, the specific properties are taken from the CondDB using the CondDBTranslator class
 *
 *  The main purpose of this class is to be used by the MCDecayMatrixReader. 
 *
 *  ...Nevertheless, if somebody wants to know some specific properties of a particle 
 *  (like its mass) and is too lazy to look in up in the pdf booklet,
 *  he/she can create the particle with the default constructor using only the mcid. 
 *  The properties can then be read out by using the different getter methods. 
 *  
 *  If many particles are produced (like in the MCDecayMatrixReader), it is more efficient to 
 *  create one CondDBTranslator instance and create the particles using the  Particle CondDBTranslator::CreateParticle(int ID) method.
 *  The latter is more efficient, because the Cond DB file is loaded only once instead of being loaded again and again for each particle.
 *
 *  Besides providing particle properties this class conatins a vector of the daughter particles,
 *  which can be filled manually. The MCDecayMatrixReader fills this vector automatically 
 *  by extracting the decay information from the MC truth information. 
 *  Different methods are implemented to read out the decay of a particle as strings.
 *
 *  *  Finally one can compare decay strings, and check, if a given decay string describes
 *  -> the same decay
 *  -> the charge conjugated decay
 *  -> a decay with the same mother particle but charge conjugated final state
 *  -> a decay with charge conjugated mother particle but same final state
 *
 *  @author Ulrich Eitschberger
 */
class Particle {


  
public:
  /**
   *  @brief Default Constructor for Particle, which uses the given MC ID to set all particle attributes
   *
   *  This constructor uses the CondDBTranslator to set all of its attributes. Daughter particles are not added.
   *  If many particles are needed, it is more efficient to create them directly with the CondDBTranslator using
   *  the CondDBTranslator::CreateParticle(int ID) method, which returns a fully created particle.
   *
   */
  Particle(int mcid);

  /**
   *  @brief This Constructor creates an empty particle, it used by the CondDBTranslator
   *
   *  This constructor is used by the the CondDBTranslator::CreateFullPropParticle(int ID) / CondDBTranslator::CreateMinimalParticle(int ID) methods.
   *  It does not set any attributes by itself, this is done inside CreateParticle.
   *
   */
  Particle();
  
  /**
   *  @brief Destructor for Particle
   *
   *  Detailed description.
   *
   */
  ~Particle();
  
  /**
   *  @brief Getter method for the name of the particle
   *
   *  Returns the real name of the particle as a string instead of the MC ID, which is a float
   */
  std::string name();
  
  /**
   *  @brief Getter method for the MC ID of the particle
   *
   *  Returns the MC ID of the particle as a float
   */
  int mc_id();

  /**
   *  @brief Getter method for the charge of the particle
   *
   *  Returns the charge of the particle as a string
   */
  std::string charge();
  
  /**
   *  @brief Getter method for the mass of the particle
   *
   *  Returns the mass of the particle as a string
   */
  std::string mass();
  
  /**
   *  @brief Getter method for the (c*)Tau/Gamma of the particle
   *
   *  Returns the (c*)Tau/Gamma of the particle as a string
   */
  std::string ctaugamma();
  
  /**
   *  @brief Getter method for the Max Width of the particle
   *
   *  Returns the Max Width of the particle as a string
   */
  std::string maxwidth();
  
  /**
   *  @brief Getter method for the EvtGen Name of the particle
   *
   *  Returns the EvtGen Name of the particle as a string
   */
  std::string evtgenname();
  
  /**
   *  @brief Getter method for the Pythia ID of the particle
   *
   *  Returns the Pythia ID of the particle as a string
   */
  std::string pythiaID();
  
  
  /**  @brief Getter method for the name of the corresponding antiparticle
    *
    *  Returns the real name of the conjugated particle as a string 
    */
  std::string antiparticlename();
  
  /**
   *  @brief Setter method for the name of the particle
   *
   */
  void set_name(std::string new_name);
  
  /**
   *  @brief Setter method for the MC ID of the particle
   *
   */
  void set_mc_id(int new_mc_id);
  
  /**
   *  @brief Setter method for the charge of the particle
   *
   */
  void set_charge(std::string new_charge);
  
  /**
   *  @brief Setter method for the mass of the particle
   *
   */
  void set_mass(std::string new_mass);
  
  /**
   *  @brief Setter method for the (c*)Tau/Gamma of the particle
   *
   */
  void set_ctaugamma(std::string new_maxwidth);
  
  /**
   *  @brief Setter method for the Max Width of the particle
   *
   */
  void set_maxwidth(std::string new_maxwidth);
  
  /**
   *  @brief Setter method for the EvtGen Name of the particle
   *
   */
  void set_evtgenname(std::string new_evtgenname);
  
  /**
   *  @brief Setter method for the Pythia ID of the particle
   *
   */
  void set_pythiaID(std::string new_pythiaID);
  
  
  /**  @brief Setter method for the name of the corresponding antiparticle
   *
   */
  void set_antiparticlename(std::string new_antiparticlename);

  
  /**
   *  @brief Getter method for the conjugated MC ID of the particle
   *
   *  Returns the conjuagted MC ID of the particle as a float, this is needed internally
   */
  int GetConjugatedMCID();

  
  /**
   *  @brief Getter method for the absolute value of the MC ID of the particle
   *
   *  Returns the absolute value of the MC ID of the particle as a float, this is needed internally
   */
  int GetAbsoluteValueOfMCID();
  
  /**
   *  @brief Method to get the direct decay products of this particle, delivered as a string
   *
   *  This method returns a string that only contains the direct daughter particles.
   *  If the complete decay is needed, that contains the decay products of the direct daughter particles
   *  as well, one should use the method GetCompleteDecayString().
   *  
   *  Example: B0(D- pi+)
   */
  std::string GetDecay();
  
  /**
   *  @brief Method to get the complete decay including all final state particles
   *
   *  This method returns a string that contains the direct daughter particles as well as their
   *  decay products and so on.
   *  
   *  Example: B0(D-(K+ pi- pi-) pi+)
   */
  std::string GetCompleteDecay();

  /**
   *  @brief Method to get the conjugated direct decay products of this particle, delivered as a string
   *
   *  This method returns a string that only contains the direct daughter particles.
   *  Both the initial state particle and the final state particles are conjugated.
   *  If the complete decay is needed, that contains the decay products of the direct daughter particles
   *  as well, one should use the method GetConjugatedCompleteDecay().
   *
   *  Example: The original Decay is B0(D- pi+), so you get B0 bar(D+ pi-)
   */
  std::string GetConjugatedDecay();
  
  /**
   *  @brief Method to get the conjugated complete decay including all final state particles
   *
   *  This method returns a string that contains the conjugated direct daughter particles as well as their
   *  conjuagted decay products and so on. The initial particle is conjugated as well.
   *
   *  Example: The original Decay is B0(D-(K+ pi- pi-) pi+), so you get B0 bar(D+(K- pi+ pi+) pi-)
   */
  std::string GetConjugatedCompleteDecay();
  
  /**
   *  @brief Method to get the decay including only the direct daughter particles, but with conjugated inital state
   *
   *  This method returns a string that contains the decay with a conjugated initial state particle.
   *  Direct daughter particles are not conjugated.
   *
   *  Example: The original Decay is B0(D- pi+), so you get B0 bar(D- pi+)
   */
  std::string GetConjugatedInitialStateDecay();

  /**
   *  @brief Method to get the complete decay including all final state particles, but with conjugated inital state
   *
   *  This method returns a string that contains the complete decay with a conjugated initial state particle.
   *  Both direct daughter particles and their decay products are not conjugated.
   *
   *  Example: The original Decay is B0(D-(K+ pi- pi-) pi+), so you get B0 bar(D-(K+ pi- pi-) pi+)
   */
  std::string GetConjugatedInitialStateCompleteDecay();
  
  /**
   *  @brief Method to get the decay including only the (conjugated) direct daughter particles (i.e. only the final state is conjugated)
   *
   *  This method returns a string that contains the decay with a conjugated final state.
   *  The further decay products of the direct daughter particles are not included.
   *  The initial state is not conjugated.
   *
   *  Example: The original Decay is B0(D- pi+), so you get B0(D+ pi-)
   */
  std::string GetConjugatedFinalStateDecay();

  /**
   *  @brief Method to get the complete decay including all final state particles, but with a conjugated final state
   *
   *  This method returns a string that contains the complete decay with a conjugated final state.
   *  Both direct daughter particles and their decay products are conjugated.
   *  The initial state is not conjugated.
   *
   *  Example: The original Decay is B0(D-(K+ pi- pi-) pi+), so you get B0(D+(K- pi+ pi+) pi-)
   */
  std::string GetConjugatedFinalStateCompleteDecay();

  /**
   *  @brief Method to get the complete decay consisting of IDs instead of names
   *
   *  This method returns a string that contains the direct daughter particles as well as their
   *  decay products and so on, but each particle is included with its ID intead of its name.
   *
   *  Example: The original Decay is B0(D-(K+ pi- pi-) pi+), so you get 511(411(-321 211 211) -211)
   */
  std::string GetCompleteMCIDDecay();

  /**
   *  @brief Method to get the complete decay consisting of the absolute values of particle IDs instead of names
   *
   *  This method returns a string that contains the direct daughter particles as well as their
   *  decay products and so on, but each particle is included with the absolute value of its ID intead of its name.
   *
   *  This method is needed to compare decays ignoring their charges.
   *
   *  Example: The original Decay is B0(D-(K+ pi- pi-) pi+), so you get 511(411(321 211 211) 211)
   */
  std::string GetCompleteAbsMCIDDecay();

  
  /**
   *  @brief Method to get the complete decay consisting of the name of the mother particle and the absolute values of particle IDs instead of names of the daughter particles
   *
   *  This method returns a string that contains the direct daughter particles as well as their
   *  decay products and so on, but each particle is included with the absolute value of its ID intead of its name.
   *
   *  
   *  This method is needed to compare decays ignoring the charges of their daughter particles.
   *
   *  Example: The original Decay is B0(D-(K+ pi- pi-) pi+), so you get B0(411(321 211 211) 211)
   */
  std::string GetCompleteFinalStateAbsMCIDDecay();

  
    /**
   *  @brief Method to compare two decays
   *
   *  This Method compares the decay of the particle to the one given as a string.
   *  Two Decays are seen as equal, if they have exactly the same decay products of each particle.
   *  Decays, that contain charge conjugated particles in any way are not seen as equal.
   */
  bool IsEqualDecay(std::string ComparativeDecay);
  
  /**
   *  @brief Method to compare two decays
   *
   *  This Method compares the decay of the particle to the one given as a string.
   *  True is returned, if they describe the same but charge charge conjugated decays.
   *
   */
  bool IsConjugatedDecay(std::string ComparativeDecay);

  /**
   *  @brief Method to compare two decays
   *
   *  This Method compares the decay of the particle to the one given as a string.
   *  Two Decays are seen as equal, if the have the same initial sate particle
   *  (with the same charge) but a conjugated final state.
   *
   */
  bool IsDecayWithConjugatedFinalState(std::string ComparativeDecay);

  /**
   *  @brief Method to compare two decays
   *
   *  This Method compares the decay of the particle to the one given as a string.
   *  Two Decays are seen as equal, if the have the same initial sate particle
   *  (with the same charge) but a conjugated initial state.
   *
   */
  bool IsDecayWithConjugatedInitialState(std::string ComparativeDecay);

  /**
   *  @brief Method to compare two decays
   *
   *  This Method compares the decay of the particle to the one given as a string.
   *  True is returned, if one of the following methods returns true
   *
   *  IsEqualDecay
   *  IsConjugatedDecay
   *  IsDecayWithConjugatedFinalState
   *  IsDecayWithConjugatedInitialState
   *
   */
  bool IsComparableDecay(std::string ComparativeDecay);
  
  /**
   *  @brief Method to add an daughter particle, a direct decay product
   *
   *  Adds an daughter particle at the end of the daughter particles vector
   *
   */
  void AddDaughterParticle(Particle new_particle);
  
  
  //  0:particleName,  1:pdgID,  2:charge,  3:mass,  4:cTauGamma,  5:maxWidth,  6:evtGenName,  7:pythiaID,  8:antiparticlename

  
private:
  /**
   *  @brief Float containing the MC ID of the particle
   *
   */
  int mc_id_;

  /**
   *  @brief String containing the real name of the particle
   *
   */
  std::string name_;

  /**
   *  @brief String containing the charge of the particle
   *
   */
  std::string charge_;

  /**
   *  @brief String containing the mass of the particle, the unit is included in the string
   *
   */
  std::string mass_;

  /**
   *  @brief String containing the real name of the particle
   *
   */
  std::string ctaugamma_;

  /**
   *  @brief String containing the max width the particle
   *
   */
  std::string maxwidth_;

  /**
   *  @brief String containing the evt Gen name of the particle
   *
   */
  std::string evtgenname_;

  /**
   *  @brief String containing the pythia ID of the particle
   *
   */
  std::string pythiaID_;

  /**
   *  @brief String containing the name of the corresponding antiparticle
   *
   */
  std::string antiparticlename_;
  
  
  /**
   *  @brief Vector, that contains the direct daugther particles
   *
   *  If a daughter particle decayed, its own decay products are saved in its own daughter particles vector
   */
  std::vector<Particle> daugther_particles_;
  
};
  
} //namespace mcdecaymatrixreader
} //namespace mctools
} //namespace dooselection

#endif // DOOSELECTION_MCTOOLS_MCDECAYMATRIXREADER_PARTICLE_H
