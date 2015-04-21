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
// $Id: MilliQSteppingMessenger.cc 70256 2013-05-28 07:29:30Z gcosmo $
//
/// \file optical/MilliQ/src/MilliQSteppingMessenger.cc
/// \brief Implementation of the MilliQSteppingMessenger class
//
//
#include "MilliQSteppingMessenger.hh"
#include "MilliQSteppingAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MilliQSteppingMessenger::MilliQSteppingMessenger(MilliQSteppingAction* step)
 : fStepping(step)
{
  fOneStepPrimariesCmd = new G4UIcmdWithABool("/MilliQ/oneStepPrimaries",this);
  fOneStepPrimariesCmd->
      SetGuidance("Only allows primaries to go one step before being killed.");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MilliQSteppingMessenger::~MilliQSteppingMessenger(){
  delete fOneStepPrimariesCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void 
MilliQSteppingMessenger::SetNewValue(G4UIcommand* command,G4String newValue){
  if( command == fOneStepPrimariesCmd ){
    fStepping->SetOneStepPrimaries(fOneStepPrimariesCmd
                                  ->GetNewBoolValue(newValue));
  }
}
