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
    
    deltaTime                           = 1.0/pow(10, 10);//s
    
    numberOfTotalParticles              = pow(10, 1);
    particleMass                        = 1;//GeV
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
    
    displaySubDetetorsInSetup           = true;
    displayAxesInSetup                  = true;
    calculateWithMagnets                = true;
    drawAllParticlesPaths               = true;
    drawDetectedParticlesPaths          = true;
    
    //------------------------ USER MAY SET  ABOVE VARIBLES -------------------------//
    //-------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------//
    
    //Convert
    particleMass = unitConversion("GeV -> kg", particleMass);
    particleCharge = unitConversion("e -> C", particleCharge);
    
    initializeDetectorsWith(numberOfDetectors, detectorWidths, detectorDepths, detectorHeights, detectorZDisplacement, numberOfSubDetectorsAlongWidth, numberOfSubDetectorsAlongHeight, subDetectorWidths, subDetectorDepths, subDetectorHeigths);
}

void drawDetectorsSetup()
{
    TVector3 aPoint;
    TPolyLine3D *aLine;
    
    TFile *detectorSetupFile = new TFile("detectorSetup.root","RECREATE");
    TCanvas *detectorSetupCanvas = new TCanvas("detectorSetupCanvas","Detector Setup Canvas",0,0,1000,700);
    
    detectorSetupCanvas->cd();
    
    if(drawAllParticlesPaths || drawDetectedParticlesPaths){
        for(int i=0; i<particles.size(); i++){
            if (drawAllParticlesPaths || (drawDetectedParticlesPaths && particles.at(i).hitDetector.at(0))) {
                aLine = new TPolyLine3D((int)particles.at(i).positions.size());
                for (int c=0; c<particles.at(i).positions.size(); c++) {
                    aLine->SetPoint(c, particles.at(i).positions.at(c).X(), particles.at(i).positions.at(c).Y(), particles.at(i).positions.at(c).Z());
                }
                aLine->SetLineWidth(2);
                aLine->SetLineColor(TColor::GetColor(.5, .5, (float)i/particles.size()));
                aLine->Draw("same");
            }
        }
    }
    
    //Particle Collision
    TPolyMarker3D *point = new TPolyMarker3D(1);
    point->SetPoint(0, 0, 0, 0);
    point->SetMarkerSize(0);
    point->SetMarkerColor(6);
    point->SetMarkerStyle(8);
    point->Draw("same");
    
    //Axis Indicators
    if (displayAxesInSetup) {
        //X
        aLine = new TPolyLine3D(2);
        aLine->SetPoint(0, 0, 0, 0);
        aLine->SetPoint(1, 5, 0, 0);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(2);
        aLine->Draw("same");
        //Y
        aLine = new TPolyLine3D(2);
        aLine->SetPoint(0, 0, 0, 0);
        aLine->SetPoint(1, 0, 5, 0);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(3);
        aLine->Draw("same");
        //Z
        aLine = new TPolyLine3D(2);
        aLine->SetPoint(0, 0, 0, 0);
        aLine->SetPoint(1, 0, 0, 5);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(4);
        aLine->Draw("same");
    }
    
    //Detector Alighnment Angle
    aLine = new TPolyLine3D(2);
    aLine->SetPoint(0, 0, 0, 0);
    aLine->SetPoint(1, (detectorRoom.lowestYZCorner.Y()+detectorRoom.depth)*tan(detectorAlighnmentAngle), detectorRoom.lowestYZCorner.Y()+detectorRoom.depth, 0);
    aLine->SetLineWidth(1);
    aLine->SetLineColor(2);
    aLine->Draw("same");
    
    //Detectors
    for (int i = (int)detectors.size()-1 ; i>=0; i--) {
        if(displaySubDetetorsInSetup){
            for (int w=0; w<detectors.at(i).numberOfSubDetectorsAlongWidth; w++) {
                for (int h=0; h<detectors.at(i).numberOfSubDetectorsAlongHeight; h++) {
                    drawBlockOnCanvasWithDimensions(detectorSetupCanvas, detectors.at(i).subDetectors.at((w*detectors.at(i).numberOfSubDetectorsAlongHeight)+h).lowestYZCorner, detectors.at(i).subDetectors.at((w*detectors.at(i).numberOfSubDetectorsAlongHeight)+h).width, detectors.at(i).subDetectors.at((w*detectors.at(i).numberOfSubDetectorsAlongHeight)+h).depth, detectors.at(i).subDetectors.at((w*detectors.at(i).numberOfSubDetectorsAlongHeight)+h).height, detectorAlighnmentAngle, detectors.at(i).color);
                }
            }
        }
        //Draw Main Detector Volume
        drawBlockOnCanvasWithDimensions(detectorSetupCanvas, detectors.at(i).lowestYZCorner, detectors.at(i).width, detectors.at(i).depth, detectors.at(i).height, detectorAlighnmentAngle, detectors.at(i).color);
    }
    
    //Detector Room
    aPoint.SetX( detectorRoom.lowestYZCorner.X() + detectorRoom.width);
    aPoint.SetY( detectorRoom.lowestYZCorner.Y());
    aPoint.SetZ( detectorRoom.lowestYZCorner.Z());
    drawBlockOnCanvasWithDimensions(detectorSetupCanvas, aPoint, detectorRoom.width, detectorRoom.depth, detectorRoom.height, 0, 1);
    
    detectorSetupCanvas->Update();
    detectorSetupCanvas->Modified();
    detectorSetupCanvas->Write();
    detectorSetupFile->Close();
}

particle adjustmentsFromCMSMagnets(particle aParticle)
{
    int count=0;

    particle p = aParticle;
    for (int i=0; i<CMSMagnets.size(); i++) {
        double w = aParticle.charge * CMSMagnets.at(i).strength * (double)CMSMagnets.at(i).direction * pow(C(), 2) / aParticle.fourMomentum.E();
        do{
            count++;
            TVector3 velocityVector = aParticle.fourMomentum.BoostVector();
            /*velocityVector.SetX( aParticle.fourMomentum.Px()/pow( pow(aParticle.mass,2) + pow(aParticle.fourMomentum.Px()/C(), 2) ,0.5) );//Vx = Px / ( m^2 + (Px/c)^2 )^(1/2)
            velocityVector.SetY( aParticle.fourMomentum.Py()/pow( pow(aParticle.mass,2) + pow(aParticle.fourMomentum.Py()/C(), 2) ,0.5) );//Vy = Py / ( m^2 + (Py/c)^2 )^(1/2)
            velocityVector.SetZ( aParticle.fourMomentum.Pz()/pow( pow(aParticle.mass,2) + pow(aParticle.fourMomentum.Pz()/C(), 2) ,0.5) );//Vz = Pz / ( m^2 + (Pz/c)^2 )^(1/2)
            Printf("x:%f y:%f z:%f",velocityVector.X(),velocityVector.Y(),velocityVector.Z());
            
            Printf("mag:%f bc:%f",velocityVector.Mag(),aParticle.fourMomentum.Beta()*C());

            
            velocityVector = aParticle.fourMomentum.BoostVector();
            Printf("x:%f y:%f z:%f",velocityVector.X(),velocityVector.Y(),velocityVector.Z());
            
            Printf("mag:%f bc:%f",velocityVector.Mag(),aParticle.fourMomentum.Beta()*C());
            */
            
            
            TVector3 aPosition;
            aPosition = aParticle.positions.back() + deltaTime*velocityVector;
            aParticle.positions.push_back(aPosition);
            
            double oldPhi = aParticle.fourMomentum.Phi();
            double oldTheta = aParticle.fourMomentum.Theta();
            aParticle.fourMomentum.SetPx( aParticle.mass*(cos(oldPhi)*sin(oldTheta)) );
            aParticle.fourMomentum.SetPy( aParticle.mass*(sin(oldPhi)*sin(oldTheta)*cos(deltaTime*w) + cos(oldTheta)*sin(deltaTime*w)) );
            aParticle.fourMomentum.SetPz( aParticle.fourMomentum.Pz());
        }while(Hypot(aParticle.positions.back().Y(), aParticle.positions.back().Z())<CMSMagnets.at(i).externalRadius);
    }
    Printf("%i",count);
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

    int cd = 1;
    TFile *particleDataFile = new TFile("particleDataHistogram.root","RECREATE");
    TCanvas *particleDataCanvas = new TCanvas("particleDataCanvas","Particle Data Canvas",0,0,2000,1300);
    particleDataCanvas->Divide(3,(int)(((2*detectors.size())+4)/3)+((((2*detectors.size())+4)%3)>0?1:0));
    
    TH1D *particleDataPhiHistogram = new TH1D("particleDataPhiHistogram", "Phi Distribution", 100, 0, Pi());
    TH1D *particleDataThetaHistogram = new TH1D("particleDataThetaHistogram", "Theta Distribution", 100, 0, 2*Pi());
    TH1D *particleDataMomentumHistogram = new TH1D("particleDataMomentumHistogram", "Momentum Distribution", 100, 1, 100);

    particleDataPhiHistogram->SetLineColor(1);
    particleDataThetaHistogram->SetLineColor(1);
    particleDataMomentumHistogram->SetLineColor(1);
    
    particleDataPhiHistogram->SetMinimum(0);
    particleDataThetaHistogram->SetMinimum(0);
    particleDataMomentumHistogram->SetMinimum(0);
    
    particleDataPhiHistogram->SetStats(kFALSE);
    particleDataThetaHistogram->SetStats(kFALSE);
    particleDataMomentumHistogram->SetStats(kFALSE);

    vector<TH2D*> subdetectorHistograms(detectors.size());
    
    vector<TGraph*> detectorsParticlePostions(detectors.size());
    
    TMultiGraph *combinedDetectorsParticlePostionsGraph = new TMultiGraph();
    
    for (int d=0; d<detectors.size(); d++) {
        char identifiyer[50];
        sprintf(identifiyer, "subdetectorHistograms.at(%i)", d+1);
        char name[50];
        sprintf(name, "Detector %i Subdetector Detections", d+1);
        subdetectorHistograms.at(d) = new TH2D(identifiyer, name, detectors.at(d).numberOfSubDetectorsAlongWidth, (double)0, detectors.at(d).width, detectors.at(d).numberOfSubDetectorsAlongHeight, (double)0, detectors.at(d).height);
        subdetectorHistograms.at(d)->SetStats(kFALSE);
        detectorsParticlePostions.at(d) = new TGraph();
    }

    for (long long int p=0; p<numberOfTotalParticles; p++)
    {
        //printf("time of Particle %i: %f seconds\n" ,p, ((double) (clock() - tStart))/CLOCKS_PER_SEC);
        
        particle currentParticle = getParticle();
        
        particleDataPhiHistogram->Fill(currentParticle.fourMomentum.Phi());
        particleDataThetaHistogram->Fill(currentParticle.fourMomentum.Theta());
        particleDataMomentumHistogram->Fill(unitConversion("kg m/v -> GeV",currentParticle.fourMomentum.Vect().Mag()));
        
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
    
    particleDataCanvas->cd(cd);
    cd++;
    particleDataPhiHistogram->Draw();
    particleDataCanvas->cd(cd);
    cd++;
    particleDataThetaHistogram->Draw();
    particleDataCanvas->cd(cd);
    cd++;
    particleDataMomentumHistogram->Draw();
    
    double combinedDetectorsParticlePostionsGraphMaxX = 0, combinedDetectorsParticlePostionsGraphMaxY = 0;
    
    for (int d=0; d<detectors.size(); d++) {
        detectorsParticlePostions.at(d)->SetMarkerColor(detectors.at(d).color);
        detectorsParticlePostions.at(d)->SetMarkerStyle(2);
        detectorsParticlePostions.at(d)->SetMarkerSize(.5);
        char name[50];
        sprintf(name, "Detector %i Particle Positions", d+1);
        detectorsParticlePostions.at(d)->SetTitle(name);
        particleDataCanvas->cd(cd);
        detectorsParticlePostions.at(d)->Draw("AP*");
        detectorsParticlePostions.at(d)->GetXaxis()->SetRangeUser(0,detectors.at(d).width);
        detectorsParticlePostions.at(d)->GetYaxis()->SetRangeUser(0,detectors.at(d).height);
        detectorsParticlePostions.at(d)->Draw("AP*");
        
        combinedDetectorsParticlePostionsGraph->Add(detectorsParticlePostions.at(d));
        
        if (combinedDetectorsParticlePostionsGraphMaxX < detectors.at(d).width) {
            combinedDetectorsParticlePostionsGraphMaxX = detectors.at(d).width;
        }
        if (combinedDetectorsParticlePostionsGraphMaxY < detectors.at(d).height) {
            combinedDetectorsParticlePostionsGraphMaxY = detectors.at(d).height;
        }
        
        for (int w=0; w<detectors.at(d).numberOfSubDetectorsAlongWidth; w++) {
            for (int h=0; h<detectors.at(d).numberOfSubDetectorsAlongHeight; h++) {
                subdetectorHistograms.at(d)->SetBinContent(w+1, h+1, detectors.at(d).subDetectors.at((w*detectors.at(d).numberOfSubDetectorsAlongHeight)+h).numberOfParticlesEntered);
            }
        }
        particleDataCanvas->cd(cd+(int)detectors.size());
        cd++;
        subdetectorHistograms.at(d)->Draw("COLZ");
    }
    cd += (int)detectors.size();
    
    combinedDetectorsParticlePostionsGraph->SetTitle("Combined Detectors Particle Postions");
    particleDataCanvas->cd(cd);
    cd++;
    combinedDetectorsParticlePostionsGraph->Draw("AP*");
    combinedDetectorsParticlePostionsGraph->GetXaxis()->SetRangeUser(0,combinedDetectorsParticlePostionsGraphMaxX);
    combinedDetectorsParticlePostionsGraph->GetYaxis()->SetRangeUser(0,combinedDetectorsParticlePostionsGraphMaxY);
    combinedDetectorsParticlePostionsGraph->Draw("AP*");
    
    
    particleDataCanvas->Update();
    particleDataCanvas->Modified();
    particleDataCanvas->Write();
    particleDataCanvas->Print("particleDataHistograms.png","png");
    particleDataFile->Write();
    particleDataFile->Close();
    
    printf("repetitions: %i\n", numberOfTotalParticles);
    for (int d=0; d<detectors.size(); d++)
    {
        printf("Detector %i: %f%%\n",(d+1), ((double) detectors.at(d).numberOfParticlesEntered)/numberOfTotalParticles*100);
    }
    printf("length: %f seconds\n" , ((double) (clock() - tStart))/CLOCKS_PER_SEC);
    return 0;
}