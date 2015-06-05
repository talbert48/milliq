
#include "MilliQRunAction.hh"
#include "MilliQRecorderBase.hh"

#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "g4root.hh"

MilliQRunAction::MilliQRunAction(MilliQRecorderBase* r) : fRecorder(r)
{
	  // Create analysis manager
	  // The choice of analysis technology is done via selectin of a namespace
	  // in B5Analysis.hh
	  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	  G4cout << "Using " << analysisManager->GetType() << G4endl;

	  // Default settings
	  analysisManager->SetVerboseLevel(1);
	  analysisManager->SetFileName("MilliQ");

	  // Book histograms, ntuple
	  //

	  // Creating 1D histograms
	  analysisManager
	    ->CreateH1("PMT","PMT # Hits", 50, 0., 50); // h1 Id = 0

	  // Creating 2D histograms
	//  analysisManager
	//    ->CreateH2("Chamber1 XY","Drift Chamber 1 X vs Y",           // h2 Id = 0
	//               50, -1000., 1000, 50, -300., 300.);

	  // Creating ntuple
	  //
	  analysisManager->CreateNtuple("MilliQ", "Hits");
	  analysisManager->CreateNtupleIColumn("PMTHits");  // column Id = 0
	  analysisManager->CreateNtupleDColumn("TotalEnergyDeposit"); // column Id = 1
	  analysisManager->CreateNtupleDColumn("Time1");    // column Id = 2
	  analysisManager->FinishNtuple();
}


MilliQRunAction::~MilliQRunAction() {
	 delete G4AnalysisManager::Instance();

}


void MilliQRunAction::BeginOfRunAction(const G4Run* aRun){
  if(fRecorder)fRecorder->RecordBeginOfRun(aRun);

  //inform the runManager to save random number seed
   //G4RunManager::GetRunManager()->SetRandomNumberStore(true);

   // Get analysis manager
   G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

   // Open an output file
   // The default file name is set in B5RunAction::B5RunAction(),
   // it can be overwritten in a macro
   analysisManager->OpenFile();

}


void MilliQRunAction::EndOfRunAction(const G4Run* aRun){
  if(fRecorder)fRecorder->RecordEndOfRun(aRun);

  // save histograms & ntuple
  //
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();

}
