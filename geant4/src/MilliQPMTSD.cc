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
// $Id: MilliQPMTSD.cc 73915 2013-09-17 07:32:26Z gcosmo $
//
/// \file optical/MilliQ/src/MilliQPMTSD.cc
/// \brief Implementation of the MilliQPMTSD class
//
//
#include "MilliQPMTSD.hh"
#include "MilliQPMTHit.hh"
#include "MilliQDetectorConstruction.hh"
#include "MilliQUserTrackInformation.hh"

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"


MilliQPMTSD::MilliQPMTSD(G4String name)
  : G4VSensitiveDetector(name),fPMTHitCollection(0)
{
  collectionName.insert("pmtHitCollection");
}


MilliQPMTSD::~MilliQPMTSD() {}


void MilliQPMTSD::Initialize(G4HCofThisEvent* hitsCE){
  fPMTHitCollection = new MilliQPMTHitsCollection
                      (SensitiveDetectorName,collectionName[0]);
  //Store collection with event and keep ID
  static G4int hitCID = -1;
  if(hitCID<0){
    hitCID = GetCollectionID(0);
  }
  hitsCE->AddHitsCollection( hitCID, fPMTHitCollection );
}


G4bool MilliQPMTSD::ProcessHits(G4Step* ,G4TouchableHistory* ){
  return false;
}


//Generates a hit and uses the postStepPoint's mother volume replica number
//PostStepPoint because the hit is generated manually when the photon is
//absorbed by the photocathode

G4bool MilliQPMTSD::ProcessHits_constStep(const G4Step* aStep,
                                       G4TouchableHistory* ){

  //need to know if this is an optical photon
  if(aStep->GetTrack()->GetDefinition()
     != G4OpticalPhoton::OpticalPhotonDefinition()) return false;
 
  //User replica number 1 since photocathode is a daughter volume
  //to the pmt which was replicated
  G4int pmtNumber=
    aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(1);
  G4VPhysicalVolume* physVol=
    aStep->GetPostStepPoint()->GetTouchable()->GetVolume(1);

  //Find the correct hit collection
  G4int n=fPMTHitCollection->entries();
  MilliQPMTHit* hit=NULL;
  for(G4int i=0;i<n;i++){
    if((*fPMTHitCollection)[i]->GetPMTNumber()==pmtNumber){
      hit=(*fPMTHitCollection)[i];
      break;
    }
  }
 
  if(hit==NULL){//this pmt wasnt previously hit in this event
    hit = new MilliQPMTHit(); //so create new hit
    hit->SetPMTNumber(pmtNumber);
    hit->SetPMTPhysVol(physVol);
    fPMTHitCollection->insert(hit);
  }

  hit->IncPhotonCount(); //increment hit for the selected pmt
 
    hit->SetDrawit(true);

  return true;
}


void MilliQPMTSD::EndOfEvent(G4HCofThisEvent* ) {}


void MilliQPMTSD::clear() {}


void MilliQPMTSD::DrawAll() {}


void MilliQPMTSD::PrintAll() {}
