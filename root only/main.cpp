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
    
    deltaTime                           = Power(10, -10);//s
    
    numberOfTotalParticles              = 10;
    particleMass                        = 50;//GeV
    particleCharge                      = Power(10, -3);//e
    
    detectorAlighnmentTheta             = ((double)17/24)*Pi();//radians >0 and <Pi and !=Pi/2
    detectorAlighnmentPhi               = ((double)-15/16)*Pi();//radians !=Pi()/2
    
    detectorRoom.corners["FBL"].SetX(     -10);//m
    detectorRoom.corners["FBL"].SetY(     -5);//m
    detectorRoom.corners["FBL"].SetZ(     -5);//m
    detectorRoom.width                  = 10;//m
    detectorRoom.depth                  = 10;//m
    detectorRoom.height                 = 2;//m
    
    int numberOfDetectors               = 3;
    
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
    aCMSMagnet.direction                = -1;
    aCMSMagnet.internalRadius           = 0;//m
    aCMSMagnet.externalRadius           = 2;//m
    CMSMagnets.push_back(aCMSMagnet);
    
    aCMSMagnet.direction                = 1;
    aCMSMagnet.internalRadius           = aCMSMagnet.externalRadius;
    aCMSMagnet.externalRadius           = 4;//m
    CMSMagnets.push_back(aCMSMagnet);
    
    displayDetectorRoom                 = true;
    displayDetectorAlignmentAngle       = true;
    displaySubDetetorsInSetup           = false;
    displayAxesInSetup                  = true;
    
    drawAllParticlesPaths               = false;
    drawDetectedParticlesPaths          = true;
    
    useEventData                        = true;
    eventDataSize                       = 200000;
    eventDataFilePath                   = "/Users/JamesLondon/Documents/Milli Charged Particle Detector Project/milliq/events.csv";
    
    calculateWithMagnets                = true;
    
    //------------------------ USER MAY SET  ABOVE VARIBLES -------------------------//
    //-------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------//
    
    initializeDetectorsWith(numberOfDetectors, detectorWidths, detectorDepths, detectorHeights, numberOfSubDetectorsAlongWidth, numberOfSubDetectorsAlongHeight, subDetectorWidths, subDetectorDepths, subDetectorHeigths);
}

void momentumAfterDeltaTime(particle &aParticle, TVector3 magneticField)
{
    double w = aParticle.charge * magneticField.Mag() * Power(C(), 2) / (aParticle.fourMomentum.E()*Power(10,9));

    TVector3 newMomentum;
    newMomentum.SetX( Power(magneticField.Mag(),-2) * ((aParticle.fourMomentum.Px()*(    (Power(magneticField.Py(),2) + Power(magneticField.Pz(),2)) * Cos(deltaTime*w) + Power(magneticField.Px(),2)          ) ) +
                                                       (aParticle.fourMomentum.Py()*(      magneticField.Mag()*magneticField.Z()*Sin(deltaTime*w) + magneticField.X()*magneticField.Y()*(1.0-Cos(deltaTime*w)) ) ) +
                                                       (aParticle.fourMomentum.Pz()*( -1.0*magneticField.Mag()*magneticField.Y()*Sin(deltaTime*w) + magneticField.X()*magneticField.Z()*(1.0-Cos(deltaTime*w)) ) )
                                                       ));
    newMomentum.SetY( Power(magneticField.Mag(),-2) * ((aParticle.fourMomentum.Py()*(    (Power(magneticField.Px(),2) + Power(magneticField.Pz(),2)) * Cos(deltaTime*w) + Power(magneticField.Py(),2)          ) ) +
                                                       (aParticle.fourMomentum.Pz()*(      magneticField.Mag()*magneticField.X()*Sin(deltaTime*w) + magneticField.Y()*magneticField.Z()*(1.0-Cos(deltaTime*w)) ) ) +
                                                       (aParticle.fourMomentum.Px()*( -1.0*magneticField.Mag()*magneticField.Z()*Sin(deltaTime*w) + magneticField.Y()*magneticField.X()*(1.0-Cos(deltaTime*w)) ) )
                                                       ));
    newMomentum.SetZ( Power(magneticField.Mag(),-2) * ((aParticle.fourMomentum.Pz()*(    (Power(magneticField.Px(),2) + Power(magneticField.Py(),2)) * Cos(deltaTime*w) + Power(magneticField.Pz(),2)          ) ) +
                                                       (aParticle.fourMomentum.Px()*(      magneticField.Mag()*magneticField.Y()*Sin(deltaTime*w) + magneticField.Z()*magneticField.X()*(1.0-Cos(deltaTime*w)) ) ) +
                                                       (aParticle.fourMomentum.Py()*( -1.0*magneticField.Mag()*magneticField.X()*Sin(deltaTime*w) + magneticField.Z()*magneticField.Y()*(1.0-Cos(deltaTime*w)) ) )
                                                       ));
    
    aParticle.fourMomentum.SetVect(newMomentum);
}

particle adjustmentsFromCMSMagnets(particle aParticle, bool &stuckParticle)
{
	int itr = 0;
    //loop a array of the CMS magnets from origin to the last magnet adjusting the particle's momentum and position at increments of deltaTime until the particle is through all the CMS magnet's
    for (int i=0; i<(int)CMSMagnets.size(); i++) {
        do{
            //calculate the new momentum of the particle
            momentumAfterDeltaTime(aParticle , *new TVector3(0,0,CMSMagnets.at(i).direction*CMSMagnets.at(i).strength));

            //estimate the new position by assuming the particle was moving linearly for a duration of deltaTime along the momentum set above
            TVector3 velocity = C()*aParticle.fourMomentum.Vect()*(1.0/aParticle.fourMomentum.E());
            aParticle.positions.push_back(aParticle.positions.back() + (deltaTime*velocity));
			
            //if calculation exceeds a certain number of interations then say the particle is stuck and will be droped from the data set
            itr++;
            if(itr>1000){
                stuckParticle = true;
                return aParticle;
            }
        //check if the particle is outside the current CMS magnet
        }while(Hypot(aParticle.positions.back().Y(), aParticle.positions.back().Z())<CMSMagnets.at(i).externalRadius);
    }
    return aParticle;
}

particle adjustmentsToParticleTrajetories(particle aParticle, bool &stuckParticle){
    aParticle = adjustmentsFromCMSMagnets(aParticle, stuckParticle);
    return aParticle;
}

int main()
{    
    printf("--Program Started--\n\n");
    clock_t tStart = clock();
    
    randomGenerator.SetSeed(0);

    setUserVaribles();
	
    if (useEventData) {
        generateKnownCMSParticles();
    }
	
    setupParticleParameterHistograms();
    
    setupDetectorParticlePostionHistogramsAndGraphs();
	
	particleDataFile = new TFile("particleData.root","RECREATE");

    for (long long int p=0; p<numberOfTotalParticles; p++)
    {
        particle currentParticle = getParticle(p);
                
        bool stuckParticle = false;
        if (calculateWithMagnets) {
            currentParticle = adjustmentsToParticleTrajetories(currentParticle, stuckParticle);
        }
        
        if (!stuckParticle) {
            for (int d=0; d<(int)detectors.size(); d++) {
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
                            bottomLeftOfFacePoint = detectors.at(d).corners["BBL"];
                            
                            particlePositionOnDetectorFace.SetX( Hypot(pointOfIntersection.X() - bottomLeftOfFacePoint.X() , bottomLeftOfFacePoint.Y() - pointOfIntersection.Y()) );
                            particlePositionOnDetectorFace.SetY( pointOfIntersection.Z() - bottomLeftOfFacePoint.Z() );
                            
                            detectorsParticlePostions.at(d)->SetPoint(detectors.at(d).numberOfParticlesEntered-1, particlePositionOnDetectorFace.X(), particlePositionOnDetectorFace.Y());
                        }
                    }
                }
            }

            particles.push_back(currentParticle);
        }
    }
    
	printf("--Program Ending--\n\n");
    drawDetectorsSetup();
    
    drawSubdetectorHitsWithTrajetories();
    
    particleDataPhiHistogram->Write();
    particleDataThetaHistogram->Write();
    particleDataMomentumHistogram->Write();
    
    enterAndDisplayDetectorParticlePostionHistogramsAndGraphs();
    
    particleDataFile->Close();
    
    printf("Particles: %i\n", numberOfTotalParticles);
    for (int d=0; d<(int)detectors.size(); d++)
    {
        printf("Detector %i Hit Ratio: %f%%\n",(d+1), ((double) detectors.at(d).numberOfParticlesEntered)/numberOfTotalParticles*100);
    }
    printf("Execution Duration: %i seconds\n" , (int)(( (double)(clock() - tStart))/CLOCKS_PER_SEC));
    return 0;
}