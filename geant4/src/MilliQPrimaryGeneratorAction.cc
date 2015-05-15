
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
    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.));           //position

    if(fKnownDataOn){
        fParticleGun->SetParticleEnergy(fKnownData[fKnownDataInterator][3]*GeV);                            //energy
        fParticleGun->SetParticleMomentumDirection(G4ThreeVector(fKnownData[fKnownDataInterator][0]*GeV,    //x-direction
                                                                 fKnownData[fKnownDataInterator][1]*GeV,    //y-direction
                                                                 fKnownData[fKnownDataInterator][2]*GeV));  //z-direction
        //iterate or start over iteration if it excesses known data size
        fKnownDataInterator = fKnownDataInterator+1>=fKnownDataSize ? 0 : fKnownDataInterator+1;
    }else{
        fParticleGun->SetParticleEnergy(1.*GeV);                                //energy
        fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));  //direction
    }
}


MilliQPrimaryGeneratorAction::~MilliQPrimaryGeneratorAction(){
    delete fParticleGun;
}


void MilliQPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent){
    fParticleGun->GeneratePrimaryVertex(anEvent);
}


void MilliQPrimaryGeneratorAction::GenerateKnownCMSParticles()
{
    //read in file containing particle data
    //
    std::ifstream file;
    file.open(fKnownDataFilePath);
    if(!file.is_open()){
        fKnownDataOn = false;
        G4cout << "Particle Event Data: Could Not Load Data from \"" << fKnownDataFilePath.c_str() << "\"\n";
        return;
    }
    
    G4String value;
    int itr = 0;
    while ( file.good() && itr < fKnownDataSize)
    {
        G4double fourMomentum[5];
        for(int i=0; i<5; i++)
        {
            getline( file, value, ',' );
            fourMomentum[i] = stod(value);
        }
        fKnownData.push_back(fourMomentum);
        itr++;
    }
    file.close();
    G4cout << "Particle Event Data: Loaded\n";
}
