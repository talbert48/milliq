
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
                          const G4String&        pName,
                          G4FieldManager*        pFieldManager,
                          G4VSensitiveDetector*  pSD,
                          G4UserLimits*          pUserLimits,
                          G4bool                 pOptimise,
                          
                          G4ThreeVector  pScintillatorDimensions,
                          G4double       pScintillatorHousingThickness,
                          G4double       pScintillatorHousingReflectivity,
                          
                          G4double  pPmtRadius,                 //outer radius of pmt including glass, not including metal shielding or housing
                          G4double  pPmtHeight,                 //height of pmt including glass, not including metal sheilding or housing
                          G4double  pPmtPhotocathodeDepth,      //depth from front(scinillator side) of pmt to beginning of phtocathode section, including glass
                          G4double  pPmtHousingThickness,
                          G4double  pPmtGlassThickness,
                          G4double  pPmtHousingReflectivity,
                          G4VSensitiveDetector*   pPmtSD);
    
    G4ThreeVector GetDimensions(){return fDimensions;};
    
  private:
    
    void VisAttributes();
    void SurfaceProperties(G4double pScintillatorHousingReflectivity, G4double pPmtHousingReflectivity);
    
    G4ThreeVector    fDimensions;
    
    G4LogicalVolume* fScintillatorHousingLV;
    G4LogicalVolume* fScintillatorLV;
    
    G4LogicalVolume* fPmtHousingLV;
    G4LogicalVolume* fPmtGlassLV;
    G4LogicalVolume* fPmtVacuumSectionLV;
    G4LogicalVolume* fPmtPhotocathodeSectionLV;
};

#endif
