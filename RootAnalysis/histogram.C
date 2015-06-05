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
 
	TFile *fMilliQ = new TFile("/home/qftsm/Work/GeantDevExamples/milliq/RootAnalysis/MilliQ.root");
	TTree *t1 = (TTree*)fMilliQ->Get("MilliQ");
	Double_t TotalEnergyDeposit;
	t1->SetBranchAddress("TotalEnergyDeposit",&TotalEnergyDeposit);

	// two histograms
	TH1F *htotalenergydepo   = new TH1F("htotalenergydepo","Total Energy Deposit",100,0,10);
	
	// all entries and fill the histograms
	Int_t nentries = (Int_t)t1->GetEntries();
	for (Int_t i=0;i<nentries;i++) {
		t1->GetEntry(i);
		if(TotalEnergyDeposit>0)
			htotalenergydepo->Fill(TotalEnergyDeposit);
	}
	TCanvas* mycanvas = new TCanvas();
	mycanvas->Divide(1,1);
	mycanvas->cd(1);
	htotalenergydepo->Draw(); 
	
	mycanvas->Print("TotalEnergyDeposit.pdf");
	
}

 #ifndef __CINT__
 int main(){
     histogram();
     }
 #endif
