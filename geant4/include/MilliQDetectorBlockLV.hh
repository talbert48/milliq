
#ifndef MilliQDetectorBlock_H
#define MilliQDetectorBlock_H 1

#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4OpticalSurface.hh"

#include "MilliQDetectorConstruction.hh"
#include "MilliQPMTSD.hh"

class MilliQDetectorBlockLV : public G4LogicalVolume
{
  public:

    MilliQDetectorBlockLV(G4VSolid*              pSolid,
                          G4Material*            pMaterial,
                          const G4String&        name,
                          G4FieldManager*        pFieldMgr,
                          G4VSensitiveDetector*  pSDetector,
                          G4UserLimits*          pULimits,
                          G4bool                 optimise,
                          
                          G4ThreeVector  scintillatorSize,
                          G4double       housingThickness,
                          G4double       pmtRadius,
                          G4double       reflectivity,
                          MilliQPMTSD*   pSD);
    

    G4LogicalVolume* GetPhotocathodeLV(){return _photocathodeLV;}
    G4LogicalVolume* GetScintillatorLV(){return _scintillatorLV;}
    
  private:

    void VisAttributes();
    void SurfaceProperties();

    G4double _reflectivity;
    G4ThreeVector _housingSize;

    // Basic Volumes
    //
    G4Box*  _scintillatorV;
    G4Tubs* _pmtV;
    G4Tubs* _photocathodeV;

    // Logical volumes
    //
    G4LogicalVolume* _scintillatorLV;
    G4LogicalVolume* _pmtLV;
    G4LogicalVolume* _photocathodeLV;

};

#endif
