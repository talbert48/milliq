
#include "MilliQPMTSD.hh"
#include "MilliQPMTHit.hh"
#include "MilliQDetectorConstruction.hh"
#include "MilliQUserTrackInformation.hh"

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"






MilliQPMTSD::MilliQPMTSD(G4String name)
  : G4VSensitiveDetector(name),fPMTHitCollection(0)
{
    collectionName.insert("pmtHitCollection");
    G4cout<<"HELLO "<<G4endl;

}


MilliQPMTSD::~MilliQPMTSD(){}


void MilliQPMTSD::Initialize(G4HCofThisEvent* hitsCE)
{
	G4cout<<"HELLO "<<G4endl;
    fPMTHitCollection = new MilliQPMTHitsCollection(SensitiveDetectorName,
                                                    collectionName[0]);
    //Store collection with event and keep ID
    static G4int hitCID = -1;
    if(hitCID<0){
     // hitCID = GetCollectionID(0);
    	hitCID = G4SDManager::GetSDMpointer()->GetCollectionID(fPMTHitCollection);
    }
    hitsCE->AddHitsCollection( hitCID, fPMTHitCollection );


    // fill calorimeter hits with zero energy deposition
    for (G4int iColumn=0;iColumn<4;iColumn++)
    	for (G4int iRow=0;iRow<1;iRow++)
        	{
            	MilliQPMTHit* hit = new MilliQPMTHit();
            	fPMTHitCollection->insert(hit);
        	}
	}


G4bool MilliQPMTSD::ProcessHits(G4Step* step,G4TouchableHistory* ){
 /*   G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();
    // energy deposit in this step
    G4double edep = step->GetTotalEnergyDeposit();

    if (edep <= 0.) return false;
   */

	   G4double edep = step->GetTotalEnergyDeposit();
	    if (edep==0.) return true;

	    G4TouchableHistory* touchable
	      = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());

	    G4VPhysicalVolume* cellPhysical = touchable->GetVolume(3); //Block Replica (row)
	    G4int rowNo = cellPhysical->GetCopyNo();

	 /*   G4cout<<"Volume() "<<touchable->GetVolume()->GetName() <<G4endl;
	    G4cout<<"Volume 0 "<<touchable->GetVolume(0)->GetName() <<G4endl;
	    G4cout<<"Volume 1 "<<touchable->GetVolume(1)->GetName() <<G4endl;
	    G4cout<<"Volume 2 "<<touchable->GetVolume(2)->GetName() <<G4endl;
	    G4cout<<"Volume 3 "<<touchable->GetVolume(3)->GetName() <<G4endl;
	    G4cout<<"Volume 4 "<<touchable->GetVolume(4)->GetName() <<G4endl;
	    G4cout<<"Volume 5 "<<touchable->GetVolume(5)->GetName() <<G4endl;
*/


	    G4VPhysicalVolume* columnPhysical = touchable->GetVolume(4); //Stack Replica (Column)
	    G4int columnNo = columnPhysical->GetCopyNo();

	    G4int hitID = 2*columnNo+rowNo;
	    MilliQPMTHit* hit = (*fPMTHitCollection)[hitID];

	    G4cout<<"COPY NUMBER "<<hitID<<G4endl;

	    // check if it is first touch
	    if (hit->GetColumnID()<0)
	    {
	        hit->SetColumnID(columnNo);
	        hit->SetRowID(rowNo);
	        G4int depth = touchable->GetHistory()->GetDepth();
	        G4AffineTransform transform
	          = touchable->GetHistory()->GetTransform(depth-2);
	        transform.Invert();
	        hit->SetRot(transform.NetRotation());
	        hit->SetPos(transform.NetTranslation());
	    }
	    // add energy deposition
	    hit->AddEdep(edep);
	    G4cout<<"ENERGY DEPOSIT" <<edep<<G4endl;

	    return true;


}


//Generates a hit and uses the postStepPoint's mother volume replica number
//PostStepPoint because the hit is generated manually when the photon is
//absorbed by the photocathode

G4bool MilliQPMTSD::ProcessHits_constStep(const G4Step* aStep,
                                          G4TouchableHistory*)
{
	G4cout<<"It got to ProcessHits_constStep, please go back and rewrite code!!!***"<<G4endl;

    //need to know if this is an optical photon
    if(aStep->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()){
        return false;
    }

    //User replica number 1 since photocathode is a daughter volume
    //to the pmt which was replicated
    G4int pmtNumber = aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(1);//1****
    G4VPhysicalVolume* physVol=
    aStep->GetPostStepPoint()->GetTouchable()->GetVolume(1);

    //Find the correct hit collection
    G4int n=fPMTHitCollection->entries();
    MilliQPMTHit* hit=NULL;
    for(G4int i=0;i<n;i++){
        if((*fPMTHitCollection)[i]->GetPMTNumber()==pmtNumber){
            hit=(*fPMTHitCollection)[i];
            break;
        }
    }

    if(hit==NULL){//this pmt wasnt previously hit in this event
        hit = new MilliQPMTHit(); //so create new hit
        hit->SetPMTNumber(pmtNumber);
        hit->SetPMTPhysVol(physVol);
        fPMTHitCollection->insert(hit);
    }

    hit->IncPhotonCount(); //increment hit for the selected pmt

    hit->SetDrawit(true);

    return true;
}


void MilliQPMTSD::EndOfEvent(G4HCofThisEvent* ) {}


void MilliQPMTSD::clear() {}


void MilliQPMTSD::DrawAll() {}


void MilliQPMTSD::PrintAll() {}
