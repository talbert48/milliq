#include "globals.hh"
#include "MilliQAnalysis.hh"
#include "G4SystemOfUnits.hh"
#include <math.h>
#include <algorithm>

MilliQAnalysis::MilliQAnalysis(std::vector< std::vector<G4double> > ppmtTime, std::vector< std::vector<G4double> > pscintTime,std::vector< std::vector<G4double> > pscintEn,
		G4int pNblock, G4int pNstack):
	fNblock(pNblock), fNstack(pNstack),fIsActive(false){

	for(int i = 0; i < ppmtTime.size(); i++){
		sort(ppmtTime[i].begin(),ppmtTime[i].end());
	}
	for(int i = 0; i < pscintTime.size(); i++){
		sort(pscintTime[i].begin(),pscintTime[i].end());
	}

	fpmtTime=ppmtTime;
	fscintTime=pscintTime;
	fscintEn=pscintEn;
	NearestN();

}


void MilliQAnalysis::NearestN(){

	//Figures out for which sequence all 3 pmt and Scint light up

	std::vector<G4int> activePMT;

	for(G4int i=0; i < fNblock; i++){
		if(fpmtTime[i].size()>0){
			activePMT.push_back(i);
		}
	}

	for(G4int i=0; i < activePMT.size(); i++){
		bool recordEvent = true;
		for(G4int j=0; j < fNstack; j++){
			if(fpmtTime[activePMT[i]+fNblock*j].size()==0)
				recordEvent = false;
			if(fscintTime[activePMT[i]+fNblock*j].size()==0)
				recordEvent = false;
		}
		if(recordEvent == true){
			activeEvent.push_back( activePMT[i] );

		}

	}

	//If passes this, means that the same 3 scint and pmt light up!
	if(activeEvent.size()==1){
		for(G4int j = 1; j < fNstack; j++)
			activeEvent.push_back(activeEvent[0]+j*fNblock);
		fIsActive = true;
		ComputeTandE();
	}

}

void MilliQAnalysis::ComputeTandE(){
	//Computes the median time for photons to hit PMT

	G4int ind;
	G4int nmedian;

	for(G4int i=0; i < activeEvent.size(); i++){
			ind = activeEvent[i];
			G4double ensum=0;
			nmedian = (fpmtTime[ind].size()-1)/2;
			pmtTimes.push_back(fpmtTime[ind][nmedian]);

			timeOfFlight.push_back(fpmtTime[ind][nmedian]-fscintTime[ind][0]);

			for(G4int k = 0; k < fscintEn[ind].size();k++){
				ensum+=fscintEn[ind][k];
			}
			totalEdep.push_back(ensum);

	}

}
