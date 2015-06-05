


//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: MilliQPrimaryGeneratorAction.cc 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file MilliQPrimaryGeneratorAction.cc
/// \brief Implementation of the MilliQPrimaryGeneratorAction class

#include "MilliQPrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4LorentzVector.hh"
#include "G4Event.hh"
#include "vector"
#include <math.h>
#include "G4ParticleGun.hh"

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4Electron.hh"

#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int MilliQPrimaryGeneratorAction::neventLHE=0;
std::vector<std::vector<G4double> > MilliQPrimaryGeneratorAction::LHEFourVectors;
bool MilliQPrimaryGeneratorAction::__initLHEFourVectors = MilliQPrimaryGeneratorAction::initLHEFourVectors();

MilliQPrimaryGeneratorAction::MilliQPrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction()
{
	fParticleGun = new G4ParticleGun(G4Electron::Definition());
/*
	 G4int nofParticles = 1;
	  fParticleGun = new G4ParticleGun(nofParticles);

	  // default particle kinematic

	  G4ParticleDefinition* particleDefinition
	    = G4ParticleTable::GetParticleTable()->FindParticle("proton");

	  fParticleGun->SetParticleDefinition(particleDefinition);
	  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
	  fParticleGun->SetParticleEnergy(3.0*GeV);
*/
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MilliQPrimaryGeneratorAction::~MilliQPrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MilliQPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // This function is called at the begining of event

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume
  // from G4LogicalVolumeStore.

	G4double xGun=18.9*m;//0*cm; //14.6 gets it to the other stack
	G4double yGun=6*cm; //Goes up
	G4double zGun=0*cm;
	fParticleGun->SetParticlePosition(G4ThreeVector(xGun,yGun,zGun));
	G4double xMoGun = LHEFourVectors[neventLHE][0];
	G4double yMoGun = LHEFourVectors[neventLHE][1];
	G4double zMoGun = LHEFourVectors[neventLHE][2];
	G4double MoNorm = sqrt(pow(xMoGun,2)+pow(yMoGun,2)+pow(zMoGun,2));

//	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(xMoGun/MoNorm,yMoGun/MoNorm,zMoGun/MoNorm));
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1-0.1*G4UniformRand(),0.1*G4UniformRand(),0.1*G4UniformRand()));


	G4double energyGun=LHEFourVectors[neventLHE][3]*CLHEP::GeV;
//	fParticleGun->SetParticleEnergy(energyGun);
	fParticleGun->SetParticleEnergy(0.5*CLHEP::GeV);
	fParticleGun->GeneratePrimaryVertex(anEvent);
	neventLHE++;

	/*
	G4double worldZHalfLength = 0;
	  G4LogicalVolume* worldLV
	    = G4LogicalVolumeStore::GetInstance()->GetVolume("World");
	  G4Box* worldBox = NULL;
	  if ( worldLV ) worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid());
	  if ( worldBox ) worldZHalfLength = worldBox->GetZHalfLength();
	  else  {
	    G4cerr << "World volume of box not found." << G4endl;
	    G4cerr << "Perhaps you have changed geometry." << G4endl;
	    G4cerr << "The gun will be place in the center." << G4endl;
	  }

	  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -worldZHalfLength));

	  fParticleGun->GeneratePrimaryVertex(anEvent);

*/
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


/*

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
        fParticleGun->SetParticleEnergy(fKnownData[fKnownDataInterator][3]*GeV); //energy
        fParticleGun->SetParticleMomentumDirection(G4ThreeVector(fKnownData[fKnownDataInterator][0]*GeV, //x-direction
                                                                 fKnownData[fKnownDataInterator][1]*GeV, //y-direction
                                                                 fKnownData[fKnownDataInterator][2]*GeV)); //z-direction
        //iterate or start over iteration if it excesses known data size
        fKnownDataInterator = fKnownDataInterator+1>=fKnownDataSize ? 0 : fKnownDataInterator+1;
    }else{
        fParticleGun->SetParticleEnergy(1.*GeV);                                //energy
        fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1., 0., 0.));  //direction
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
*/
