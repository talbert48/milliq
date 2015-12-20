//This script is the main analysis script for the detector


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

void histogram(){
 
	TFile *fMilliQ = new TFile("MilliQ.root");//("../../BuildExamples/MilliQBuild/MilliQ.root");
	TTree *t1 = (TTree*)fMilliQ->Get("MilliQEn");
	TTree *t2 = (TTree*)fMilliQ->Get("MilliQAll");

	Double_t sEnDep0, sTime0;
	Double_t pmtHitTime0, pmtHitTime1, pmtHitTime2, TOFScint0, TOFScint1, TOFScint2, TotEnDep0, TotEnDep1, TotEnDep2;
	Int_t activePMT0, activePMT1, activePMT2, NScintPho;
	t1->SetBranchAddress("sEnDep0",&sEnDep0);
	t1->SetBranchAddress("sTime0",&sTime0);


	t2->SetBranchAddress("activePMT0",&activePMT0);
        t2->SetBranchAddress("activePMT1",&activePMT1);
        t2->SetBranchAddress("activePMT2",&activePMT2);
        t2->SetBranchAddress("pmtHitTime0",&pmtHitTime0);
        t2->SetBranchAddress("pmtHitTime1",&pmtHitTime1);
        t2->SetBranchAddress("pmtHitTime2",&pmtHitTime2);
        t2->SetBranchAddress("TOFScint0",&TOFScint0);
        t2->SetBranchAddress("TOFScint1",&TOFScint1);
        t2->SetBranchAddress("TOFScint2",&TOFScint2);
        t2->SetBranchAddress("TotEnDep0",&TotEnDep0);
        t2->SetBranchAddress("TotEnDep1",&TotEnDep1);
        t2->SetBranchAddress("TotEnDep2",&TotEnDep2);
	t2->SetBranchAddress("NScintPho",&NScintPho);

	Int_t nentries_t1 = (Int_t)t1->GetEntries();
	Int_t nentries_t2 = (Int_t)t2->GetEntries();

	ofstream mcpall;
	ofstream sedep;
//  	sedep.open ("sedep.dat");
	mcpall.open ("mcpall.dat");

  	if (sedep == NULL && mcpall == NULL)
		printf("Sorry, but the ascii output file did not open.");
//	for (Int_t i=0; i<nentries_t1;i++){
//		t1->GetEvent(i);
//		sedep<<sEnDep0<<" "<<sTime0<<endl;
//	}
	
	for (Int_t i=0; i<nentries_t2;i++)
	{
		t2->GetEvent(i);
		mcpall<<activePMT0<<" "<<activePMT1<<" "<<activePMT2<<" "<<pmtHitTime0<<" "<<pmtHitTime1<<" "<<pmtHitTime2<<" "<<TOFScint0<<" "<<TOFScint1<<" "<<TOFScint2<<" "<<TotEnDep0<<" "<<TotEnDep1<<" "<<TotEnDep2<<" "<<NScintPho<<endl;
	}

//	sedep.close();
	mcpall.close();






/*
	//Making Histograms in Root Directly
	TH1F *htotalenergydepo   = new TH1F("htotalenergydepo","Total Energy Deposit in Scintillators",100,0,1.5);

	// all entries and fill the histograms
	Int_t nentries_t1 = (Int_t)t1->GetEntries();
	for (Int_t i=0;i<nentries_t1;i++) {
		t1->GetEntry(i);
		if(TotalEnergyDeposit>0)
			htotalenergydepo->Fill(TotalEnergyDeposit);
	}
	TCanvas* mycanvas = new TCanvas();
	mycanvas->Divide(1,1);
	mycanvas->cd(1);
	htotalenergydepo->Draw(); 
	mycanvas->Print("TotalEnergyDeposit.pdf");
*/

}

 #ifndef __CINT__
 int main(){
     histogram();
return 0;
     }
 #endif
