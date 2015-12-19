// This script is to be used if the Set Alternative Geometry = 1 is turned on

#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;

void histogramCalibrate(){
 
	TFile *fMilliQ = new TFile("../../BuildExamples/MilliQBuild/MilliQ.root");
	TTree *t3 = (TTree*)fMilliQ->Get("MilliQDedx");
	TTree *t4 = (TTree*)fMilliQ->Get("MilliQRadius");

	Double_t EKinMeV, MeVpermm;
	Double_t CoordinateX, CoordinateY, CoordinateZ;
	t3->SetBranchAddress("EKinMeV",&EKinMeV);
	t3->SetBranchAddress("MeVpermm",&MeVpermm);

	t4->SetBranchAddress("CoordinateX",&CoordinateX);
        t4->SetBranchAddress("CoordinateY",&CoordinateY);
        t4->SetBranchAddress("CoordinateZ",&CoordinateZ);

	Int_t nentries_t3 = (Int_t)t3->GetEntries();
	Int_t nentries_t4 = (Int_t)t4->GetEntries();

	ofstream dedxfile;
	ofstream myfile;
  	myfile.open ("radiusofcurvature.dat");
	dedxfile.open ("dedx.dat");

  	if (myfile == NULL && dedxfile == NULL)
		printf("Sorry, but the ascii output file did not open.");
	for (Int_t i=0; i<nentries_t3;i++){
		t3->GetEvent(i);
		dedxfile<<EKinMeV<<" "<<MeVpermm<<endl;
	}
	
	for (Int_t i=0; i<nentries_t4;i++)
	{
		t4->GetEvent(i);
		myfile<<CoordinateX<<" "<<CoordinateY<<" "<<CoordinateZ<<endl;
	}
	myfile.close();
	dedxfile.close();

	// two histograms
//	TH1F *htotalenergydepo   = new TH1F("htotalenergydepo","Total Energy Deposit in Scintillators",100,0,1.5);
//	TH1F *htimeofflightscint = new TH1F("htimeofflightscint","Time of Flight in Scintillator",100,0,10);
//	htotalenergydepo -> GetYaxis() -> SetTitle("N");
/*
	// all entries and fill the histograms
	Int_t nentries_t1 = (Int_t)t1->GetEntries();
	for (Int_t i=0;i<nentries_t1;i++) {
		t1->GetEntry(i);
		if(TotalEnergyDeposit>0)
			htotalenergydepo->Fill(TotalEnergyDeposit);
	}

	Int_t nentries_t2 = (Int_t)t2->GetEntries();
	for (Int_t i=0; i < nentries_t2; i++){
		t2->GetEntry(i);
		if(TimeOfFlightScint>0)
			htimeofflightscint->Fill(TimeOfFlightScint);
	}


	TCanvas* mycanvas = new TCanvas();
	mycanvas->Divide(1,1);
	mycanvas->cd(1);
	htotalenergydepo->Draw(); 
	
	mycanvas->Print("TotalEnergyDeposit.pdf");
	
	TCanvas* mycanvas_t2 = new TCanvas();
	mycanvas_t2->Divide(1,1);
	mycanvas_t2->cd(1);
	htimeofflightscint->Draw();
	mycanvas_t2->Print("TimeOfFlightScint.pdf");
*/

}

 #ifndef __CINT__
 int main(){
     histogramCalibrate();
return 0;
     }
 #endif
