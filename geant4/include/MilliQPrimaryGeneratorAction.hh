
#ifndef MilliQPrimaryGeneratorAction_h
#define MilliQPrimaryGeneratorAction_h 1

#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <string>

#include "globals.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;
class G4Event;

class MilliQPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:

    MilliQPrimaryGeneratorAction();
    virtual ~MilliQPrimaryGeneratorAction();
    virtual void GeneratePrimaries(G4Event* anEvent);

  private:

    G4ParticleGun* fParticleGun;
    
    std::vector<G4double*> fKnownData;
    std::string fKnownDataFilePath;
    G4bool* fKnownDataOn;
    G4int fKnownDataSize;
    
    G4int fKnownDataInterator;
    
    void GenerateKnownCMSParticles();
    
};

#endif
