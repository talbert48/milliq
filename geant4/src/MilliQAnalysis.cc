#include "globals.hh"
#include "MilliQAnalysis.hh"
#include "G4SystemOfUnits.hh"
#include <math.h>

MilliQAnalysis::MilliQAnalysis(std::vector< std::vector<G4double> > ppmtTime, G4int pNblock, G4int pNstack):
	fpmtTime(ppmtTime),fNblock(pNblock), fNstack(pNstack),fIsActive(false){
}


void MilliQAnalysis::NearestN(){

	//Figures out for which sequence of PMTs all 3 light up

	std::vector<G4int> activePMT;

	for(G4int i=0; i < fNblock; i++){
		if(fpmtTime[i].size()>0){
			activePMT.push_back(i);
		}
	}

	for(G4int i=0; i < activePMT.size(); i++){
		bool recordEvent = true;
		for(G4int j=1; j < fNstack+1; j++){
			if(fpmtTime[activePMT[i]*fNstack].size()==0)
				recordEvent = false;
		}
		if(recordEvent = true){
			activeEvent.push_back( activePMT[i] );
		}

	}
	if(activeEvent.size()==1){
		fIsActive = true;
		ComputeTimes();
	}

}

void MilliQAnalysis::ComputeTimes(){

	G4int pmt;
	G4int nmedian;

	for(G4int i=0; i < activeEvent.size(); i++){
		pmt = activeEvent[i];
		for(G4int j = 1; j < fNstack+1; j++){
			nmedian = (fpmtTime[j*pmt].size()-1)/2;
			times.push_back(fpmtTime[j*pmt][nmedian]);
		}

	}

}


