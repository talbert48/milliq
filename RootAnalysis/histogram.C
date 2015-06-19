// Builds a graph with errors, displays it and saves it
 // as image. First, include some header files (within,
 // CINT these will be ignored).

#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"

void histogram(){
 
	TFile *fMilliQ = new TFile("MilliQ.root");//"/home/qftsm/Work/GeantDevExamples/BuildExamples/MilliQBuild/MilliQ.root");
	TTree *t1 = (TTree*)fMilliQ->Get("MilliQ");
	TTree *t2 = (TTree*)fMilliQ->Get("MilliQTime");	

	Double_t TotalEnergyDeposit;
	Double_t TimeOfFlightScint;
	t1->SetBranchAddress("TotalEnergyDeposit",&TotalEnergyDeposit);
	t2->SetBranchAddress("TimeOfFlightScint", &TimeOfFlightScint);

	// two histograms
	TH1F *htotalenergydepo   = new TH1F("htotalenergydepo","Total Energy Deposit in Scintillators",100,0,0.6);
	TH1F *htimeofflightscint = new TH1F("htimeofflightscint","Time of Flight in Scintillator",100,1,5);

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


}

 #ifndef __CINT__
 int main(){
     histogram();
     }
 #endif
