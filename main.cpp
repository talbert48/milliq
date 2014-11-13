//
//  main.cpp
//  CMS Sidedetector
//
//  Created by James London on 8/25/14.
//  Copyright (c) 2014 James London. All rights reserved.
//

#include <iostream>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <math.h>

#include <TMath.h>
#include <TRandom3.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>

#include <TInterpreter.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TFrame.h>
#include <TGraph.h>
#include <TPolyLine3D.h>
#include <TPolyMarker3D.h>
#include <TColor.h>
#include <TMultiGraph.h>

using namespace TMath;
using namespace std;

enum class direction {negative=-1, positive=1};
double speedOfLight = 3*pow(10, 8);

struct triplet {
    double x;
    double y;
    double z;
};

struct fourMomentum {
    double energy;
    triplet momentum;
};

struct particle {
    fourMomentum fourMomentum;
    double mass;
    double charge;
};

struct detector {
    double width, depth, height;
    triplet lowestYZCorner;
    int numberOfParticlesEntered;
};

struct subDetector:detector {
};

struct mainDetector:detector {
    int numberOfSubDetectorsAlongWidth;
    int numberOfSubDetectorsAlongHeight;
    vector<subDetector> subDetectors;
    short color;
};

struct detectorRoom {
    double width, depth, height;
    triplet lowestYZCorner;
};

struct CMSMagnet {
    double internalRadius, externalRadius;
    double strength;
    direction direction;
};

TRandom3 randomGenerator;

bool displaySubDetetorsInSetup, displayAxesInSetup, drawAParticle;

double detectorAlighnmentAngle;

detectorRoom detectorRoom;

vector<mainDetector> detectors;

vector<CMSMagnet> CMSMagnets;

int numberOfTotalParticles;
double particleMass;
double particleCharge;

void initializeDetectorsWith(int numberOfDetectors, double width, double depth, double height, double detectorZDisplacement, int numberOfSubDetectorsAlongWidth, int numberOfSubDetectorsAlongHeight, double subDetectorWidth, double subDetectorDepth, double subDetectorHeigth)
{
    double detectorXSeperation;
    double detectorYSeperation;
    double firstDetectorBottomLeftCornerXDisplacement;
    double firstDetectorBottomLeftCornerYDisplacement;
    double lastDetectorBottomLeftCornerXDisplacement;
    double lastDetectorBottomLeftCornerYDisplacement;
    
    firstDetectorBottomLeftCornerXDisplacement = detectorRoom.lowestYZCorner.y*tan(detectorAlighnmentAngle) + ((width/2)*(1/cos(detectorAlighnmentAngle)));
    firstDetectorBottomLeftCornerYDisplacement = detectorRoom.lowestYZCorner.y;
    
    if(detectorAlighnmentAngle>=atan((detectorRoom.lowestYZCorner.x + detectorRoom.width)/(detectorRoom.lowestYZCorner.y+detectorRoom.depth)))
    {
        lastDetectorBottomLeftCornerXDisplacement = detectorRoom.lowestYZCorner.x + detectorRoom.width - (depth*sin(detectorAlighnmentAngle));
        lastDetectorBottomLeftCornerYDisplacement = ((detectorRoom.lowestYZCorner.x+detectorRoom.width)*(1/tan(detectorAlighnmentAngle))) - ((width/2)*(1/sin(detectorAlighnmentAngle))) - (depth*cos(detectorAlighnmentAngle));
    }
    else
    {
        lastDetectorBottomLeftCornerXDisplacement = (detectorRoom.lowestYZCorner.y+detectorRoom.depth)*tan(detectorAlighnmentAngle) - (width/2)*(1/cos(detectorAlighnmentAngle)) - depth*sin(detectorAlighnmentAngle) + width*cos(detectorAlighnmentAngle);
        lastDetectorBottomLeftCornerYDisplacement = detectorRoom.lowestYZCorner.y+detectorRoom.depth - depth*cos(detectorAlighnmentAngle) - width*sin(detectorAlighnmentAngle);
    }
    
    detectorXSeperation = (lastDetectorBottomLeftCornerXDisplacement-firstDetectorBottomLeftCornerXDisplacement)/(numberOfDetectors-1);
    detectorYSeperation = (lastDetectorBottomLeftCornerYDisplacement-firstDetectorBottomLeftCornerYDisplacement)/(numberOfDetectors-1);
    
    for(int i=0; i<numberOfDetectors; i++)
    {
        mainDetector newDetector;
        
        newDetector.numberOfParticlesEntered = 0;
        
        float r, g, b;
        TColor::HLStoRGB((float)(360*i)/(float)numberOfDetectors, (float).5, (float).5, r, g, b);
        newDetector.color = TColor::GetColor(r, g, b);
        
        newDetector.width = width;
        newDetector.depth = depth;
        newDetector.height = height;
        
        newDetector.lowestYZCorner.x = (detectorRoom.lowestYZCorner.y*tan(detectorAlighnmentAngle)) + ((newDetector.width/2)*(1/cos(detectorAlighnmentAngle))) + (detectorXSeperation*i);
        newDetector.lowestYZCorner.y = detectorRoom.lowestYZCorner.y + (detectorYSeperation*i);
        newDetector.lowestYZCorner.z = detectorRoom.lowestYZCorner.z + detectorZDisplacement;
        
        newDetector.numberOfSubDetectorsAlongWidth = numberOfSubDetectorsAlongWidth;
        newDetector.numberOfSubDetectorsAlongHeight = numberOfSubDetectorsAlongHeight;
        
        newDetector.subDetectors.resize(newDetector.numberOfSubDetectorsAlongHeight*newDetector.numberOfSubDetectorsAlongWidth);
        
        for (int w=0; w<numberOfSubDetectorsAlongWidth; w++) {
            for (int h=0; h<numberOfSubDetectorsAlongHeight; h++) {
                subDetector newSubDetector;
                
                newSubDetector.numberOfParticlesEntered = 0;
                
                newSubDetector.width = subDetectorWidth;
                newSubDetector.depth = subDetectorDepth;
                newSubDetector.height = subDetectorHeigth;
                
                newSubDetector.lowestYZCorner.x = newDetector.lowestYZCorner.x - subDetectorWidth*cos(detectorAlighnmentAngle)*w;
                newSubDetector.lowestYZCorner.y = newDetector.lowestYZCorner.y + subDetectorWidth*sin(detectorAlighnmentAngle)*w;
                newSubDetector.lowestYZCorner.z = newDetector.lowestYZCorner.z + subDetectorHeigth*h;
                
                newDetector.subDetectors.at((w*newDetector.numberOfSubDetectorsAlongHeight)+h) = newSubDetector;
            }
        }
        detectors.push_back(newDetector);
    }
}

void setUserVaribles()
{
    //                                        //
    //----- USER MAY SET BELLOW VARIBLES -----//
    //                                        //
    numberOfTotalParticles              = 10000;
    particleMass                        = 9.10938291*(10^-31);//kg
    particleCharge                      = (10^-3);//e
    
    detectorAlighnmentAngle             = M_PI/6;//radians
    
    detectorRoom.lowestYZCorner.x       = 0;  //m
    detectorRoom.lowestYZCorner.y       = 10; //m
    detectorRoom.lowestYZCorner.z       = 0;  //m
    detectorRoom.width                  = 10; //m
    detectorRoom.depth                  = 10; //m
    detectorRoom.height                 = 2;  //m
    
    int numberOfDetectors               = 3;
    
    double detectorZDisplacement        = 0;  //m
    
    double detectorWidths               = 1;  //m
    double detectorDepths               = 1.4;//m
    double detectorHeights              = 1;  //m
    
    double subDetectorWidths            = .1; //m
    double subDetectorDepths            = 1.4;//m
    double subDetectorHeigths           = .05;//m
    
    int numberOfSubDetectorsAlongWidth  = 10;
    int numberOfSubDetectorsAlongHeight = 20;
    
    displaySubDetetorsInSetup           = true;
    displayAxesInSetup                  = true;
    drawAParticle                       = false;
    
    //                                         //
    //-------- END VARIBLES USER MAY SET-------//
    //                                         //
    
    //2m , 4T
    
    initializeDetectorsWith(numberOfDetectors, detectorWidths, detectorDepths, detectorHeights, detectorZDisplacement, numberOfSubDetectorsAlongWidth, numberOfSubDetectorsAlongHeight, subDetectorWidths, subDetectorDepths, subDetectorHeigths);
    
}

double getMomentum()
{
    return pow(10,9)*(randomGenerator.Landau(0, 25));//eV
}

double getTheta()
{
    return randomGenerator.Gaus(M_PI/2,M_PI/8);//radians
}

double getPhi()
{
    return randomGenerator.Uniform(0,2*M_PI);//radians
}

fourMomentum fourMomentumFromEnergyThetaPhi(double energy, double theta, double phi)
{
    fourMomentum fourMomentum;
    fourMomentum.energy     = energy;
    fourMomentum.momentum.x = (sin(theta)*cos(phi))*energy;
    fourMomentum.momentum.y = (sin(theta)*sin(phi))*energy;
    fourMomentum.momentum.z = (cos(theta))*energy;
    return fourMomentum;
}

particle getParticle()
{
    double momentum                     = getMomentum();
    double theta                        = getTheta();
    double phi                          = getPhi();
    
    particle newParticle;
    newParticle.mass                    = particleMass;
    newParticle.charge                  = particleCharge;
    newParticle.fourMomentum.energy     = pow( pow(newParticle.mass,2)*pow(speedOfLight,4) + pow(momentum,2)*pow(speedOfLight,2) , .5);
    newParticle.fourMomentum            = fourMomentumFromEnergyThetaPhi(newParticle.fourMomentum.energy, theta, phi);

    return newParticle;
}

double phiFromMomentum(triplet aMomentum){
    double r = pow(pow(aMomentum.x,2) + pow(aMomentum.y,2) + pow(aMomentum.z,2) , .5);
    return acos(aMomentum.z/r);
}

double thetaFromMomentum(triplet aMomentum){
    return acos(aMomentum.y/aMomentum.x);
}

bool pointOfIntersectionIsInDetector(triplet thePointOfIntersection,detector aDetector)
{
    //X
    if (thePointOfIntersection.x >= aDetector.lowestYZCorner.x + aDetector.depth*sin(detectorAlighnmentAngle) - aDetector.width*cos(detectorAlighnmentAngle)) {
        if (thePointOfIntersection.x <= aDetector.lowestYZCorner.x + aDetector.depth*sin(detectorAlighnmentAngle)) {
            //Y
            if (thePointOfIntersection.y >= aDetector.lowestYZCorner.y + aDetector.depth*cos(detectorAlighnmentAngle)) {
                if (thePointOfIntersection.y <= aDetector.lowestYZCorner.y + aDetector.depth*cos(detectorAlighnmentAngle) + aDetector.width*sin(detectorAlighnmentAngle)) {
                    //Z
                    if (thePointOfIntersection.z >= aDetector.lowestYZCorner.z) {
                        if (thePointOfIntersection.z <= aDetector.lowestYZCorner.z + aDetector.height) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

triplet getPointOfIntersectionOfParticleWithDetector(particle aParticle, detector aDetector)
{
    triplet particleTrajectory;
    
    particleTrajectory = aParticle.fourMomentum.momentum;
    
    triplet pointA, pointB , pointC;
    
    pointA.x = aDetector.lowestYZCorner.x + aDetector.depth*sin(detectorAlighnmentAngle);
    pointA.y = aDetector.lowestYZCorner.y + aDetector.depth*cos(detectorAlighnmentAngle);
    pointA.z = aDetector.lowestYZCorner.z;

    pointB.x = pointA.x;
    pointB.y = pointA.y;
    pointB.z = pointA.z + aDetector.height;
    
    pointC.x = pointA.x - aDetector.width*cos(detectorAlighnmentAngle);
    pointC.y = pointA.y + aDetector.width*sin(detectorAlighnmentAngle);
    pointC.z = pointA.z;
    
    triplet vectorAB, vectorAC, normal;
    
    vectorAB.x = pointB.x - pointA.x;
    vectorAB.y = pointB.y - pointA.y;
    vectorAB.z = pointB.z - pointA.z;
    
    vectorAC.x = pointC.x - pointA.x;
    vectorAC.y = pointC.y - pointA.y;
    vectorAC.z = pointC.z - pointA.z;
    
    normal.x =   (vectorAB.y*vectorAC.z) - (vectorAB.z*vectorAC.y);
    normal.y = -((vectorAB.x*vectorAC.z) - (vectorAB.z*vectorAC.x));
    normal.z =   (vectorAB.x*vectorAC.y) - (vectorAB.y*vectorAC.x);
    
    double mutiplyer = (normal.x*pointA.x + normal.y*pointA.y + normal.z*pointA.z)/(normal.x*particleTrajectory.x + normal.y*particleTrajectory.y + normal.z*particleTrajectory.z);
    
    triplet pointOfIntersection;
    
    pointOfIntersection = particleTrajectory;
    
    pointOfIntersection.x *= mutiplyer;
    pointOfIntersection.y *= mutiplyer;
    pointOfIntersection.z *= mutiplyer;
    
    return pointOfIntersection;
}

void drawBlockOnCanvasWithDimensions(TCanvas *aCanvas, triplet bottomRightFrontPoint, double width, double depth, double height, double angle, short color)
{
    triplet aPoint;
    TPolyLine3D *aLine;
    
    aCanvas->cd();
    
    //1
        aLine = new TPolyLine3D(2);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(color);
    
        aPoint = bottomRightFrontPoint;
        aLine->SetPoint(0, aPoint.x, aPoint.y, aPoint.z);
        
        aPoint.x += depth*sin(angle);
        aPoint.y += depth*cos(angle);
        aLine->SetPoint(1, aPoint.x, aPoint.y, aPoint.z);
        
        aLine->Draw("same");
    //2
        aLine = new TPolyLine3D(2);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(color);
    
        aPoint = bottomRightFrontPoint;
        aPoint.z += height;
        aLine->SetPoint(0, aPoint.x, aPoint.y, aPoint.z);
        
        aPoint.x += depth*sin(angle);
        aPoint.y += depth*cos(angle);
        aLine->SetPoint(1, aPoint.x, aPoint.y, aPoint.z);
        
        aLine->Draw("same");
    
    //3
        aLine = new TPolyLine3D(2);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(color);
    
        aPoint = bottomRightFrontPoint;
        aPoint.x -= width*cos(angle);
        aPoint.y += width*sin(angle);
        aLine->SetPoint(0, aPoint.x, aPoint.y, aPoint.z);
        
        aPoint.x += depth*sin(angle);
        aPoint.y += depth*cos(angle);
        aLine->SetPoint(1, aPoint.x, aPoint.y, aPoint.z);
        
        aLine->Draw("same");
    
    //4
        aLine = new TPolyLine3D(2);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(color);
    
        aPoint = bottomRightFrontPoint;
        aPoint.x -= width*cos(angle);
        aPoint.y += width*sin(angle);
        aPoint.z += height;
        aLine->SetPoint(0, aPoint.x, aPoint.y, aPoint.z);
        
        aPoint.x += depth*sin(angle);
        aPoint.y += depth*cos(angle);
        aLine->SetPoint(1, aPoint.x, aPoint.y, aPoint.z);
        
        aLine->Draw("same");
    
    //5
        aLine = new TPolyLine3D(2);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(color);
    
        aPoint = bottomRightFrontPoint;
        aPoint.x += depth*sin(angle);
        aPoint.y += depth*cos(angle);
        aLine->SetPoint(0, aPoint.x, aPoint.y, aPoint.z);
        
        aPoint.x -= width*cos(angle);
        aPoint.y += width*sin(angle);
        aLine->SetPoint(1, aPoint.x, aPoint.y, aPoint.z);
        
        aLine->Draw("same");
    
    //6
        aLine = new TPolyLine3D(2);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(color);
    
        aPoint = bottomRightFrontPoint;
        aPoint.x += depth*sin(angle);
        aPoint.y += depth*cos(angle);
        aPoint.z += height;
        aLine->SetPoint(0, aPoint.x, aPoint.y, aPoint.z);
        
        aPoint.x -= width*cos(angle);
        aPoint.y += width*sin(angle);
        aLine->SetPoint(1, aPoint.x, aPoint.y, aPoint.z);
        
        aLine->Draw("same");
    
    //7
        aLine = new TPolyLine3D(2);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(color);
    
        aPoint = bottomRightFrontPoint;
        aLine->SetPoint(0, aPoint.x, aPoint.y, aPoint.z);
        
        aPoint.x -= width*cos(angle);
        aPoint.y += width*sin(angle);
        aLine->SetPoint(1, aPoint.x, aPoint.y, aPoint.z);
        
        aLine->Draw("same");
    
    //8
        aLine = new TPolyLine3D(2);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(color);
    
        aPoint = bottomRightFrontPoint;
        aPoint.z += height;
        aLine->SetPoint(0, aPoint.x, aPoint.y, aPoint.z);
        
        aPoint.x -= width*cos(angle);
        aPoint.y += width*sin(angle);
        aLine->SetPoint(1, aPoint.x, aPoint.y, aPoint.z);
        
        aLine->Draw("same");
    
    //9
        aLine = new TPolyLine3D(2);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(color);
    
        aPoint = bottomRightFrontPoint;
        aLine->SetPoint(0, aPoint.x, aPoint.y, aPoint.z);
        
        aPoint.z += height;
        aLine->SetPoint(1, aPoint.x, aPoint.y, aPoint.z);
        
        aLine->Draw("same");
    
    //10
        aLine = new TPolyLine3D(2);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(color);
    
        aPoint = bottomRightFrontPoint;
        aPoint.x += depth*sin(angle);
        aPoint.y += depth*cos(angle);
        aLine->SetPoint(0, aPoint.x, aPoint.y, aPoint.z);
        
        aPoint.z += height;
        aLine->SetPoint(1, aPoint.x, aPoint.y, aPoint.z);
        
        aLine->Draw("same");
    
    //11
        aLine = new TPolyLine3D(2);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(color);
    
        aPoint = bottomRightFrontPoint;
        aPoint.x += depth*sin(angle);
        aPoint.y += depth*cos(angle);
        aPoint.x -= width*cos(angle);
        aPoint.y += width*sin(angle);
        aLine->SetPoint(0, aPoint.x, aPoint.y, aPoint.z);
        
        aPoint.z += height;
        aLine->SetPoint(1, aPoint.x, aPoint.y, aPoint.z);
        
        aLine->Draw("same");
    
    //12
        aLine = new TPolyLine3D(2);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(color);
    
        aPoint = bottomRightFrontPoint;
        aPoint.x -= width*cos(angle);
        aPoint.y += width*sin(angle);
        aLine->SetPoint(0, aPoint.x, aPoint.y, aPoint.z);
        
        aPoint.z += height;
        aLine->SetPoint(1, aPoint.x, aPoint.y, aPoint.z);
 
        aLine->Draw("same");
 
}

void drawDetectorsSetup(){
    triplet aPoint;
    TPolyLine3D *aLine;
    
    TFile *detectorSetupFile = new TFile("detectorSetup.root","RECREATE");
    TCanvas *detectorSetupCanvas = new TCanvas("detectorSetupCanvas","Detector Setup Canvas",0,0,1000,700);
    
    detectorSetupCanvas->cd();
    
    //Particle Collision
        TPolyMarker3D *point = new TPolyMarker3D(1);
        point->SetPoint(0, 0, 0, 0);
        point->SetMarkerSize(2);
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
            aLine->SetLineColor(46);
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
        aLine->SetPoint(1, (detectorRoom.lowestYZCorner.y+detectorRoom.depth)*tan(detectorAlighnmentAngle), detectorRoom.lowestYZCorner.y+detectorRoom.depth, 0);
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
    
    //Draw a Particle
    if(drawAParticle){
        fourMomentum fourMomentum = fourMomentumFromEnergyThetaPhi(1,0,0);
        triplet particleTrajectory;
        particleTrajectory.x = fourMomentum.momentum.x/fourMomentum.energy;
        particleTrajectory.y = fourMomentum.momentum.y/fourMomentum.energy;
        particleTrajectory.z = fourMomentum.momentum.z/fourMomentum.energy;
        aLine = new TPolyLine3D(2);
        aLine->SetPoint(0, 0, 0, 0);
        aLine->SetPoint(1, particleTrajectory.x, particleTrajectory.y, particleTrajectory.z);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(2);
        aLine->Draw("same");
    }
    
    //Detector Room
        aPoint.x = detectorRoom.lowestYZCorner.x + detectorRoom.width;
        aPoint.y = detectorRoom.lowestYZCorner.y;
        aPoint.z = detectorRoom.lowestYZCorner.z;
        drawBlockOnCanvasWithDimensions(detectorSetupCanvas, aPoint, detectorRoom.width, detectorRoom.depth, detectorRoom.height, 0, 1);

    detectorSetupCanvas->Update();
    detectorSetupCanvas->Modified();
    detectorSetupCanvas->Write();
    detectorSetupFile->Close();
}

particle adjustmentsFromCMSMagnets(particle aParticle){
    double w = aParticle.charge*
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
    
    drawDetectorsSetup();
    
    TFile *particleDataFile = new TFile("particleDataHistogram.root","RECREATE");
    TCanvas *particleDataCanvas = new TCanvas("particleDataCanvas","Particle Data Canvas",0,0,2000,1300);
    particleDataCanvas->Divide(3,(int)(((2*detectors.size())+3)/3)+((((2*detectors.size())+3)%3)>0?1:0));

    TH1D *particleDataPhiHistogram = new TH1D("particleDataPhiHistogram", "Phi Distribution", 100, 0, M_PI);
    TH1D *particleDataThetaHistogram = new TH1D("particleDataThetaHistogram", "Theta Distribution", 100, 0, 2*M_PI);

    particleDataPhiHistogram->SetLineColor(1);
    particleDataThetaHistogram->SetLineColor(1);
    
    particleDataPhiHistogram->SetMinimum(0);
    particleDataThetaHistogram->SetMinimum(0);
    
    particleDataPhiHistogram->SetStats(kFALSE);
    particleDataThetaHistogram->SetStats(kFALSE);
    
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
        particle currentParticle = getParticle();
        
        particleDataPhiHistogram->Fill(phiFromMomentum(currentParticle.fourMomentum.momentum));
        particleDataThetaHistogram->Fill(thetaFromMomentum(currentParticle.fourMomentum.momentum));
        
        for (int d=0; d<detectors.size(); d++) {
            triplet pointOfIntersection = getPointOfIntersectionOfParticleWithDetector(currentParticle, detectors.at(d));
            if (pointOfIntersectionIsInDetector(pointOfIntersection, detectors.at(d))) {
                detectors.at(d).numberOfParticlesEntered++;

                for (int w=0; w<detectors.at(d).numberOfSubDetectorsAlongWidth; w++) {
                    for (int h=0; h<detectors.at(d).numberOfSubDetectorsAlongHeight; h++) {
                        if(pointOfIntersectionIsInDetector(pointOfIntersection, detectors.at(d).subDetectors.at((w*detectors.at(d).numberOfSubDetectorsAlongHeight)+h))){
                            detectors.at(d).subDetectors.at((w*detectors.at(d).numberOfSubDetectorsAlongHeight)+h).numberOfParticlesEntered++;
                        }
                        
                        triplet bottomLeftOfFacePoint, particlePositionOnDetectorFace;
                        bottomLeftOfFacePoint.x = detectors.at(d).lowestYZCorner.x + detectors.at(d).depth*sin(detectorAlighnmentAngle) - detectors.at(d).width*cos(detectorAlighnmentAngle);
                        bottomLeftOfFacePoint.y = detectors.at(d).lowestYZCorner.y + detectors.at(d).depth*cos(detectorAlighnmentAngle) + detectors.at(d).width*sin(detectorAlighnmentAngle);
                        bottomLeftOfFacePoint.z = detectors.at(d).lowestYZCorner.z;
                        
                        particlePositionOnDetectorFace.x = pow(pow(pointOfIntersection.x - bottomLeftOfFacePoint.x,2) + pow(bottomLeftOfFacePoint.y - pointOfIntersection.y,2), .5);
                        particlePositionOnDetectorFace.y = pointOfIntersection.z - bottomLeftOfFacePoint.z;
                        
                        detectorsParticlePostions.at(d)->SetPoint(detectors.at(d).numberOfParticlesEntered-1, particlePositionOnDetectorFace.x, particlePositionOnDetectorFace.y);
                    }
                }
            }
        }
    }

    particleDataCanvas->cd(1);
    particleDataPhiHistogram->Draw();
    particleDataCanvas->cd(2);
    particleDataThetaHistogram->Draw();
    
    double combinedDetectorsParticlePostionsGraphMaxX = 0, combinedDetectorsParticlePostionsGraphMaxY = 0;
    
    for (int d=0; d<detectors.size(); d++) {
        detectorsParticlePostions.at(d)->SetMarkerColor(detectors.at(d).color);
        detectorsParticlePostions.at(d)->SetMarkerStyle(2);
        detectorsParticlePostions.at(d)->SetMarkerSize(.5);
        char name[50];
        sprintf(name, "Detector %i Particle Positions", d+1);
        detectorsParticlePostions.at(d)->SetTitle(name);
        particleDataCanvas->cd(3+d);
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
        particleDataCanvas->cd(4+(int)detectors.size()+d);
        subdetectorHistograms.at(d)->Draw(/*"LEGO"*/"COLZ");
    }
    
    combinedDetectorsParticlePostionsGraph->SetTitle("Combined Detectors Particle Postions");
    particleDataCanvas->cd(3+(int)detectors.size());
    combinedDetectorsParticlePostionsGraph->Draw("AP*");
    combinedDetectorsParticlePostionsGraph->GetXaxis()->SetRangeUser(0,combinedDetectorsParticlePostionsGraphMaxX);
    combinedDetectorsParticlePostionsGraph->GetYaxis()->SetRangeUser(0,combinedDetectorsParticlePostionsGraphMaxY);
    combinedDetectorsParticlePostionsGraph->Draw("AP*");
    
    
    particleDataCanvas->Update();
    particleDataCanvas->Modified();
    particleDataCanvas->Write();
    particleDataCanvas->Print("particleDataHistograms.jpeg");
    particleDataFile->Close();
    
    printf("repetitions: %i\n", numberOfTotalParticles);
    for (int d=0; d<detectors.size(); d++)
    {
        printf("Detector %i: %f%%\n",(d+1), ((double) detectors.at(d).numberOfParticlesEntered)/numberOfTotalParticles*100);
    }
    printf("length: %f seconds\n" , ((double) (clock() - tStart))/CLOCKS_PER_SEC);
    return 0;
}