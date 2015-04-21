
#ifndef MilliQDetectorStack_H
#define MilliQDetectorStack_H 1

#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4OpticalSurface.hh"
#include "G4PVParameterised.hh"

#include "MilliQStackParameterisation.hh"
#include "MilliQDetectorBlockLV.hh"
#include "MilliQPMTSD.hh"

class MilliQDetectorStack : public G4PVPlacement
{
public:
    
    MilliQDetectorStack(G4RotationMatrix*   pRot,
                        const G4ThreeVector&tlate,
                        G4LogicalVolume*    pMotherLogical,
                        G4bool              pMany,
                        G4int               pCopyNo,
                        G4ThreeVector       sSize,
                        G4double            hThickness,
                        G4double            pmtRadius,
                        G4double            refl,
                        MilliQPMTSD*        pSD);
    
private:
    G4Box* fStackhouseingV;
    G4LogicalVolume* fStackhouseingLV;
};

#endif
