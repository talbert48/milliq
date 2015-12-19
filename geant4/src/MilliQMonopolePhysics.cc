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
/// \file exoticphysics/monopole/src/MilliQMonopolePhysics.cc
/// \brief Implementation of the MilliQMonopolePhysics class
//
// $Id: MilliQMonopolePhysics.cc 68036 2013-03-13 14:13:45Z gcosmo $
//
//---------------------------------------------------------------------------
//
// ClassName:   MilliQMonopolePhysics
//
// Author:      V.Ivanchenko 13.03.2005
//
// Modified:
//
//  12.07.10  S.Burdin (changed the magnetic and electric charge variables from integer to double)
//----------------------------------------------------------------------------
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "MilliQMonopolePhysics.hh"
#include "MilliQMonopolePhysicsMessenger.hh"


#include "MilliQMonopole.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"

#include "G4StepLimiter.hh"
#include "G4Transportation.hh"
//#include "MilliQMonopoleTransportation.hh"
#include "G4GoudsmitSaundersonMscModel.hh"
#include "G4hMultipleScattering.hh"
//#include "G4eMultipleScattering.hh"
#include "G4NuclearStopping.hh"
#include "G4hCoulombScatteringModel.hh"
#include "G4ionIonisation.hh"
#include "G4IonCoulombScatteringModel.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4CoulombScattering.hh"
//#include "G4IonIonisation.hh"
#include "G4hBremsstrahlung.hh"
#include "G4mplIonisation.hh"
#include "G4mplIonisationWithDeltaModel.hh"
#include "G4hhIonisation.hh"
#include "G4hIonisation.hh"

#include "G4PhysicsListHelper.hh"

#include "G4BuilderType.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MilliQMonopolePhysics::MilliQMonopolePhysics(const G4String& nam)
  : G4VPhysicsConstructor(nam),
    fMessenger(0), fMpl(0)
{
  fMagCharge = 0.0;
  fElCharge = 0.01;
  fMonopoleMass = 0.105*GeV;
  fMessenger = new MilliQMonopolePhysicsMessenger(this);
  SetPhysicsType(bUnknown);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MilliQMonopolePhysics::~MilliQMonopolePhysics()
{
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MilliQMonopolePhysics::ConstructParticle()
{
	  fMpl = MilliQMonopole::MonopoleDefinition(fMonopoleMass, fMagCharge, fElCharge);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MilliQMonopolePhysics::ConstructProcess()
{
  if(verboseLevel > 0) {
    G4cout << "MilliQMonopolePhysics::ConstructProcess" << G4endl;
  }
  
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
   G4ProcessManager* pManager = fMpl->GetProcessManager();

   // defined monopole parameters and binning

   G4double magn = fMpl->MagneticCharge();
   G4double emin = fMonopoleMass/20000.;
   if(emin < keV) { emin = keV; }
   G4double emax = std::max(10.*TeV, fMonopoleMass*100);
   G4int nbin = G4lrint(10*std::log10(emax/emin));


 	G4cout<<" MilliQMonopolePhysics: It launched G4hIonisation!!"<<G4endl;

 	G4hIonisation* hhioni = new G4hIonisation();
     hhioni->SetDEDXBinning(nbin);
     hhioni->SetMinKinEnergy(emin);
     hhioni->SetMaxKinEnergy(emax);
     ph->RegisterProcess(hhioni, fMpl);
     pManager->AddProcess(hhioni,-1,2,2);

 //    G4CoulombScattering -> G4hCoulombScatteringModel, G4IonCoulombScatteringModel
 //    G4ionIonisation->G4IonParametrisedLossModel
 //	   G4NuclearStopping
 //    G4hBremsstrahlung

/*   G4hMultipleScattering* msc = new G4hMultipleScattering();
   msc->AddEmModel(0, new G4GoudsmitSaundersonMscModel());
   pManager->AddProcess(msc, -1, 1, 1);
   ph->RegisterProcess(msc, fMpl);
*/

    ph->RegisterProcess(new G4StepLimiter(), fMpl);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MilliQMonopolePhysics::SetMagneticCharge(G4double val)
{
  fMagCharge = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MilliQMonopolePhysics::SetElectricCharge(G4double val)
{
  fElCharge = 0.01;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MilliQMonopolePhysics::SetMonopoleMass(G4double mass)
{
  fMonopoleMass = 0.105*GeV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

