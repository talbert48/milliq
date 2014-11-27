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
#include <TLorentzVector.h>
#include <TVector3.h>

using namespace TMath;
using namespace std;

#ifndef MilliChargedParticleDetector_structsandfunctions_h
#define MilliChargedParticleDetector_structsandfunctions_h

TRandom3 randomGenerator;

enum class direction {negative=-1, neutral=0, positive=1};
double deltaTime;


//----------------------------------//
//---------------START--------------//
//--------------Structs-------------//

struct particle {
    TLorentzVector fourMomentum;
    double mass;
    double charge;
    vector<TVector3> positions;
    vector<bool> hitDetector;
};

struct detector {
    double width, depth, height;
    TVector3 lowestYZCorner;
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
    TVector3 lowestYZCorner;
};

struct CMSMagnet {
    double internalRadius, externalRadius;
    double strength;
    direction direction;
};

//--------------Structs-------------//
//----------------END---------------//
//----------------------------------//

bool displaySubDetetorsInSetup, displayAxesInSetup, calculateWithMagnets, drawAllParticlesPaths, drawDetectedParticlesPaths;
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
        value = orginalValue * ( pow(10,9)/C() );
    }else if (type == "kg m/v -> GeV"){
        value = orginalValue / ( pow(10,9)/C() );
    }else if (type == "GeV -> kg") {
        value = orginalValue * (double)( 1.782661845/pow(10,27) );
    }else if (type =="kg -> GeV") {
        value = orginalValue / ( 1.782661845/pow(10,27) );
    }else if (type == "e -> C") {
        value = orginalValue * ( 1.602176565/pow(10, 19) );
    }else if (type == "C -> e") {
        value = orginalValue / ( 1.602176565/pow(10, 19) );
    }
    return value;
}

TLorentzVector fourMomentumFromMometumMassThetaPhi(double momentum, double mass, double theta, double phi)
{
    TLorentzVector fourMomentum;
    fourMomentum.SetE(  Hypot(mass*pow(C(),2) , momentum*C()) );
    
    TVector3 p;
    p.SetMagThetaPhi(momentum, theta, phi);
    fourMomentum.SetVect(p);

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
    return randomGenerator.Gaus(Pi()/2,Pi()/8);//radians
}

double getPhi()
{
    return randomGenerator.Uniform(0,2*Pi());//radians
}


particle getParticle()
{
    double momentum                     = getMomentum();
    double theta                        = getTheta();
    double phi                          = getPhi();
    
    TVector3 initalPosition;
    initalPosition.SetXYZ(0,0,0);
    
    particle newParticle;
    newParticle.mass                    = particleMass;
    newParticle.charge                  = particleCharge;
    newParticle.fourMomentum            = fourMomentumFromMometumMassThetaPhi(momentum , newParticle.mass , theta , phi);
    newParticle.positions.push_back(initalPosition);
    newParticle.hitDetector.assign(detectors.size(),false);
    return newParticle;
}

//----------------------------------//
//---------------START--------------//
//-------------Detection------------//

bool pointOfIntersectionIsInDetector(TVector3 thePointOfIntersection,detector aDetector)
{
    //X
    if (thePointOfIntersection.X() >= aDetector.lowestYZCorner.X() + aDetector.depth*sin(detectorAlighnmentAngle) - aDetector.width*cos(detectorAlighnmentAngle)) {
        if (thePointOfIntersection.X() <= aDetector.lowestYZCorner.X() + aDetector.depth*sin(detectorAlighnmentAngle)) {
            //Y
            if (thePointOfIntersection.Y() >= aDetector.lowestYZCorner.Y() + aDetector.depth*cos(detectorAlighnmentAngle)) {
                if (thePointOfIntersection.Y() <= aDetector.lowestYZCorner.Y() + aDetector.depth*cos(detectorAlighnmentAngle) + aDetector.width*sin(detectorAlighnmentAngle)) {
                    //Z
                    if (thePointOfIntersection.Z() >= aDetector.lowestYZCorner.Z()) {
                        if (thePointOfIntersection.Z() <= aDetector.lowestYZCorner.Z() + aDetector.height) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

TVector3 getPointOfIntersectionOfParticleWithDetector(particle aParticle, detector aDetector)
{
    TVector3 particleTrajectory = aParticle.fourMomentum.Vect();
    TVector3 particlePosition   = aParticle.positions.back();
    
    TVector3 pointA, pointB , pointC;
    
    pointA.SetX( aDetector.lowestYZCorner.X() + aDetector.depth*sin(detectorAlighnmentAngle) );
    pointA.SetY( aDetector.lowestYZCorner.Y() + aDetector.depth*cos(detectorAlighnmentAngle) );
    pointA.SetZ( aDetector.lowestYZCorner.Z() );
    
    pointB.SetX( pointA.X() );
    pointB.SetY( pointA.Y() );
    pointB.SetZ( pointA.Z() + aDetector.height );
    
    pointC.SetX( pointA.X() - aDetector.width*cos(detectorAlighnmentAngle) );
    pointC.SetY( pointA.Y() + aDetector.width*sin(detectorAlighnmentAngle) );
    pointC.SetZ( pointA.Z() );
    
    TVector3 vectorAB, vectorAC, normal;
    
    vectorAB = pointB - pointA;
    
    vectorAC = pointC - pointA;
    
    normal.SetX(   (vectorAB.Y()*vectorAC.Z()) - (vectorAB.Z()*vectorAC.Y()) );
    normal.SetY( -((vectorAB.X()*vectorAC.Z()) - (vectorAB.Z()*vectorAC.X())) );
    normal.SetZ(   (vectorAB.X()*vectorAC.Y()) - (vectorAB.Y()*vectorAC.X()) );
    
    double mutiplyer = (normal.X()*(pointA.X()-particlePosition.X()) + normal.Y()*(pointA.Y()-particlePosition.Y()) + normal.Z()*(pointA.Z()-particlePosition.Z()))/(normal.X()*particleTrajectory.X() + normal.Y()*particleTrajectory.Y() + normal.Z()*particleTrajectory.Z());
    
    TVector3 pointOfIntersection;
    
    pointOfIntersection = particleTrajectory;
    pointOfIntersection *= mutiplyer;
    pointOfIntersection += particlePosition;
    
    return pointOfIntersection;
}

//-------------Detection------------//
//----------------END---------------//
//----------------------------------//



//----------------------------------//
//---------------START--------------//
//-------------Graphics-------------//

void drawBlockOnCanvasWithDimensions(TCanvas *aCanvas, TVector3 bottomRightFrontPoint, double width, double depth, double height, double angle, short color)
{
    TVector3 aPoint;
    TPolyLine3D *aLine;
    
    aCanvas->cd();
    
    //1
    aLine = new TPolyLine3D(2);
    aLine->SetLineWidth(1);
    aLine->SetLineColor(color);
    
    aPoint = bottomRightFrontPoint;
    aLine->SetPoint(0, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aPoint.SetX(aPoint.X() + depth*sin(angle));
    aPoint.SetY(aPoint.Y() + depth*cos(angle));
    aLine->SetPoint(1, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aLine->Draw("same");
    //2
    aLine = new TPolyLine3D(2);
    aLine->SetLineWidth(1);
    aLine->SetLineColor(color);
    
    aPoint = bottomRightFrontPoint;
    aPoint.SetZ(aPoint.Z() + height);
    aLine->SetPoint(0, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aPoint.SetX(aPoint.X() + depth*sin(angle));
    aPoint.SetY(aPoint.Y() + depth*cos(angle));
    aLine->SetPoint(1, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aLine->Draw("same");
    
    //3
    aLine = new TPolyLine3D(2);
    aLine->SetLineWidth(1);
    aLine->SetLineColor(color);
    
    aPoint = bottomRightFrontPoint;
    aPoint.SetX(aPoint.X() - width*cos(angle));
    aPoint.SetY(aPoint.Y() + width*sin(angle));
    aLine->SetPoint(0, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aPoint.SetX(aPoint.X() + depth*sin(angle));
    aPoint.SetY(aPoint.Y() + depth*cos(angle));
    aLine->SetPoint(1, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aLine->Draw("same");
    
    //4
    aLine = new TPolyLine3D(2);
    aLine->SetLineWidth(1);
    aLine->SetLineColor(color);
    
    aPoint = bottomRightFrontPoint;
    aPoint.SetX(aPoint.X() - width*cos(angle));
    aPoint.SetY(aPoint.Y() + width*sin(angle));
    aPoint.SetZ(aPoint.Z() + height);
    aLine->SetPoint(0, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aPoint.SetX(aPoint.X() + depth*sin(angle));
    aPoint.SetY(aPoint.Y() + depth*cos(angle));
    aLine->SetPoint(1, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aLine->Draw("same");
    
    //5
    aLine = new TPolyLine3D(2);
    aLine->SetLineWidth(1);
    aLine->SetLineColor(color);
    
    aPoint = bottomRightFrontPoint;
    aPoint.SetX(aPoint.X() + depth*sin(angle));
    aPoint.SetY(aPoint.Y() + depth*cos(angle));
    aLine->SetPoint(0, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aPoint.SetX(aPoint.X() - width*cos(angle));
    aPoint.SetY(aPoint.Y() + width*sin(angle));
    aLine->SetPoint(1, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aLine->Draw("same");
    
    //6
    aLine = new TPolyLine3D(2);
    aLine->SetLineWidth(1);
    aLine->SetLineColor(color);
    
    aPoint = bottomRightFrontPoint;
    aPoint.SetX(aPoint.X() + depth*sin(angle));
    aPoint.SetY(aPoint.Y() + depth*cos(angle));
    aPoint.SetZ(aPoint.Z() + height);
    aLine->SetPoint(0, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aPoint.SetX(aPoint.X() - width*cos(angle));
    aPoint.SetY(aPoint.Y() + width*sin(angle));
    aLine->SetPoint(1, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aLine->Draw("same");
    
    //7
    aLine = new TPolyLine3D(2);
    aLine->SetLineWidth(1);
    aLine->SetLineColor(color);
    
    aPoint = bottomRightFrontPoint;
    aLine->SetPoint(0, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aPoint.SetX(aPoint.X() - width*cos(angle));
    aPoint.SetY(aPoint.Y() + width*sin(angle));
    aLine->SetPoint(1, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aLine->Draw("same");
    
    //8
    aLine = new TPolyLine3D(2);
    aLine->SetLineWidth(1);
    aLine->SetLineColor(color);
    
    aPoint = bottomRightFrontPoint;
    aPoint.SetZ(aPoint.Z() + height);
    aLine->SetPoint(0, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aPoint.SetX(aPoint.X() - width*cos(angle));
    aPoint.SetY(aPoint.Y() + width*sin(angle));
    aLine->SetPoint(1, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aLine->Draw("same");
    
    //9
    aLine = new TPolyLine3D(2);
    aLine->SetLineWidth(1);
    aLine->SetLineColor(color);
    
    aPoint = bottomRightFrontPoint;
    aLine->SetPoint(0, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aPoint.SetZ(aPoint.Z() + height);
    aLine->SetPoint(1, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aLine->Draw("same");
    
    //10
    aLine = new TPolyLine3D(2);
    aLine->SetLineWidth(1);
    aLine->SetLineColor(color);
    
    aPoint = bottomRightFrontPoint;
    aPoint.SetX(aPoint.X() + depth*sin(angle));
    aPoint.SetY(aPoint.Y() + depth*cos(angle));
    aLine->SetPoint(0, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aPoint.SetZ(aPoint.Z() + height);
    aLine->SetPoint(1, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aLine->Draw("same");
    
    //11
    aLine = new TPolyLine3D(2);
    aLine->SetLineWidth(1);
    aLine->SetLineColor(color);
    
    aPoint = bottomRightFrontPoint;
    aPoint.SetX(aPoint.X() + depth*sin(angle));
    aPoint.SetY(aPoint.Y() + depth*cos(angle));
    aPoint.SetX(aPoint.X() - width*cos(angle));
    aPoint.SetY(aPoint.Y() + width*sin(angle));
    aLine->SetPoint(0, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aPoint.SetZ(aPoint.Z() + height);
    aLine->SetPoint(1, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aLine->Draw("same");
    
    //12
    aLine = new TPolyLine3D(2);
    aLine->SetLineWidth(1);
    aLine->SetLineColor(color);
    
    aPoint = bottomRightFrontPoint;
    aPoint.SetX(aPoint.X() - width*cos(angle));
    aPoint.SetY(aPoint.Y() + width*sin(angle));
    aLine->SetPoint(0, aPoint.X(), aPoint.Y(), aPoint.Z());
    
    aPoint.SetZ(aPoint.Z() + height);
    aLine->SetPoint(1, aPoint.X(), aPoint.Y(), aPoint.Z());
    
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
    
    firstDetectorBottomLeftCornerXDisplacement = detectorRoom.lowestYZCorner.Y()*tan(detectorAlighnmentAngle) + ((width/2)*(1/cos(detectorAlighnmentAngle)));
    firstDetectorBottomLeftCornerYDisplacement = detectorRoom.lowestYZCorner.Y();
    
    if(detectorAlighnmentAngle>=atan((detectorRoom.lowestYZCorner.X() + detectorRoom.width)/(detectorRoom.lowestYZCorner.Y()+detectorRoom.depth)))
    {
        lastDetectorBottomLeftCornerXDisplacement = detectorRoom.lowestYZCorner.X() + detectorRoom.width - (depth*sin(detectorAlighnmentAngle));
        lastDetectorBottomLeftCornerYDisplacement = ((detectorRoom.lowestYZCorner.X()+detectorRoom.width)*(1/tan(detectorAlighnmentAngle))) - ((width/2)*(1/sin(detectorAlighnmentAngle))) - (depth*cos(detectorAlighnmentAngle));
    }
    else
    {
        lastDetectorBottomLeftCornerXDisplacement = (detectorRoom.lowestYZCorner.Y()+detectorRoom.depth)*tan(detectorAlighnmentAngle) - (width/2)*(1/cos(detectorAlighnmentAngle)) - depth*sin(detectorAlighnmentAngle) + width*cos(detectorAlighnmentAngle);
        lastDetectorBottomLeftCornerYDisplacement = detectorRoom.lowestYZCorner.Y()+detectorRoom.depth - depth*cos(detectorAlighnmentAngle) - width*sin(detectorAlighnmentAngle);
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
        
        newDetector.lowestYZCorner.SetX( (detectorRoom.lowestYZCorner.Y()*tan(detectorAlighnmentAngle)) + ((newDetector.width/2)*(1/cos(detectorAlighnmentAngle))) + (detectorXSeperation*i) );
        newDetector.lowestYZCorner.SetY( detectorRoom.lowestYZCorner.Y() + (detectorYSeperation*i) );
        newDetector.lowestYZCorner.SetZ( detectorRoom.lowestYZCorner.Z() + detectorZDisplacement );
        
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
                
                newSubDetector.lowestYZCorner.SetX( newDetector.lowestYZCorner.X() - subDetectorWidth*cos(detectorAlighnmentAngle)*w );
                newSubDetector.lowestYZCorner.SetY( newDetector.lowestYZCorner.Y() + subDetectorWidth*sin(detectorAlighnmentAngle)*w );
                newSubDetector.lowestYZCorner.SetZ( newDetector.lowestYZCorner.Z() + subDetectorHeigth*h );
                
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
