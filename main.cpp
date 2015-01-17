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
    
    deltaTime                           = Power(10, -11);//s
    
    numberOfTotalParticles              = 100;
    particleMass                        = 50;//GeV
    particleCharge                      = Power(10, 0);//e
    
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
    aCMSMagnet.direction                = 1;
    aCMSMagnet.internalRadius           = 0;//m
    aCMSMagnet.externalRadius           = 8;//m
    CMSMagnets.push_back(aCMSMagnet);
    
    aCMSMagnet.direction                = 1;
    aCMSMagnet.internalRadius           = aCMSMagnet.externalRadius;
    aCMSMagnet.externalRadius           = 16;//m
    CMSMagnets.push_back(aCMSMagnet);
    
    displayDetectorRoom                 = true;
    displayDetectorAlignmentAngle       = false;
    displaySubDetetorsInSetup           = false;
    displayAxesInSetup                  = true;
    
    drawAllParticlesPaths               = true;
    drawDetectedParticlesPaths          = false;
    
    calculateWithMagnets                = true;
    
    
    //------------------------ USER MAY SET  ABOVE VARIBLES -------------------------//
    //-------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------//
    
    initializeDetectorsWith(numberOfDetectors, detectorWidths, detectorDepths, detectorHeights, detectorZDisplacement, numberOfSubDetectorsAlongWidth, numberOfSubDetectorsAlongHeight, subDetectorWidths, subDetectorDepths, subDetectorHeigths);
}

particle adjustmentsFromCMSMagnets(particle aParticle)
{
    double velocity = C()*aParticle.fourMomentum.Vect().Mag()/aParticle.fourMomentum.E();
    double momentumMagnitude = aParticle.fourMomentum.Vect().Mag();
    double phi = aParticle.fourMomentum.Phi();
    double theta = aParticle.fourMomentum.Theta();
    
    //loop a array of the CMS magnets from origin to the last magnet adjusting the particle's trajectory and position at increments of deltaTime until the particle is through all the CMS magnet's
    for (int i=0; i<CMSMagnets.size(); i++) {
        double w = aParticle.charge * CMSMagnets.at(i).strength * CMSMagnets.at(i).direction * Power(C(), 2) / (aParticle.fourMomentum.E()*Power(10,9));
        do{
            //calculate the new trajectory of the particle
            TVector3 trajetory;
            trajetory.SetX( Cos(phi)*Sin(theta) );
            trajetory.SetY( Sin(phi)*Sin(theta)*Cos(deltaTime*w) - Cos(theta)*Sin(deltaTime*w) );
            trajetory.SetZ( Cos(w*deltaTime)*Cos(theta) + Sin(w*deltaTime)*Sin(theta)*Sin(phi) );
            aParticle.fourMomentum.SetVect(trajetory*momentumMagnitude);
            
            //estimate the new position by assuming the particle was moving linearly for a duration of deltaTime along the trajectory set above
            aParticle.positions.push_back(aParticle.positions.back() + (deltaTime*velocity) * aParticle.fourMomentum.Vect().Unit());
            
            //set the new values for phi and theta based on the trajectory set above
            phi = aParticle.fourMomentum.Phi();
            theta = aParticle.fourMomentum.Theta();
            
        //check if the particle is outside the current CMS magnet
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
	
    generateKnownCMSParticles();
	
    setupParticleParameterHistograms();
    
    particleDataFile = new TFile("particleData.root","RECREATE");
    
    setupDetectorParticlePostionHistogramsAndGraphs();

    for (long long int p=0; p<numberOfTotalParticles; p++)
    {        
        particle currentParticle = getParticle(p);
        
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