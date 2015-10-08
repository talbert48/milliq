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
/// \file field/field03/src/MilliQPrimaryGeneratorMessenger.cc
/// \brief Implementation of the MilliQPrimaryGeneratorMessenger class
//
//
// $Id: MilliQPrimaryGeneratorMessenger.cc 77892 2013-11-29 08:56:08Z gcosmo $
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "../include/MilliQPrimaryGeneratorMessenger.hh"
#include "MilliQPrimaryGeneratorAction.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ParticleDefinition*  MilliQPrimaryGeneratorAction::fgPrimaryParticle = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MilliQPrimaryGeneratorMessenger::MilliQPrimaryGeneratorMessenger(
                                           MilliQPrimaryGeneratorAction* action)
 : G4UImessenger(),
   fAction(action),
   fRndmCmd(0),
   fSetXVertexCmd(0),
   fSetYVertexCmd(0),
   fSetZVertexCmd(0),
   fSetCalibEnergyCmd(0)

{
  fRndmCmd = new G4UIcmdWithAString("/gun/random",this);
  fRndmCmd->SetGuidance("Shoot randomly the incident particle.");
  fRndmCmd->SetGuidance("  Choice : on, off(default)");
  fRndmCmd->SetParameterName("choice",true);
  fRndmCmd->SetDefaultValue("off");
  fRndmCmd->SetCandidates("on off");
  fRndmCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
 
  fSetXVertexCmd = new G4UIcmdWithADoubleAndUnit("/gun/xvertex",this);
  fSetXVertexCmd->SetGuidance(" Set x coord. of the primary vertex.");
  fSetXVertexCmd->SetParameterName("xv",true);
  fSetXVertexCmd->SetDefaultValue(0.0*cm);
  fSetXVertexCmd->SetDefaultUnit("cm");

  fSetYVertexCmd = new G4UIcmdWithADoubleAndUnit("/gun/yvertex",this);
  fSetYVertexCmd->SetGuidance(" Set y coord. of the primary vertex.");
  fSetYVertexCmd->SetParameterName("yv",true);
  fSetYVertexCmd->SetDefaultValue(0.0*cm);
  fSetYVertexCmd->SetDefaultUnit("cm");

  fSetZVertexCmd = new G4UIcmdWithADoubleAndUnit("/gun/zvertex",this);
  fSetZVertexCmd->SetGuidance(" Set z coord. of the primary vertex.");
  fSetZVertexCmd->SetParameterName("zv",true);
  fSetZVertexCmd->SetDefaultValue(0.0*cm);
  fSetZVertexCmd->SetDefaultUnit("cm");

  fSetCalibEnergyCmd = new G4UIcmdWithADoubleAndUnit("/gun/calibenergy",this);
  fSetCalibEnergyCmd->SetGuidance(" Set CalibEnergy of the primary vertex, used for calibration mode.");
  fSetCalibEnergyCmd->SetParameterName("ene",true);
  fSetCalibEnergyCmd->SetDefaultValue(0.0*GeV);
  fSetCalibEnergyCmd->SetDefaultUnit("GeV");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MilliQPrimaryGeneratorMessenger::~MilliQPrimaryGeneratorMessenger()
{
  delete fRndmCmd;
  delete fSetXVertexCmd;
  delete fSetYVertexCmd;
  delete fSetZVertexCmd;
  delete fSetCalibEnergyCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MilliQPrimaryGeneratorMessenger::SetNewValue(G4UIcommand * command,
                                               G4String newValue)
{
  if( command == fRndmCmd )
   { fAction->SetRndmFlag(newValue);}
  if( command == fSetXVertexCmd )
   { fAction->SetXVertex(fSetXVertexCmd->GetNewDoubleValue(newValue));}
  if( command == fSetYVertexCmd )
   { fAction->SetYVertex(fSetYVertexCmd->GetNewDoubleValue(newValue));}
  if( command == fSetZVertexCmd )
   { fAction->SetZVertex(fSetZVertexCmd->GetNewDoubleValue(newValue));}
  if( command == fSetCalibEnergyCmd )
   { fAction->SetCalibEnergy(fSetCalibEnergyCmd->GetNewDoubleValue(newValue));}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
