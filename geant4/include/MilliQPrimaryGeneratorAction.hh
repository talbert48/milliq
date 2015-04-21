
#ifndef MilliQPrimaryGeneratorAction_h
#define MilliQPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;
class G4Event;

class MilliQPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:

    MilliQPrimaryGeneratorAction();
    virtual ~MilliQPrimaryGeneratorAction();
 
  public:

    virtual void GeneratePrimaries(G4Event* anEvent);

  private:

    G4ParticleGun* fParticleGun;
    
};

#endif
