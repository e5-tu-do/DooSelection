// from STL
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

// from ROOT
#include "TString.h"

// from Project
#include "CondDBTranslator.h"


namespace dooselection {
namespace mctools {
namespace mcdecaymatrixreader {


CondDBTranslator::CondDBTranslator(){
  TString pathPart1(getenv ("DOOSELECTIONSYS"));
  TString path = pathPart1 + "/include/dooselection/mctools/mcdecaymatrixreader/CondDB_particle_table.txt";

  std::ifstream particle_table_file;
  particle_table_file.open(path);

  std::string line = "";
  std::string mc_id_string = "";
  int mc_id = 0;

  if(particle_table_file.is_open()){
    while(particle_table_file.good()){
      getline(particle_table_file,line);
      mc_id_string = line.substr(21,13);
      std::stringstream temp;
      temp << mc_id_string;
      temp >> mc_id;
      table_lines_.insert (std::pair<int,std::string>(mc_id,line));
    }
    particle_table_file.close();
  }
  else {
    std::cout << "CondDBTranslator: Unable to open the particle table file" << std::endl;
  }
}



CondDBTranslator::~CondDBTranslator(){
  
}

std::string CondDBTranslator::GetCorrespondingLine(int ID){
  std::map<int, std::string>::const_iterator iter = table_lines_.find(ID);
  if(iter==table_lines_.end()){
    std::cout << "<<<<<<<<<< WARNING: searched particle ID " << ID << " is NOT included in the CondDB Particle Table >>>>>>>>>" << std::endl;
    return "";
  }
  else {
    return iter->second;
  }
}

  
std::string CondDBTranslator::GetInfoFromCondDBLine(std::string CondDBLine, int Startpoint){
  if(CondDBLine==""){
    return CondDBLine;
  }
  else{
    std::string PartialStringFromLine = "";
    for(int i=Startpoint; CondDBLine.substr(i,1) != "|"; ++i){
      if(CondDBLine.substr(i,1) != " "){
        PartialStringFromLine += CondDBLine.at(i);
      }
    }
    return PartialStringFromLine;
  }
  
}

std::string CondDBTranslator::TranslateIDintoName(int ID){
  std::string particleName = GetInfoFromCondDBLine(GetCorrespondingLine(ID), 2);
  if(particleName==""){
    std::ostringstream temp;
    temp << ID;
    std::string ID_string(temp.str());
    particleName = ID_string;
  }
  return particleName;
}
  

std::string CondDBTranslator::TranslateIDintoCharge(int ID){
  return GetInfoFromCondDBLine(GetCorrespondingLine(ID), 36);
}


std::string CondDBTranslator::TranslateIDintoMass(int ID){
  return GetInfoFromCondDBLine(GetCorrespondingLine(ID), 43);
}


std::string CondDBTranslator::TranslateIDintoCTauGamma(int ID){
  return GetInfoFromCondDBLine(GetCorrespondingLine(ID), 57);
}


std::string CondDBTranslator::TranslateIDintoMaxWidth(int ID){
  return GetInfoFromCondDBLine(GetCorrespondingLine(ID), 74);
}


std::string CondDBTranslator::TranslateIDintoEvtGenName(int ID){
  return GetInfoFromCondDBLine(GetCorrespondingLine(ID), 87);
}


std::string CondDBTranslator::TranslateIDintoPythiaID(int ID){
  return GetInfoFromCondDBLine(GetCorrespondingLine(ID), 110);
}


std::string CondDBTranslator::TranslateIDintoAntiparticleName(int ID){
  std::string particleAntiparticleName = GetInfoFromCondDBLine(GetCorrespondingLine(ID), 123);
  if(particleAntiparticleName==""){
    std::ostringstream temp;
    temp << ID;
    std::string ID_string(temp.str());
    particleAntiparticleName = ID_string;
    return particleAntiparticleName;
  }
  else if(particleAntiparticleName != "self-cc"){
    return particleAntiparticleName;
  }
  else {
    return this->TranslateIDintoName(ID);
  }
}


Particle CondDBTranslator::CreateFullPropParticle(int ID){
  Particle new_particle;
  
  std::string particleLine = GetCorrespondingLine(ID);

  new_particle.set_mc_id(ID);
  

  std::string particleName = "";
  for(int i=2; particleLine.substr(i,1) != "|"; ++i){
    if(particleLine.substr(i,1) != " "){
      particleName += particleLine.at(i);
    }
  }
  new_particle.set_name(particleName);

  
  std::string particleAntiparticleName = "";
  for(int i=123; particleLine.substr(i,1) != "|"; ++i){
    if(particleLine.substr(i,1) != " "){
      particleAntiparticleName += particleLine.at(i);
    }
  }
  if(particleAntiparticleName != "self-cc"){
    new_particle.set_antiparticlename(particleAntiparticleName);
  }
  else {
    new_particle.set_antiparticlename(particleName);
  }

  
  std::string particleCharge = "";
  for(int i=36; particleLine.substr(i,1) != "|"; ++i){
    if(particleLine.substr(i,1) != " "){
      particleCharge += particleLine.at(i);
    }
  }
  new_particle.set_charge(particleCharge);

  
  std::string particleMass = "";
  for(int i=43; particleLine.substr(i,1) != "|"; ++i){
    if(particleLine.substr(i,1) != " "){
      particleMass += particleLine.at(i);
    }
  }
  new_particle.set_mass(particleMass);

  
  std::string particleCTauGamma = "";
  for(int i=57; particleLine.substr(i,1) != "|"; ++i){
    if(particleLine.substr(i,1) != " "){
      particleCTauGamma += particleLine.at(i);
    }
  }
  new_particle.set_ctaugamma(particleCTauGamma);

  
  std::string particleMaxWidth = "";
  for(int i=74; particleLine.substr(i,1) != "|"; ++i){
    if(particleLine.substr(i,1) != " "){
      particleMaxWidth += particleLine.at(i);
    }
  }
  new_particle.set_maxwidth(particleMaxWidth);
 
  
  std::string particleEvtGenName = "";
  for(int i=87; particleLine.substr(i,1) != "|"; ++i){
    if(particleLine.substr(i,1) != " "){
      particleEvtGenName += particleLine.at(i);
    }
  }
  new_particle.set_evtgenname(particleEvtGenName);
  
  
  std::string particlePythiaID = "";
  for(int i=110; particleLine.substr(i,1) != "|"; ++i){
    if(particleLine.substr(i,1) != " "){
      particlePythiaID += particleLine.at(i);
    }
  }
  new_particle.set_pythiaID(particlePythiaID);
  
  
  return new_particle;
}


Particle CondDBTranslator::CreateMinimalParticle(int ID){
  Particle new_particle;

  std::string particleLine = GetCorrespondingLine(ID);

  new_particle.set_mc_id(ID);
  
  std::string particleName = "";
  std::string particleAntiparticleName = "";


  //if the line corresponding to the ID is found, read out the (anti-)particles name and set both
  if (particleLine != "") {
    for(int i=2; particleLine.substr(i,1) != "|"; ++i){
      if(particleLine.substr(i,1) != " "){
        particleName += particleLine.at(i);
      }
    }
    new_particle.set_name(particleName);
  
  
    for(int i=123; particleLine.substr(i,1) != "|"; ++i){
      if(particleLine.substr(i,1) != " "){
        particleAntiparticleName += particleLine.at(i);
      }
    }
    if(particleAntiparticleName != "self-cc"){
      new_particle.set_antiparticlename(particleAntiparticleName);
    }
    else {
      new_particle.set_antiparticlename(particleName);
    }
  }
  //if the particle is not included in the CondDB, convert the ID into a string and use it as name
  else {
    std::stringstream temp;
    temp << ID;
    particleName = temp.str();
    particleAntiparticleName = temp.str();
  }

  
  return new_particle;
}

  
} //namespace mcdecaymatrixreader
} //namespace mctools
} //namespace dooselection
