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
 
	TFile *fgab = new TFile("/home/qftsm/Work/GeantDevExamples/basic/BuildExamples/B2Build/B2.root");
	TTree *t1gab = (TTree*)fgab->Get("B2");
	Double_t EnergyDeposit;
	t1gab->SetBranchAddress("EnergyDeposit",&EnergyDeposit);

	// two histograms
	TH1F *h1gab   = new TH1F("hpxgab","Energy Deposit",100,0,40);
	
	// all entries and fill the histograms
	Int_t nentries = (Int_t)t1gab->GetEntries();
	for (Int_t i=0;i<nentries;i++) {
		t1gab->GetEntry(i);
		if(EnergyDeposit>0)
			h1gab->Fill(EnergyDeposit);
	}
	TCanvas* mycanvas = new TCanvas();
	mycanvas->Divide(1,1);
	mycanvas->cd(1);
	h1gab->Draw(); 
	
	mycanvas->Print("EnergyDeposit.pdf");
	
}

 #ifndef __CINT__
 int main(){
     histogram();
     }
 #endif
