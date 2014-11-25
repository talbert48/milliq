//
//  structsandfunctions.h
//  MilliChargedParticleDetector
//
//  Created by James London on 11/17/14.
//  Copyright (c) 2014 James London. All rights reserved.
//

#include <iostream>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <math.h>

#include <TMath.h>
#include <TRandom3.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TInterpreter.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TFrame.h>
#include <TGraph.h>
#include <TPolyLine3D.h>
#include <TPolyMarker3D.h>
#include <TColor.h>
#include <TMultiGraph.h>
#include <TText.h>

using namespace TMath;
using namespace std;

#ifndef MilliChargedParticleDetector_structsandfunctions_h
#define MilliChargedParticleDetector_structsandfunctions_h

TRandom3 randomGenerator;

enum class direction {negative=-1, neutral=0, positive=1};
double speedOfLight = 299792458;//m/s
double deltaTime;


//----------------------------------//
//---------------START--------------//
//--------------Structs-------------//

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
    vector<triplet> positions;
    vector<bool> hitDetector;
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

//--------------Structs-------------//
//----------------END---------------//
//----------------------------------//

bool displaySubDetetorsInSetup, displayAxesInSetup, calculateWithMagnets;
double detectorAlighnmentAngle;
detectorRoom detectorRoom;

vector<mainDetector> detectors;
vector<CMSMagnet> CMSMagnets;
vector<particle> particles;

int numberOfTotalParticles;
double particleMass;
double particleCharge;

double unitConversion(string type, double orginalValue)
{
    double value;
    if (type == "GeV -> kg m/v") {
        value = orginalValue * ( 1000000000.0/speedOfLight );
    }else if ("kg m/v -> GeV"){
        value = orginalValue / ( 1000000000.0/speedOfLight );
    }else if ("GeV -> kg") {
        value = orginalValue * ( 1.782661845/pow(10,27) );
    }else if ("kg -> GeV") {
        value = orginalValue / ( 1.782661845/pow(10,27) );
    }else if ("e -> C") {
        value = orginalValue * ( 1.602176565/pow(10, 19) );
    }else if ("C -> e") {
        value = orginalValue / ( 1.602176565/pow(10, 19) );
    }
    return value;
}

double tripletMagnitude(triplet momentumVetor)
{
    return pow( pow(momentumVetor.x,2)+pow(momentumVetor.y,2)+pow(momentumVetor.z,2) , 0.5);
}

fourMomentum fourMomentumFromMometumMassThetaPhi(double momentum, double mass, double theta, double phi)
{
    fourMomentum fourMomentum;
    fourMomentum.energy     = pow( pow(mass,2)*pow(speedOfLight,4) + pow(momentum,2)*pow(speedOfLight,2) , 0.5);
    fourMomentum.momentum.x = (sin(theta)*cos(phi))*momentum;
    fourMomentum.momentum.y = (sin(theta)*sin(phi))*momentum;
    fourMomentum.momentum.z = (cos(theta))*momentum;
    return fourMomentum;
}

double getMomentum()
{
    double p  = (randomGenerator.Landau(25,5));//GeV
    p = unitConversion("GeV -> kg m/v", p);
    return p;
}

double getTheta()
{
    return randomGenerator.Gaus(M_PI/2,M_PI/8);//radians
}

double getPhi()
{
    return randomGenerator.Uniform(0,2*M_PI);//radians
}


particle getParticle()
{
    double momentum                     = getMomentum();
    double theta                        = getTheta();
    double phi                          = getPhi();
    
    triplet initalPosition;
    initalPosition.x                    = 0;
    initalPosition.y                    = 0;
    initalPosition.z                    = 0;
    
    particle newParticle;
    newParticle.mass                    = particleMass;
    newParticle.charge                  = particleCharge;
    newParticle.fourMomentum            = fourMomentumFromMometumMassThetaPhi(momentum , newParticle.mass , theta , phi);
    newParticle.positions.push_back(initalPosition);
    newParticle.hitDetector.assign(detectors.size(),false);
    return newParticle;
}

double phiFromMomentum(triplet aMomentum){
    double r = pow(pow(aMomentum.x,2) + pow(aMomentum.y,2) + pow(aMomentum.z,2) , .5);
    return acos(aMomentum.z/r);
}

double thetaFromMomentum(triplet aMomentum){
    return acos(aMomentum.y/aMomentum.x);
}

//----------------------------------//
//---------------START--------------//
//-------------Detection------------//

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
    triplet particleTrajectory = aParticle.fourMomentum.momentum;
    triplet particlePosition   = aParticle.positions.back();
    
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
    
    double mutiplyer = (normal.x*(pointA.x-particlePosition.x) + normal.y*(pointA.y-particlePosition.y) + normal.z*(pointA.z-particlePosition.z))/(normal.x*particleTrajectory.x + normal.y*particleTrajectory.y + normal.z*particleTrajectory.z);
    
    triplet pointOfIntersection;
    
    pointOfIntersection = particleTrajectory;
    
    pointOfIntersection.x *= mutiplyer;
    pointOfIntersection.y *= mutiplyer;
    pointOfIntersection.z *= mutiplyer;
    
    pointOfIntersection.x += particlePosition.x;
    pointOfIntersection.y += particlePosition.y;
    pointOfIntersection.z += particlePosition.z;
    
    return pointOfIntersection;
}

//-------------Detection------------//
//----------------END---------------//
//----------------------------------//



//----------------------------------//
//---------------START--------------//
//-------------Graphics-------------//

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

//-------------Graphics-------------//
//----------------END---------------//
//----------------------------------//

//----------------------------------//
//---------------START--------------//
//-----------Initializer------------//

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

//-----------Initializer------------//
//----------------END---------------//
//----------------------------------//

#endif
