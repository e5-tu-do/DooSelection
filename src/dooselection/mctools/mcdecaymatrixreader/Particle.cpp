// from STL
#include <vector>
#include <string>
#include <sstream>
#include <cmath>


// from Project
#include "Particle.h"
#include "CondDBTranslator.h"


namespace dooselection {
namespace mctools {
namespace mcdecaymatrixreader {
      


Particle::Particle(int ID) :
mc_id_(ID)
{
  CondDBTranslator giveAttribute;
  this->set_name(giveAttribute.TranslateIDintoName(ID));
  this->set_charge(giveAttribute.TranslateIDintoCharge(ID));
  this->set_mass(giveAttribute.TranslateIDintoMass(ID));
  this->set_ctaugamma(giveAttribute.TranslateIDintoCTauGamma(ID));
  this->set_maxwidth(giveAttribute.TranslateIDintoMaxWidth(ID));
  this->set_evtgenname(giveAttribute.TranslateIDintoEvtGenName(ID));
  this->set_pythiaID(giveAttribute.TranslateIDintoPythiaID(ID));
  this->set_antiparticlename(giveAttribute.TranslateIDintoAntiparticleName(ID));
}

Particle::Particle(){
  
}

Particle::~Particle(){
  
}

std::string Particle::name(){
  return name_;
}

int Particle::mc_id(){
  return mc_id_;
}

std::string Particle::charge(){
  return charge_;
}

std::string Particle::mass(){
  return mass_;
}

std::string Particle::ctaugamma(){
  return ctaugamma_;
}

std::string Particle::maxwidth(){
  return maxwidth_;
}

std::string Particle::evtgenname(){
  return evtgenname_;
}

std::string Particle::pythiaID(){
  return pythiaID_;
}

std::string Particle::antiparticlename(){
  return antiparticlename_;
}

int Particle::GetAbsoluteValueOfMCID(){
  return fabs(mc_id_);
}

void Particle::set_name(std::string new_name){
  name_ = new_name;
}

void Particle::set_mc_id(int new_mc_id){
  mc_id_ = new_mc_id;
}

void Particle::set_charge(std::string new_charge){
  charge_ = new_charge;
}

void Particle::set_mass(std::string new_mass){
  mass_ = new_mass;
}

void Particle::set_ctaugamma(std::string new_ctaugamma){
  ctaugamma_ = new_ctaugamma;
}

void Particle::set_maxwidth(std::string new_maxwidth){
  maxwidth_ = new_maxwidth;
}

void Particle::set_evtgenname(std::string new_evtgenname){
  evtgenname_ = new_evtgenname;
}

void Particle::set_pythiaID(std::string new_pythiaID){
  pythiaID_ = new_pythiaID;
}

void Particle::set_antiparticlename(std::string new_antiparticlename){
  antiparticlename_ = new_antiparticlename;
}



std::string Particle::GetDecay(){
  std::string decaystring = name_;
  std::vector<Particle>::iterator it = daugther_particles_.begin();
  if (it != daugther_particles_.end()){
    decaystring += "(";
    do {
      decaystring += it->name();
      ++it;
      if (it != daugther_particles_.end()){
        decaystring += " ";
      }
    } while (it != daugther_particles_.end());
    decaystring += ")";
  }
  return decaystring;
}


std::string Particle::GetCompleteDecay(){
  std::string decaystring = name_;
  std::vector<Particle>::iterator it = daugther_particles_.begin();
  if (it != daugther_particles_.end()){
    decaystring += "(";
    do {
      decaystring += it->GetCompleteDecay();
      ++it;
      if (it != daugther_particles_.end()){
        decaystring += " ";
      }
    } while (it != daugther_particles_.end());
    decaystring += ")";
  }
  return decaystring;
}



std::string Particle::GetConjugatedDecay(){
  std::string decaystring = this->antiparticlename();
  std::vector<Particle>::iterator it = daugther_particles_.begin();
  if (it != daugther_particles_.end()){
    decaystring += "(";
    do {
      decaystring += it->antiparticlename();
      ++it;
      if (it != daugther_particles_.end()){
        decaystring += " ";
      }
    } while (it != daugther_particles_.end());
    decaystring += ")";
  }
  return decaystring;
}


std::string Particle::GetConjugatedCompleteDecay(){
  std::string decaystring = this->antiparticlename();
  std::vector<Particle>::iterator it = daugther_particles_.begin();
  if (it != daugther_particles_.end()){
    decaystring += "(";
    do {
      decaystring += it->GetConjugatedCompleteDecay();
      ++it;
      if (it != daugther_particles_.end()){
        decaystring += " ";
      }
    } while (it != daugther_particles_.end());
    decaystring += ")";
  }
  return decaystring;
}


std::string Particle::GetConjugatedInitialStateDecay(){
  std::string decaystring = this->antiparticlename();
  std::vector<Particle>::iterator it = daugther_particles_.begin();
  if (it != daugther_particles_.end()){
    decaystring += "(";
    do {
      decaystring += it->name();
      ++it;
      if (it != daugther_particles_.end()){
        decaystring += " ";
      }
    } while (it != daugther_particles_.end());
    decaystring += ")";
  }
  return decaystring;
}


std::string Particle::GetConjugatedInitialStateCompleteDecay(){
  std::string decaystring = this->antiparticlename();
  std::vector<Particle>::iterator it = daugther_particles_.begin();
  if (it != daugther_particles_.end()){
    decaystring += "(";
    do {
      decaystring += it->GetCompleteDecay();
      ++it;
      if (it != daugther_particles_.end()){
        decaystring += " ";
      }
    } while (it != daugther_particles_.end());
    decaystring += ")";
  }
  return decaystring;
}


std::string Particle::GetConjugatedFinalStateDecay(){
  std::string decaystring = this->name();
  std::vector<Particle>::iterator it = daugther_particles_.begin();
  if (it != daugther_particles_.end()){
    decaystring += "(";
    do {
      decaystring += it->antiparticlename();
      ++it;
      if (it != daugther_particles_.end()){
        decaystring += " ";
      }
    } while (it != daugther_particles_.end());
    decaystring += ")";
  }
  return decaystring;
}


std::string Particle::GetConjugatedFinalStateCompleteDecay(){
  std::string decaystring = this->name();
  std::vector<Particle>::iterator it = daugther_particles_.begin();
  if (it != daugther_particles_.end()){
    decaystring += "(";
    do {
      decaystring += it->GetConjugatedCompleteDecay();
      ++it;
      if (it != daugther_particles_.end()){
        decaystring += " ";
      }
    } while (it != daugther_particles_.end());
    decaystring += ")";
  }
  return decaystring;
}


std::string Particle::GetCompleteMCIDDecay(){
  std::stringstream temp;
  temp << mc_id_;
  std::string decaystring = temp.str();

  std::vector<Particle>::iterator it = daugther_particles_.begin();
  if (it != daugther_particles_.end()){
    decaystring += "(";
    do {
      decaystring += it->GetCompleteMCIDDecay();
      ++it;
      if (it != daugther_particles_.end()){
        decaystring += " ";
      }
    } while (it != daugther_particles_.end());
    decaystring += ")";
  }
  return decaystring;
}


std::string Particle::GetCompleteAbsMCIDDecay(){
  std::stringstream temp;
  temp << fabs(mc_id_) ;
  std::string decaystring = temp.str();
  
  std::vector<Particle>::iterator it = daugther_particles_.begin();
  if (it != daugther_particles_.end()){
    decaystring += "(";
    do {
      decaystring += it->GetCompleteAbsMCIDDecay();
      ++it;
      if (it != daugther_particles_.end()){
        decaystring += " ";
      }
    } while (it != daugther_particles_.end());
    decaystring += ")";
  }
  return decaystring;
}


bool Particle::IsEqualDecay(std::string ComparativeDecay){
  if (this->GetCompleteDecay()==ComparativeDecay){
    return true;
  }
  else return false;
}


bool Particle::IsConjugatedDecay(std::string ComparativeDecay){
  if (this->GetConjugatedCompleteDecay()==ComparativeDecay){
    return true;
  }
  else return false;
}


bool Particle::IsDecayWithConjugatedFinalState(std::string ComparativeDecay){
  if (this->GetConjugatedFinalStateDecay()==ComparativeDecay){
    return true;
  }
  else return false;
}


bool Particle::IsDecayWithConjugatedInitialState(std::string ComparativeDecay){
  if (this->GetConjugatedInitialStateDecay()==ComparativeDecay){
    return true;
  }
  else return false;
}

bool Particle::IsComparableDecay(std::string ComparativeDecay){
  if (this->GetCompleteDecay()==ComparativeDecay){
    return true;
  }
  else if (this->GetConjugatedCompleteDecay()==ComparativeDecay){
    return true;
  }
  else if (this->GetConjugatedFinalStateDecay()==ComparativeDecay){
    return true;
  }
  else if (this->GetConjugatedInitialStateDecay()==ComparativeDecay){
    return true;
  }
  else return false;
}

void Particle::AddDaughterParticle(Particle new_particle){
  daugther_particles_.push_back(new_particle);
}

  
} //namespace mcdecaymatrixreader
} //namespace mctools
} //namespace dooselection
