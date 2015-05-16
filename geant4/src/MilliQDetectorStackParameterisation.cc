
#include "MilliQDetectorStackParameterisation.hh"

MilliQDetectorStackParameterisation::MilliQDetectorStackParameterisation(G4int  pN,
                                                                         G4ThreeVector  pStackDimensions,
                                                                         G4ThreeVector  pDistributionUnitVector,
                                                                         G4double       pStartDepth,
                                                                         G4double       pEndDepth)
{
    fStartCenterDepth = pStartDepth + (pStackDimensions.z()/2.);
    fN = pN;
    
    fDepthGap = (pEndDepth - pStartDepth - pStackDimensions.z())/pN;
    
    fStackDimensions = pStackDimensions;
    fDistributionUnitVector = pDistributionUnitVector;
}


MilliQDetectorStackParameterisation::~MilliQDetectorStackParameterisation(){}


void MilliQDetectorStackParameterisation::ComputeTransformation(const G4int pId,
                                                                G4VPhysicalVolume*  pPV) const
{
    G4ThreeVector translation = (fStartCenterDepth + pId*fDepthGap)*fDistributionUnitVector;
    
    pPV->SetTranslation(translation);
    //pPV->SetRotation( new G4RotationMatrix( fDistributionUnitVector.getPhi() , fDistributionUnitVector.getTheta() , fDistributionUnitVector.getRho() ) );
}


G4ThreeVector MilliQDetectorStackParameterisation::GetStackDimensions()
{
    G4ThreeVector stackDimensions;
    stackDimensions.setX(fStackDimensions.x());
    stackDimensions.setY(fStackDimensions.y());
    stackDimensions.setZ((fStackDimensions.z()*fN) + (fDepthGap*(fN-1)));
    
    return stackDimensions;
}
