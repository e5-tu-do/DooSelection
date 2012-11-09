#ifndef DOOSELECTION_MCTOOLS_IDTRANSLATOR_IDTRANSLATOR_H
#define DOOSELECTION_MCTOOLS_IDTRANSLATOR_IDTRANSLATOR_H

#include <string>
#include <cmath>

#include "TROOT.h"

/**
 * @namespace dooselection::mctools
 * @brief Tools for Monte Carlo (MC)
 *
 * This namespace contains tools used together with Monte Carlo (MC) data. That
 * includes tools used to extract and analyse MC information stored in tuples.
 */

/**
 * @namespace dooselection::mctools::IDTranslator
 * @brief IDTranslator analyzing decay matrices in MC tuples
 *
 * This namespace contains tools used together with Monte Carlo (MC) data. That
 * includes tools used to extract and analyse MC information stored in tuples.
 */

namespace dooselection {
namespace mctools {

namespace IDTranslator{
  std::string convertMCID(Float_t ID, bool real_names, bool only_abs);
  int checkparticle (Float_t decaymatrixelement);
  std::string makedecaystring (Float_t decaymatrix[][25], int rows, int columns, int row, int column, std::string decaystring, bool real_names, bool only_abs);
}

} //namespace mctools
} //namespace dooselection

#endif //DOOSELECTION_MCTOOLS_IDTRANSLATOR_IDTRANSLATOR_H