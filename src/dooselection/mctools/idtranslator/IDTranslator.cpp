#include "IDTranslator.h"

#include <sstream>

using namespace std;

namespace dooselection {
namespace mctools {

string IDTranslator::convertMCID(Float_t ID, bool real_names, bool only_abs){
  if (only_abs){
    ID = abs(ID);
  }
  if (!real_names){
    stringstream tmp;
    tmp << ID;
    string boing = tmp.str();
    return boing.c_str();
  }
  if      (ID ==     1) return "d";
  else if (ID ==    -1) return "d bar";
  else if (ID ==     2) return "u";
  else if (ID ==    -2) return "u bar";
  else if (ID ==     3) return "s";
  else if (ID ==    -3) return "s bar";
  else if (ID ==     4) return "c";
  else if (ID ==    -4) return "c bar";
  else if (ID ==     5) return "b";
  else if (ID ==    -5) return "b bar";
  else if (ID ==    11) return "electron";
  else if (ID ==   -11) return "anti-electron";
  else if (ID ==    12) return "e neutrino";
  else if (ID ==   -12) return "anti-e neutrino";
  else if (ID ==    13) return "muon";
  else if (ID ==   -13) return "anti-muon";
  else if (ID ==    14) return "muon neutrino";
  else if (ID ==   -14) return "anti-muon neutrino";
  else if (ID ==    15) return "tau";
  else if (ID ==   -15) return "anti-tau";
  else if (ID ==    16) return "tau neutrino";
  else if (ID ==   -16) return "anti-tau neutrino";
  else if (ID ==    21) return "gluon";
  else if (ID ==    22) return "gamma";
  else if (ID ==   111) return "pi0";
  else if (ID ==   115) return "a2 0";
  else if (ID ==  -115) return "a2 0 bar";
  else if (ID ==   211) return "pi+";
  else if (ID ==  -211) return "pi-";
  else if (ID ==   215) return "a2+";
  else if (ID ==  -215) return "a2-";
  else if (ID ==   221) return "eta";
  else if (ID ==   223) return "omega";
  else if (ID ==   225) return "f2";
  else if (ID ==   311) return "K0";
  else if (ID ==   315) return "K2* 0";
  else if (ID ==  -315) return "K2* 0 bar";
  else if (ID ==  -311) return "K0 bar";
  else if (ID ==   321) return "K+";
  else if (ID ==  -321) return "K-";
  else if (ID ==   113) return "rho0";
  else if (ID ==   213) return "rho+";
  else if (ID ==  -213) return "rho-";
  else if (ID ==   130) return "K0 long";
  else if (ID ==   310) return "K0 short";
  else if (ID ==   313) return "K*0";
  else if (ID ==  -313) return "K*0 bar";
  else if (ID ==   323) return "K*+";
  else if (ID ==  -323) return "K*-";
  else if (ID ==   331) return "eta'(958)";
  else if (ID ==   433) return "Ds*+";
  else if (ID ==  -433) return "Ds*-";
  else if (ID ==   443) return "J/Psi";
  else if (ID ==   411) return "D+";
  else if (ID ==  -411) return "D-";
  else if (ID ==   415) return "D2*(2460)+";
  else if (ID ==  -415) return "D2*(2460)-";
  else if (ID ==   421) return "D0";
  else if (ID ==  -421) return "D0 bar";
  else if (ID ==   431) return "Ds+";
  else if (ID ==  -431) return "Ds-";
  else if (ID ==   423) return "D*0";
  else if (ID ==  -423) return "D*0 bar";
  else if (ID ==   413) return "D*(2010)+";
  else if (ID ==  -413) return "D*(2010)-";
  else if (ID ==   425) return "D2*0";
  else if (ID ==  -425) return "D2*0 bar";
  else if (ID ==   445) return "chi c2(1P)";
  else if (ID ==   511) return "B0";
  else if (ID ==  -511) return "B0 bar";
  else if (ID ==   521) return "B+";
  else if (ID ==  -521) return "B-";
  else if (ID ==   531) return "Bs 0";
  else if (ID ==  -531) return "Bs 0 bar";
  else if (ID ==   513) return "B* 0";
  else if (ID ==  -513) return "B* 0 bar";
  else if (ID ==  2101) return "(ud)0";
  else if (ID ==  2203) return "(uu)1";
  else if (ID ==  2112) return "neutron";
  else if (ID == -2112) return "anti-neutron";
  else if (ID ==  2212) return "proton";
  else if (ID == -2212) return "anti-proton";
  else if (ID ==  2224) return "delta++";
  else if (ID == -2224) return "delta--";
  else if (ID ==  2214) return "delta+";
  else if (ID ==  2114) return "delta0";
  else if (ID == -2114) return "delta0 bar";
  else if (ID ==  3122) return "lambda";
  else if (ID == -3122) return "lambda bar";
  else if (ID ==  3212) return "sigma 0";
  else if (ID ==  4112) return "sigma c 0";
  else if (ID ==  4132) return "Xi c0";
  else if (ID ==  5122) return "Lambda_b";
  else if (ID == -5122) return "Lambda_b bar";
  else if (ID == -4132) return "Xi c0 bar";
  else if (ID == 10413) return "D1(2420)+";
  else if (ID ==-10413) return "D1(2420)-";
  else if (ID == 20213) return "a1(1260)+";
  else if (ID ==-20213) return "a1(1260)-";
  else if (ID == 20413) return "D1(H)+";
  else if (ID ==-20413) return "D1(H)-";
  else if (ID == 20443) return "chi c1(1P)";
  else if (ID == 9010221) return "f0(980)";
  else if (ID == 333) return "phi(1020)";
  else{
    stringstream tmp;
    tmp << ID;
    string boing = tmp.str();
    return boing.c_str();
  }
}

int IDTranslator::checkparticle (Float_t decaymatrixelement){
  if (abs(decaymatrixelement) == 22 || abs(decaymatrixelement) == 13 || abs(decaymatrixelement) == 14)
    return -1;
  else if ((abs(decaymatrixelement) < 10000000) && (abs(decaymatrixelement) > 1))
    return 1;
  else
    return 0;
}

string IDTranslator::makedecaystring (Float_t decaymatrix[][25], int rows, int columns, int row, int column, string decaystring, bool real_names, bool only_abs){
  Float_t ID = decaymatrix[row][column];
  
  for (int i=row+1; i<rows; i++){
    for (int j=column+1; j>0; j--){
      if (checkparticle(decaymatrix[i][j]) == 1){
        if (i==1)
          return makedecaystring(decaymatrix, rows, columns, i, j, decaystring + convertMCID(ID, real_names, only_abs) + " --> ", real_names, only_abs);
        if (j==column+1)
          return makedecaystring(decaymatrix, rows, columns, i, j, decaystring + "(" + convertMCID(ID, real_names, only_abs) + " --> ", real_names, only_abs);
        if (j==column)
          return makedecaystring(decaymatrix, rows, columns, i, j, decaystring + convertMCID(ID, real_names, only_abs) + " + ", real_names, only_abs);
        if (j<column)
          decaystring += convertMCID(ID, real_names, only_abs);
        for (int l=column-j; l>0; l--)   decaystring += ")";
        return makedecaystring(decaymatrix, rows, columns, i, j, decaystring + " + ",real_names, only_abs);
      }
    }
  }
  decaystring += convertMCID(ID, real_names, only_abs);
  for (int j=column-1; j>0; j--)   decaystring += ")";
  return decaystring;
}

} //namespace mctools
} //namespace dooselection
