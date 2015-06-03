
#ifndef MilliQDetectorConstruction_H
#define MilliQDetectorConstruction_H 1

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Box;
class G4Tubs;
class MilliQDetectorStack;

#include "G4Material.hh"
#include "MilliQDetectorMessenger.hh"
#include "G4VisAttributes.hh"
#include "G4RotationMatrix.hh"

#include "MilliQPMTSD.hh"
#include "MilliQDetectorStackParameterisation.hh"

#include "G4VUserDetectorConstruction.hh"
#include "G4Cache.hh"

class MilliQDetectorConstruction : public G4VUserDetectorConstruction
{
  public:

    MilliQDetectorConstruction();
    virtual ~MilliQDetectorConstruction(){};

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    //Functions to modify the geometry
    void SetHousingThickness(G4double );
    void SetPMTRadius(G4double );
    void SetDefaults();

    //Get values
    G4double GetHousingThickness(){return fD_mtl;}
    G4double GetPMTRadius(){return fOuterRadius_pmt;}

    void SetHousingReflectivity(G4double );
    G4double GetHousingReflectivity(){return fRefl;}

    void SetMainScintYield(G4double );

  private:

    void DefineMaterials();
    G4VPhysicalVolume* ConstructDetector();

    MilliQDetectorMessenger* fDetectorMessenger;

    G4VPhysicalVolume* fWorldPV;

    //Materials & Elements
    G4Material* fScintillatorMaterial;
    G4Material* fAluminiumMaterial;
    G4Material* fAirMaterial;
    G4Material* fVacuumMaterial;
    G4Material* fGlassMaterial;
    G4Material* fConcreteMaterial;
    G4Material* led;
    G4Material* polyethylene;

    //Geometry
    G4double fScint_x;
    G4double fScint_y;
    G4double fScint_z;
    G4double fD_mtl;
    G4double fOuterRadius_pmt;
    G4double fRefl;

    MilliQDetectorStack* fDetectorStack;

    G4MaterialPropertiesTable* fScintillator_mt;

    //Sensitive Detectors
    G4Cache<MilliQPMTSD*> fPmt_SD;

};

#endif
