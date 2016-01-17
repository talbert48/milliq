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
 
	TFile *fMilliQ = new TFile("MilliQ.root");
	TTree *t1 = (TTree*)fMilliQ->Get("MilliQEn");
	TTree *t2 = (TTree*)fMilliQ->Get("MilliQAll");
	TTree *t5 = (TTree*)fMilliQ->Get("MilliQPMT0All");
        TTree *t6 = (TTree*)fMilliQ->Get("MilliQPMT1All");
        TTree *t7 = (TTree*)fMilliQ->Get("MilliQPMT2All");

	Double_t sEnDep0, sTime0;
	Double_t pmtHitTime0, pmtHitTime1, pmtHitTime2, TOFScint0, TOFScint1, TOFScint2, TotEnDep0, TotEnDep1, TotEnDep2;
	Double_t pmtAllTimes0, pmtAllTimes1, pmtAllTimes2;
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

	t5->SetBranchAddress("pmtAllTimes0",&pmtAllTimes0);
        t6->SetBranchAddress("pmtAllTimes1",&pmtAllTimes1);
        t7->SetBranchAddress("pmtAllTimes2",&pmtAllTimes2);

	Int_t nentries_t1 = (Int_t)t1->GetEntries();
	Int_t nentries_t2 = (Int_t)t2->GetEntries();
	Int_t nentries_t5 = (Int_t)t5->GetEntries();
        Int_t nentries_t6 = (Int_t)t6->GetEntries();
        Int_t nentries_t7 = (Int_t)t7->GetEntries();

	ofstream mcpall, sedep, pmt0time, pmt1time, pmt2time;
  	sedep.open ("sedep.dat");
	mcpall.open ("mcpall.dat");
	pmt0time.open ("pmt0time.dat");
	pmt1time.open ("pmt1time.dat");
	pmt2time.open ("pmt2time.dat");
	

  	if (sedep == NULL && mcpall == NULL)
		printf("Sorry, but the ascii output file did not open.");
	for (Int_t i=0; i<nentries_t1;i++){
		t1->GetEvent(i);
		sedep<<sEnDep0<<" "<<sTime0<<endl;
	}
	
	for (Int_t i=0; i<nentries_t2;i++)
	{
		t2->GetEvent(i);
		mcpall<<activePMT0<<" "<<activePMT1<<" "<<activePMT2<<" "<<pmtHitTime0<<" "<<pmtHitTime1<<" "<<pmtHitTime2<<" "<<TOFScint0<<" "<<TOFScint1<<" "<<TOFScint2<<" "<<TotEnDep0<<" "<<TotEnDep1<<" "<<TotEnDep2<<" "<<NScintPho<<endl;
	}

	for (Int_t i=0; i<nentries_t5;i++){
                t5->GetEvent(i);
                pmt0time<<pmtAllTimes0<<endl;
        }

        for (Int_t i=0; i<nentries_t6;i++){
                t6->GetEvent(i);
                pmt1time<<pmtAllTimes1<<endl;
        }

        for (Int_t i=0; i<nentries_t7;i++){
                t7->GetEvent(i);
                pmt2time<<pmtAllTimes2<<endl;
        }



	sedep.close();
	mcpall.close();
	pmt0time.close();
	pmt1time.close();
	pmt2time.close();





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
