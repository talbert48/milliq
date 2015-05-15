
#include "MilliQStackParameterisation.hh"

MilliQStackParameterisation::MilliQStackParameterisation(G4ThreeVector  pN,
                                                         G4ThreeVector  pBlockDimensions,
                                                         G4ThreeVector  pGapDimensions)
{
    fN = pN;
    fBlockDimensions = pBlockDimensions;
    fGapDimensions = pGapDimensions;
}


MilliQStackParameterisation::~MilliQStackParameterisation(){}


void MilliQStackParameterisation::ComputeTransformation(const G4int         pId,
                                                        G4VPhysicalVolume*  pPV) const
{
    G4ThreeVector idDimensions;
    GetIdDimensionsFromId(pId,idDimensions);
    
    // origin = (idDimensions - ((fN-1.)/2)) * (fBlockDimensions+fGapDimensions)
    //
    G4ThreeVector origin;
    G4ThreeVector one(1.,1.,1.);
    G4ThreeVector blockIdPosition =  idDimensions - ((fN-one)/2.);
    G4ThreeVector individualBlockVolume = fBlockDimensions+fGapDimensions;
    origin.setX( blockIdPosition.x() * individualBlockVolume.x() );
    origin.setY( blockIdPosition.y() * individualBlockVolume.y() );
    origin.setZ( blockIdPosition.z() * individualBlockVolume.z() );
    
    pPV->SetTranslation(origin);
    pPV->SetRotation(0);
}


void MilliQStackParameterisation::GetIdDimensionsFromId(G4int           pId,
                                                        G4ThreeVector&  pIdDimensions) const
{
    pIdDimensions.setX( (pId % (G4int)fN.x()) );
    pIdDimensions.setY( (pId-pIdDimensions.x())/(G4int)fN.x() );
    pIdDimensions.setZ( 0. );
}


G4ThreeVector MilliQStackParameterisation::GetStackDimensions()
{
    // stackDimensions = fN*(fBlockDimensions+fGapDimensions)
    //
    G4ThreeVector stackDimensions;
    G4ThreeVector individualBlockVolume = fBlockDimensions+fGapDimensions;
    stackDimensions.setX(fN.x() * individualBlockVolume.x());
    stackDimensions.setY(fN.y() * individualBlockVolume.y());
    stackDimensions.setZ(fN.z() * individualBlockVolume.z());
    
    return stackDimensions;
}
