//
//  main.cpp
//  CMS Sidedetector
//
//  Created by James London on 8/25/14.
//  Copyright (c) 2014 James London. All rights reserved.
//

#include "structsandfunctions.h"

using namespace TMath;
using namespace std;

void setUserVaribles()
{
    //-------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------//
    //------------------------ USER MAY SET BELLOW VARIBLES -------------------------//
    
    deltaTime                           = 1.0/pow(10, 9);//s
    
    numberOfTotalParticles              = pow(10, 3);
    particleMass                        = 50;//GeV
    particleCharge                      = 1.0/pow(10, 3);//e
    
    detectorAlighnmentAngle             = Pi()/6;//radians
    
    detectorRoom.lowestYZCorner.SetX(     0);//m
    detectorRoom.lowestYZCorner.SetY(     10);//m
    detectorRoom.lowestYZCorner.SetZ(     0);//m
    detectorRoom.width                  = 10;//m
    detectorRoom.depth                  = 10;//m
    detectorRoom.height                 = 2;//m
    
    int numberOfDetectors               = 3;
    
    double detectorZDisplacement        = 0;//m
    
    double detectorWidths               = 1;//m
    double detectorDepths               = 1.4;//m
    double detectorHeights              = 1;//m
    
    double subDetectorWidths            = .1;//m
    double subDetectorDepths            = 1.4;//m
    double subDetectorHeigths           = .05;//m
    
    int numberOfSubDetectorsAlongWidth  = 10;
    int numberOfSubDetectorsAlongHeight = 20;
    
    CMSMagnet aCMSMagnet;
    aCMSMagnet.strength                 = 4;//T
    aCMSMagnet.direction                = direction::positive;
    aCMSMagnet.internalRadius           = 0;//m
    aCMSMagnet.externalRadius           = 4;//m
    CMSMagnets.push_back(aCMSMagnet);
    
    aCMSMagnet.direction                = direction::negative;
    aCMSMagnet.internalRadius           = aCMSMagnet.externalRadius;
    aCMSMagnet.externalRadius           = 8;//m
    CMSMagnets.push_back(aCMSMagnet);
    
    displayDetectorRoom                 = true;
    displayDetectorAlignmentAngle       = false;
    displaySubDetetorsInSetup           = false;
    displayAxesInSetup                  = true;
    calculateWithMagnets                = true;
    drawAllParticlesPaths               = false;
    drawDetectedParticlesPaths          = true;
    
    CMSParticleParametersRootFileName   = "mQ_prod_LHC14_M050GeV_unweighted_events.root";
    useKnownCMSParticleParameters       = false;
    
    //------------------------ USER MAY SET  ABOVE VARIBLES -------------------------//
    //-------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------//
    
    //Convert
    particleMass = unitConversion("GeV -> kg", particleMass);
    particleCharge = unitConversion("e -> C", particleCharge);
    
    initializeDetectorsWith(numberOfDetectors, detectorWidths, detectorDepths, detectorHeights, detectorZDisplacement, numberOfSubDetectorsAlongWidth, numberOfSubDetectorsAlongHeight, subDetectorWidths, subDetectorDepths, subDetectorHeigths);
}

particle adjustmentsFromCMSMagnets(particle aParticle)
{
    particle p = aParticle;
    for (int i=0; i<CMSMagnets.size(); i++) {
        double w = aParticle.charge * CMSMagnets.at(i).strength * (double)CMSMagnets.at(i).direction * pow(C(), 2) / aParticle.fourMomentum.E();
        double velocity = C()*aParticle.fourMomentum.Vect().Mag()/Sqrt(Power(C()*aParticle.mass,2)+Power(aParticle.fourMomentum.Vect().Mag(),2));
        do{
            //Printf("Phi: %f, Theta: %f",aParticle.fourMomentum.Phi(),aParticle.fourMomentum.Theta());

            TVector3 aPosition;
            aPosition = aParticle.positions.back() + (deltaTime*velocity/aParticle.fourMomentum.Vect().Mag()) * aParticle.fourMomentum.Vect();
            aParticle.positions.push_back(aPosition);
            
            double oldPhi = aParticle.fourMomentum.Phi();
            double oldTheta = aParticle.fourMomentum.Theta();
            TVector3 trajetory;
            trajetory.SetX( cos(oldPhi)*sin(oldTheta) );
            trajetory.SetY( sin(oldPhi)*sin(oldTheta)*cos(deltaTime*w) - cos(oldTheta)*sin(deltaTime*w) );
            trajetory.SetZ( cos(w*deltaTime)*cos(oldTheta) + sin(w*deltaTime)*sin(oldTheta)*sin(oldPhi) );
            aParticle.fourMomentum.SetTheta( acos(trajetory.Z()/trajetory.Mag()) );
            aParticle.fourMomentum.SetPhi( atan( trajetory.Y() / trajetory.X() ) );            
        }while(Hypot(aParticle.positions.back().Y(), aParticle.positions.back().Z())<CMSMagnets.at(i).externalRadius);
    }
    return aParticle;
}

particle adjustmentsToParticleTrajetories(particle aParticle){
    aParticle = adjustmentsFromCMSMagnets(aParticle);
    return aParticle;
}

int main()
{
    printf("--Program Started--\nCalculating...\n");
    clock_t tStart = clock();
    
    randomGenerator.SetSeed(0);

    setUserVaribles();
    
    setupParticleParameterHistograms();
    
    particleDataFile = new TFile("particleData.root","RECREATE");
    
    setupDetectorParticlePostionHistogramsAndGraphs();

    for (long long int p=0; p<numberOfTotalParticles; p++)
    {        
        particle currentParticle = getParticle();
        
        if (!useKnownCMSParticleParameters) {
            particleDataPhiHistogram->Fill(currentParticle.fourMomentum.Phi());
            particleDataThetaHistogram->Fill(currentParticle.fourMomentum.Theta());
            particleDataMomentumHistogram->Fill(unitConversion("kg m/v -> GeV",currentParticle.fourMomentum.Vect().Mag()));
        }
        if (calculateWithMagnets) {
            currentParticle = adjustmentsToParticleTrajetories(currentParticle);
        }
        
        for (int d=0; d<detectors.size(); d++) {
            TVector3 pointOfIntersection = getPointOfIntersectionOfParticleWithDetector(currentParticle, detectors.at(d));
            
            if (pointOfIntersectionIsInDetector(pointOfIntersection, detectors.at(d))) {
                detectors.at(d).numberOfParticlesEntered++;
                currentParticle.hitDetector.at(d) = true;
                currentParticle.positions.push_back(pointOfIntersection);
                
                for (int w=0; w<detectors.at(d).numberOfSubDetectorsAlongWidth; w++) {
                    for (int h=0; h<detectors.at(d).numberOfSubDetectorsAlongHeight; h++) {
                        if(pointOfIntersectionIsInDetector(pointOfIntersection, detectors.at(d).subDetectors.at((w*detectors.at(d).numberOfSubDetectorsAlongHeight)+h))){
                            detectors.at(d).subDetectors.at((w*detectors.at(d).numberOfSubDetectorsAlongHeight)+h).numberOfParticlesEntered++;
                        }
                        
                        TVector3 bottomLeftOfFacePoint, particlePositionOnDetectorFace;
                        bottomLeftOfFacePoint.SetX( detectors.at(d).lowestYZCorner.X() + detectors.at(d).depth*sin(detectorAlighnmentAngle) - detectors.at(d).width*cos(detectorAlighnmentAngle) );
                        bottomLeftOfFacePoint.SetY( detectors.at(d).lowestYZCorner.Y() + detectors.at(d).depth*cos(detectorAlighnmentAngle) + detectors.at(d).width*sin(detectorAlighnmentAngle) );
                        bottomLeftOfFacePoint.SetZ( detectors.at(d).lowestYZCorner.Z() );
                        
                        particlePositionOnDetectorFace.SetX( Hypot(pointOfIntersection.X() - bottomLeftOfFacePoint.X() , bottomLeftOfFacePoint.Y() - pointOfIntersection.Y()) );
                        particlePositionOnDetectorFace.SetY( pointOfIntersection.Z() - bottomLeftOfFacePoint.Z() );
                        
                        detectorsParticlePostions.at(d)->SetPoint(detectors.at(d).numberOfParticlesEntered-1, particlePositionOnDetectorFace.X(), particlePositionOnDetectorFace.Y());
                    }
                }
            }
        }
        
        particles.push_back(currentParticle);
    }
    
    drawDetectorsSetup();
    
    drawSubdetectorHitsWithTrajetories();
    
    particleDataPhiHistogram->Write();
    particleDataThetaHistogram->Write();
    particleDataMomentumHistogram->Write();
    
    enterAndDisplayDetectorParticlePostionHistogramsAndGraphs();
    
    particleDataFile->Close();
    
    printf("repetitions: %i\n", numberOfTotalParticles);
    for (int d=0; d<detectors.size(); d++)
    {
        printf("Detector %i: %f%%\n",(d+1), ((double) detectors.at(d).numberOfParticlesEntered)/numberOfTotalParticles*100);
    }
    printf("length: %f seconds\n" , ((double) (clock() - tStart))/CLOCKS_PER_SEC);
    return 0;
}