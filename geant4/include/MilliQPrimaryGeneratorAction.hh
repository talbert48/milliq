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
// $Id: MilliQPrimaryGeneratorAction.hh 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file MilliQPrimaryGeneratorAction.hh
/// \brief Definition of the MilliQPrimaryGeneratorAction class

#ifndef MilliQPrimaryGeneratorAction_h
#define MilliQPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4LorentzVector.hh"
#include "fstream"
#include "iostream"
#include "sstream"
#include "vector"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

/// The primary generator action class with particle gum.
///
/// It defines a single particle which hits the Tracker
/// perpendicular to the input face. The type of the particle
/// can be changed via the G4 build-in commands of G4ParticleGun class
/// (see the macros provided with this example).

class MilliQPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    MilliQPrimaryGeneratorAction();
    virtual ~MilliQPrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event* );

    G4ParticleGun* GetParticleGun() {return fParticleGun;}
    static G4int neventLHE;
    static std::vector<std::vector<G4double> > LHEFourVectors;


    // Set methods
    void SetRandomFlag(G4bool );

  private:
    G4ParticleGun*          fParticleGun; // G4 particle gun


    static bool __initLHEFourVectors;
    static bool initLHEFourVectors(){ // Doing this ignores the first 2 entries of the LHE file, but well...


        std::ifstream infile;
        infile.open("/home/qftsm/Work/GeantDevExamples/B2/B2a/LHEFile/outputpositive.dat" );
        G4String line;
        while( std::getline( infile, line ) ) {

        	std::istringstream iss( line );

            std::getline( infile, line );


            G4double px, py, pz, e;

            while( std::getline( infile, line ) ) {

            	iss.clear();
                iss.str( line );

                if( !( iss >> px >> py >> pz >> e  ) )
                	break;

                std::vector<G4double> table;
                table.push_back(px);
                table.push_back(py);
                table.push_back(pz);
                table.push_back(e);
                LHEFourVectors.push_back(table);

             }

         }


    	return true;

    }



};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


/*
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
*/
