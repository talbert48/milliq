
#ifndef MilliQAnalysis_H
#define MilliQAnalysis_H 1



class MilliQAnalysis{
  public:
    MilliQAnalysis(std::vector< std::vector<G4double> > pmtTime,
    		G4int pNblock,
			G4int pNstack);

    void NearestN();
    void ComputeTimes();
    bool IsActive(){return fIsActive;}
    std::vector<G4double> GetTimes(){return times;}
    std::vector<G4int> GetActivePMT(){return activeEvent;}


  private:

    std::vector< std::vector<G4double> > fpmtTime;
    std::vector<G4int> activeEvent;
	std::vector<G4double> times;
    G4int fNblock;
    G4int fNstack;
    bool fIsActive;
};

#endif
