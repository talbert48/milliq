
#include "MilliQPhysicsList.hh"

#include "MilliQGeneralPhysics.hh"
#include "MilliQEMPhysics.hh"
#include "MilliQMuonPhysics.hh"

#include "G4OpticalPhysics.hh"
#include "G4OpticalProcessIndex.hh"

#include "G4SystemOfUnits.hh"

MilliQPhysicsList::MilliQPhysicsList() : G4VModularPhysicsList()
{
    // default cut value  (1.0mm)
    defaultCutValue = 1.0*mm;

    // General Physics
    RegisterPhysics( new MilliQGeneralPhysics("general") );

    // EM Physics
    RegisterPhysics( new MilliQEMPhysics("standard EM"));

    // Muon Physics
    RegisterPhysics( new MilliQMuonPhysics("muon"));

    // Optical Physics
    G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
    RegisterPhysics( opticalPhysics );


    opticalPhysics->SetScintillationYieldFactor(1.0);
    opticalPhysics->SetScintillationExcitationRatio(0.0);

    opticalPhysics->SetMaxNumPhotonsPerStep(100);
    opticalPhysics->SetMaxBetaChangePerStep(10.0);

    opticalPhysics->SetTrackSecondariesFirst(kCerenkov,true);
    opticalPhysics->SetTrackSecondariesFirst(kScintillation,true);
}


MilliQPhysicsList::~MilliQPhysicsList() {}


void MilliQPhysicsList::SetCuts(){
  //  " G4VUserPhysicsList::SetCutsWithDefault" method sets
  //   the default cut value for all particle types
  SetCutsWithDefault();
}
