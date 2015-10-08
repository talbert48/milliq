#include "MilliQRunAction.hh"
#include "MilliQRecorderBase.hh"
#include "MilliQDetectorConstruction.hh"
#include "MilliQPrimaryGeneratorAction.hh"
#include "MilliQRunActionMessenger.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleDefinition.hh"
#include "g4root.hh"
#include "G4ios.hh"



#include "G4EmCalculator.hh"

MilliQRunAction::MilliQRunAction(MilliQRecorderBase* r) :
	fRecorder(r), fMilliQRunActionMessenger(0) {
	fDetector = new MilliQDetectorConstruction();
	fKinematic = new MilliQPrimaryGeneratorAction();
	// Create analysis manager
	// The choice of analysis technology is done via selection of a namespace
	// in B5Analysis.hh
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	G4cout << "Using " << analysisManager->GetType() << G4endl;

	fVerboseLevel = 0;
	fMilliQRunActionMessenger = new MilliQRunActionMessenger(this);

	// Default settings
	  analysisManager->SetVerboseLevel(1);
	analysisManager->SetFileName("MilliQ");

	// Book histograms, ntuple
	//

	// Creating 1D histograms
	analysisManager->CreateH1("PMT", "PMT # Hits", 50, 0., 50); // h1 Id = 0

	// Creating 2D histograms
	//  analysisManager
	//    ->CreateH2("Chamber1 XY","Drift Chamber 1 X vs Y",           // h2 Id = 0
	//               50, -1000., 1000, 50, -300., 300.);

	// Creating ntuple
	//
	analysisManager->SetFirstNtupleId(1);

	analysisManager->CreateNtuple("MilliQ", "Hits");
	analysisManager->CreateNtupleDColumn("TotalEnergyDeposit"); // column Id = 0
	analysisManager->FinishNtuple();

	analysisManager->CreateNtuple("MilliQTime", "Time");
	analysisManager->CreateNtupleDColumn("TimeOfFlightScint");  // column Id = 0
	analysisManager->FinishNtuple();

	analysisManager->CreateNtuple("MilliQDedx", "DEDX");
	analysisManager->CreateNtupleDColumn("EKinMeV"); // column Id = 0
	analysisManager->CreateNtupleDColumn("MeVpermm"); // column Id = 1
	analysisManager->FinishNtuple();

	analysisManager->CreateNtuple("MilliQRadius","Radius");
	analysisManager->CreateNtupleDColumn("CoordinateX");//column Id = 0
	analysisManager->CreateNtupleDColumn("CoordinateY");//column Id = 1
	analysisManager->CreateNtupleDColumn("CoordinateZ");//column Id = 2
	analysisManager->FinishNtuple();

}

MilliQRunAction::~MilliQRunAction() {
	delete G4AnalysisManager::Instance();
	delete fMilliQRunActionMessenger;

}

void MilliQRunAction::BeginOfRunAction(const G4Run* aRun) {
	if (fRecorder)
		fRecorder->RecordBeginOfRun(aRun);

	//inform the runManager to save random number seed
	//G4RunManager::GetRunManager()->SetRandomNumberStore(true);

	// Get analysis manager
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	// Open an output file
	// The default file name is set in B5RunAction::B5RunAction(),
	// it can be overwritten in a macro
	analysisManager->OpenFile();

}

void MilliQRunAction::EndOfRunAction(const G4Run* aRun) {
	if (fRecorder)
		fRecorder->RecordEndOfRun(aRun);

	// save histograms & ntuple
	//
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

////////////////////////////

	G4int nEvents = aRun->GetNumberOfEvent();
	if (nEvents == 0) {
		return;
	}

	if (fDetector->GetAlternateGeometry() == 1) {
		//run conditions
		//
		fDetector->DefineMaterials();
		const G4Material* material = fDetector->GetScintMaterial();
		G4double density = material->GetDensity();
		G4String matName = material->GetName();

		const G4ParticleDefinition* part =
					fKinematic->GetParticleGun()->GetParticleDefinition();
//		G4String particle = part->GetParticleName();


		G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
			  G4String particleName;
		G4ParticleDefinition* particle
		    = particleTable->FindParticle(particleName="monopole");



		G4double ekin[100], dedx[100];
		G4EmCalculator calc;
		//  calc.SetVerbose(0);

		//
		G4int i;
		for (i = 0; i < 100; ++i) {
			ekin[i] = 0.2*std::pow(10., 0.07 * G4double(i))*std::pow(10,G4double(3))*MeV;
			dedx[i] = calc.ComputeElectronicDEDX(ekin[i], "monopole",
					matName);
		}
G4cout<<ekin[0]<<G4endl;
		for (i = 0; i < 100; i++) {
			G4cout << " E(MeV)= " << ekin[i] << " dedx= " << dedx[i] << G4endl;
		}

		G4cout << "### End of dEdX table" << G4endl;

		for (i = 0; i < 100; ++i) {
			analysisManager->FillNtupleDColumn(3, 0, ekin[i]);
			analysisManager->FillNtupleDColumn(3, 1, dedx[i]);
			analysisManager->AddNtupleRow(3);
		}
	}
	CLHEP::HepRandom::showEngineStatus();


/////////////////////////
	analysisManager->Write();
	analysisManager->CloseFile();

}
