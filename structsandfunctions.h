//
//  structsandfunctions.h
//  MilliChargedParticleDetector

#include <iostream>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <string>

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
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TDirectory.h>
#include "Riostream.h"

using namespace TMath;
using namespace std;

#ifndef MilliChargedParticleDetector_structsandfunctions_h
#define MilliChargedParticleDetector_structsandfunctions_h

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
    double direction;
};

//------------Structs END-----------//

//---------Global Varibles----------//

double deltaTime;

TRandom3 randomGenerator;

TFile *particleDataFile;
vector<TH2D*> subdetectorHistograms;
TMultiGraph *combinedDetectorsParticlePostionsGraph;
vector<TGraph*> detectorsParticlePostions;

detectorRoom detectorRoom;
double detectorAlighnmentAngle;
bool displayDetectorRoom, displayDetectorAlignmentAngle, displaySubDetetorsInSetup, displayAxesInSetup, calculateWithMagnets, drawAllParticlesPaths, drawDetectedParticlesPaths;

vector<mainDetector> detectors;
vector<CMSMagnet> CMSMagnets;
vector<particle> particles;

int numberOfTotalParticles;

double particleMass;
double particleCharge;

bool useEventData;
string eventDataFilePath;
int eventDataSize;
vector<vector<double>> particleDataArray;

TH1D *particleDataPhiHistogram, *particleDataThetaHistogram, *particleDataMomentumHistogram;

//-------Global Varibles END--------//

particle getParticle(long long int particleNumber)
{
    particle newParticle;
    newParticle.mass                    = particleMass;
    newParticle.charge                  = particleCharge;
    
    TLorentzVector fourMomentumTemp;
    if (useEventData) {
        fourMomentumTemp.SetPxPyPzE(particleDataArray[particleNumber][2], particleDataArray[particleNumber][1], particleDataArray[particleNumber][0], particleDataArray[particleNumber][3]);
    }else{
        double tempMomentum = randomGenerator.Landau(100,50);
        fourMomentumTemp.SetE(  Hypot(newParticle.mass*Power(C(),2) , tempMomentum*C()) );
        
        TVector3 p;
        p.SetMagThetaPhi( tempMomentum, randomGenerator.Gaus(Pi()/2,Pi()/8) , randomGenerator.Uniform(0,2*Pi()) );
        fourMomentumTemp.SetVect(p);
    }
    
    newParticle.fourMomentum            = fourMomentumTemp;
    
    TVector3 initalPosition;
    initalPosition.SetXYZ(0,0,0);
    
    newParticle.positions.push_back(initalPosition);
    newParticle.hitDetector.assign(detectors.size(),false);
	
	particleDataMomentumHistogram->Fill(newParticle.fourMomentum.Vect().Mag());
	particleDataThetaHistogram->Fill(newParticle.fourMomentum.Theta());
	particleDataPhiHistogram->Fill(newParticle.fourMomentum.Phi());
    
    return newParticle;
}

void generateKnownCMSParticles()
{
	//read in file containing particle data
	ifstream file;
    file.open(eventDataFilePath);
    if(!file.is_open()){
        useEventData = false;
        Printf("Particle Event Data: Could Not Load Data from \"%s\"\n",eventDataFilePath.c_str());
        return;
    }
	string value;
	int itr = 0;
	while ( file.good() )
	{
        vector<double> tempArray(5);
		for(int i=0; i<5; i++)
		{
			getline( file, value, ',' );
			tempArray[i] = stod(value);
		}
        particleDataArray.push_back(tempArray);
		itr++;
		if(itr == eventDataSize)
		{break;}
	}
    file.close();
	Printf("Particle Event Data: Loaded\n");
}

void setupParticleParameterHistograms()
{
    particleDataPhiHistogram = new TH1D("Phi Histogram", "Phi Distribution", 100, 0, Pi());
    particleDataThetaHistogram = new TH1D("Theta Histogram", "Theta Distribution", 100, -Pi()/2,Pi()/2);
    particleDataMomentumHistogram = new TH1D("Momentum Histogram", "Momentum Distribution", 100, 0, 1000);
    
    particleDataPhiHistogram->SetLineColor(1);
    particleDataThetaHistogram->SetLineColor(1);
    particleDataMomentumHistogram->SetLineColor(1);
    
    particleDataPhiHistogram->SetMinimum(0);
    particleDataThetaHistogram->SetMinimum(0);
    particleDataMomentumHistogram->SetMinimum(0);
    
    particleDataPhiHistogram->SetStats(kFALSE);
    particleDataThetaHistogram->SetStats(kFALSE);
    particleDataMomentumHistogram->SetStats(kFALSE);
    
}

void setupDetectorParticlePostionHistogramsAndGraphs()
{
    subdetectorHistograms.resize(detectors.size());
    
    detectorsParticlePostions.resize(detectors.size());
    
    combinedDetectorsParticlePostionsGraph = new TMultiGraph();
    
    for (int d=0; d<(int)detectors.size(); d++) {
        char identifiyer[50];
        sprintf(identifiyer, "subdetectorHistograms.at(%i)", d+1);
        char name[50];
        sprintf(name, "Detector %i Subdetector Detections", d+1);
        subdetectorHistograms.at(d) = new TH2D(identifiyer, name, detectors.at(d).numberOfSubDetectorsAlongWidth, (double)0, detectors.at(d).width, detectors.at(d).numberOfSubDetectorsAlongHeight, (double)0, detectors.at(d).height);
        subdetectorHistograms.at(d)->SetStats(kFALSE);
        detectorsParticlePostions.at(d) = new TGraph();
    }
}

void enterAndDisplayDetectorParticlePostionHistogramsAndGraphs()
{
    int cd = 1;
    TCanvas *detectorCollisionPositionsCanvas = new TCanvas("Detector Collision Positions","Detect Collision Positions",0,0,2000,1300);
    detectorCollisionPositionsCanvas->Divide(3,(int)(((2*detectors.size()))/3)+((((2*detectors.size()))%3)>0?1:0));
    
    double combinedDetectorsParticlePostionsGraphMaxX = 0, combinedDetectorsParticlePostionsGraphMaxY = 0;
    
    for (int d=0; d<(int)detectors.size(); d++) {
        detectorsParticlePostions.at(d)->SetMarkerColor(detectors.at(d).color);
        detectorsParticlePostions.at(d)->SetMarkerStyle(2);
        detectorsParticlePostions.at(d)->SetMarkerSize(.5);
        char name[50];
        sprintf(name, "Detector %i Collision Positions", d+1);
        detectorsParticlePostions.at(d)->SetTitle(name);
        detectorCollisionPositionsCanvas->cd(cd);
        detectorsParticlePostions.at(d)->Draw("AP*");
        detectorsParticlePostions.at(d)->GetXaxis()->SetLimits(0.0 , detectors.at(d).width);
        detectorsParticlePostions.at(d)->GetYaxis()->SetRangeUser(0.0 , detectors.at(d).height );
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
                subdetectorHistograms.at(d)->SetBinContent(detectors.at(d).numberOfSubDetectorsAlongWidth - w, h+1, detectors.at(d).subDetectors.at((w*detectors.at(d).numberOfSubDetectorsAlongHeight)+h).numberOfParticlesEntered);
            }
        }
        detectorCollisionPositionsCanvas->cd(cd+(int)detectors.size());
        cd++;
        subdetectorHistograms.at(d)->Draw("COLZ");
    }
    
    detectorCollisionPositionsCanvas->Update();
    detectorCollisionPositionsCanvas->Modified();
    detectorCollisionPositionsCanvas->Write();
    
    TCanvas *combinedDetectorsParticlePostionsCanvas = new TCanvas("Combined Detectors Collision Positions","Combined Detectors Collision Positions",0,0,2000,1300);
    combinedDetectorsParticlePostionsGraph->SetTitle("Combined Detectors Collision Positions");
    combinedDetectorsParticlePostionsGraph->Draw("AP*");
    combinedDetectorsParticlePostionsGraph->GetXaxis()->SetRangeUser(0,combinedDetectorsParticlePostionsGraphMaxX);
    combinedDetectorsParticlePostionsGraph->GetYaxis()->SetRangeUser(0,combinedDetectorsParticlePostionsGraphMaxY);
    combinedDetectorsParticlePostionsGraph->GetXaxis()->SetLimits(0, combinedDetectorsParticlePostionsGraphMaxX);
    combinedDetectorsParticlePostionsCanvas->Update();
    combinedDetectorsParticlePostionsCanvas->Modified();
    combinedDetectorsParticlePostionsCanvas->Write();
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
    
    mutiplyer = Abs(mutiplyer);//Stops particles from being detecting backwords
    
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

void drawBoxOnCanvasWithDimensions(TCanvas *aCanvas, TVector3 bottomRightFrontPoint, double width, double depth, double height, double angle, short color)
{
    TVector3 aPoint;
    TPolyLine3D *aLine;
    
    aCanvas->cd();
    
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
}

void drawSubdetectorHitsWithTrajetories()
{
    TVector3 aPoint;
    TPolyLine3D *aLine;
    
    TCanvas *combinedSubdetectorWithTrajetoriesCanvas = new TCanvas("Particle Trajectories in Setup","Particle Trajectories in Setup",0,0,2000,1300);
    combinedSubdetectorWithTrajetoriesCanvas->cd();
    
    //Particles
    if(drawAllParticlesPaths||drawDetectedParticlesPaths){
        for(int i=0; i<(int)particles.size(); i++){
            bool particleHitADetector = false;
            for(int d=0; d<(int)detectors.size(); d++){
                if(particles.at(i).hitDetector.at(d))
                {   particleHitADetector = true; }
            }
            if (particleHitADetector||drawAllParticlesPaths) {
            aLine = new TPolyLine3D((int)particles.at(i).positions.size());
            for (int c=0; c<(int)particles.at(i).positions.size(); c++) {
                aLine->SetPoint(c, particles.at(i).positions.at(c).X(), particles.at(i).positions.at(c).Y(), particles.at(i).positions.at(c).Z());
            }
            aLine->SetLineWidth(1);
            aLine->SetLineColor(7);
            aLine->Draw("same");
            }
        }
    }
    
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
    
    //Detectors
    for (int i = (int)detectors.size()-1 ; i>=0; i--) {
        for (int w=0; w<detectors.at(i).numberOfSubDetectorsAlongWidth; w++) {
            for (int h=0; h<detectors.at(i).numberOfSubDetectorsAlongHeight; h++) {
                if (detectors.at(i).subDetectors.at((w*detectors.at(i).numberOfSubDetectorsAlongHeight)+h).numberOfParticlesEntered>0) {
                    drawBoxOnCanvasWithDimensions(combinedSubdetectorWithTrajetoriesCanvas, detectors.at(i).subDetectors.at((w*detectors.at(i).numberOfSubDetectorsAlongHeight)+h).lowestYZCorner, detectors.at(i).subDetectors.at((w*detectors.at(i).numberOfSubDetectorsAlongHeight)+h).width, detectors.at(i).subDetectors.at((w*detectors.at(i).numberOfSubDetectorsAlongHeight)+h).depth, detectors.at(i).subDetectors.at((w*detectors.at(i).numberOfSubDetectorsAlongHeight)+h).height, detectorAlighnmentAngle, detectors.at(i).color);
                }
            }
        }
        //Draw Main Detector Volume
        drawBlockOnCanvasWithDimensions(combinedSubdetectorWithTrajetoriesCanvas, detectors.at(i).lowestYZCorner, detectors.at(i).width, detectors.at(i).depth, detectors.at(i).height, detectorAlighnmentAngle, detectors.at(i).color);
    }
    
    combinedSubdetectorWithTrajetoriesCanvas->Update();
    combinedSubdetectorWithTrajetoriesCanvas->Modified();
    combinedSubdetectorWithTrajetoriesCanvas->Write();
}

void drawDetectorsSetup()
{
    TVector3 aPoint;
    TPolyLine3D *aLine;
    
    TCanvas *detectorSetupCanvas = new TCanvas("Detector Setup","Detector Setup",0,0,1000,700);
    detectorSetupCanvas->cd();
    
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
    if (displayDetectorAlignmentAngle) {
        aLine = new TPolyLine3D(2);
        aLine->SetPoint(0, 0, 0, 0);
        aLine->SetPoint(1, (detectorRoom.lowestYZCorner.Y()+detectorRoom.depth)*tan(detectorAlighnmentAngle), detectorRoom.lowestYZCorner.Y()+detectorRoom.depth, 0);
        aLine->SetLineWidth(1);
        aLine->SetLineColor(2);
        aLine->Draw("same");
    }
    
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
    if (displayDetectorRoom) {
        aPoint.SetX( detectorRoom.lowestYZCorner.X() + detectorRoom.width);
        aPoint.SetY( detectorRoom.lowestYZCorner.Y());
        aPoint.SetZ( detectorRoom.lowestYZCorner.Z());
        drawBlockOnCanvasWithDimensions(detectorSetupCanvas, aPoint, detectorRoom.width, detectorRoom.depth, detectorRoom.height, 0, 1);
    }
    
    detectorSetupCanvas->Update();
    detectorSetupCanvas->Modified();
    detectorSetupCanvas->Write();
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