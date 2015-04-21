
#include "MilliQPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"


MilliQPrimaryGeneratorAction::MilliQPrimaryGeneratorAction(){
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    fParticleGun = new G4ParticleGun(1);                                    //count
    fParticleGun->SetParticleDefinition(particleTable->FindParticle("e-")); //type
    fParticleGun->SetParticleEnergy(1.*MeV);                                //energy
    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.));           //position
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));  //direction
}


MilliQPrimaryGeneratorAction::~MilliQPrimaryGeneratorAction(){
    delete fParticleGun;
}


void MilliQPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent){
    fParticleGun->GeneratePrimaryVertex(anEvent);
}
