
#include "globals.hh"

#include "MilliQDetectorStack.hh"

#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4SystemOfUnits.hh"


MilliQDetectorStack::MilliQDetectorStack(G4RotationMatrix*              pRot,
                                         const G4ThreeVector&           tlate,
                                         G4LogicalVolume*               pMotherLogical,
                                         G4bool                         pMany,
                                         G4int                          pCopyNo,
                                         G4ThreeVector                  scintillatorSize,
                                         G4double                       housingThickness,
                                         G4double                       pmtRadius,
                                         G4double                       reflectivity,
                                         MilliQPMTSD*                   pSD)
                    :G4PVPlacement(pRot,
                                   tlate,
                                   //Temp logical volume
                                   new G4LogicalVolume(new G4Box("temp",1,1,1),
                                                       G4Material::GetMaterial("Vacuum"),
                                                       "temp",
                                                       0,0,0),
                                   "housing",
                                   pMotherLogical,
                                   pMany,
                                   pCopyNo)
{
    //
    // stack volume and logical volumes
    //
    fStackhouseingV = new G4Box( "Stack Housing Volume" ,
                           1.*m ,1.*m , 1.*m ); //temp dimensions
    fStackhouseingLV = new G4LogicalVolume(fStackhouseingV,
                                      G4Material::GetMaterial("Air"),
                                      "Stack Houseing Logical Volume",
                                      0,0,0);
    fStackhouseingLV->SetVisAttributes(G4VisAttributes::Invisible);
    //fStackhouseingLV->SetVisAttributes(G4Color(.5,.5,.5,.1));
    
    //
    // parameterise and place detector blocks
    //
    G4ThreeVector housingSize = scintillatorSize + (2.*G4ThreeVector(housingThickness,housingThickness,housingThickness));
    
    G4Box* houseingV = new G4Box("Housing Volume" ,
                           housingSize.x()/2. , housingSize.y()/2. , housingSize.z()/2. );

    
    MilliQDetectorBlockLV* aDetectorBlock
    = new MilliQDetectorBlockLV(houseingV,
                                G4Material::GetMaterial("Al"),
                                "Houseing Logical Volume",
                                0,0,0,
                                true,
                                scintillatorSize,
                                housingThickness,
                                pmtRadius,
                                reflectivity,
                                pSD);
    
    MilliQStackParameterisation* parameterisation
    = new MilliQStackParameterisation(G4ThreeVector(1.,1.,1.),          //n
                                      housingSize,                      //block dimensions
                                      G4ThreeVector(1.*cm,1.*cm,1.*cm));//gap dimensions
    
    new G4PVParameterised( "Detector Blocks",
                            aDetectorBlock,
                            fStackhouseingLV,
                            kZAxis,
                            parameterisation->GetNumberOfBlocks(),
                            parameterisation);
    
    //
    // set final dimensions of stack housing and set this logical volume
    //
    G4ThreeVector stackDimensions = parameterisation->GetStackDimensions();
    fStackhouseingV->SetXHalfLength(stackDimensions.x()/2.);
    fStackhouseingV->SetYHalfLength(stackDimensions.y()/2.);
    fStackhouseingV->SetZHalfLength(stackDimensions.z()/2.);
    
    
    SetLogicalVolume(fStackhouseingLV);
}
