
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

#include "MilliQScintSD.hh"
#include "MilliQPMTSD.hh"

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
    G4double GetScintX(){return fScint_x;}
    G4double GetScintY(){return fScint_y;}
    G4double GetScintZ(){return fScint_z;}
    G4double GetHousingThickness(){return fD_mtl;}
    G4double GetPMTRadius(){return fOuterRadius_pmt;}

    void SetHousingReflectivity(G4double );
    G4double GetHousingReflectivity(){return fRefl;}

    void SetMainScintYield(G4double );

  private:

    void DefineMaterials();
    G4VPhysicalVolume* ConstructDetector();

    MilliQDetectorMessenger* fDetectorMessenger;

    G4Box* fExperimentalHall_box;
    G4LogicalVolume* fExperimentalHall_log;
    G4VPhysicalVolume* fExperimentalHall_phys;

    //Materials & Elements
    G4Material* fScintillator;
    G4Material* fAl;
    G4Element* fN;
    G4Element* fO;
    G4Material* fAir;
    G4Material* fVacuum;
    G4Element* fC;
    G4Element* fH;
    G4Material* fGlass;
    G4Material* fConcrete;

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
