
#include "MilliQPMTHit.hh"
#include "G4ios.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

G4ThreadLocal G4Allocator<MilliQPMTHit>* MilliQPMTHitAllocator=0;


MilliQPMTHit::MilliQPMTHit()
  : fPmtNumber(-1),fPhotons(0),fPhysVol(0),fDrawit(false) {}


MilliQPMTHit::~MilliQPMTHit() {}


MilliQPMTHit::MilliQPMTHit(const MilliQPMTHit &right) : G4VHit()
{
  fPmtNumber=right.fPmtNumber;
  fPhotons=right.fPhotons;
  fPhysVol=right.fPhysVol;
  fDrawit=right.fDrawit;
}


const MilliQPMTHit& MilliQPMTHit::operator=(const MilliQPMTHit &right){
  fPmtNumber = right.fPmtNumber;
  fPhotons=right.fPhotons;
  fPhysVol=right.fPhysVol;
  fDrawit=right.fDrawit;
  return *this;
}


G4int MilliQPMTHit::operator==(const MilliQPMTHit &right) const{
  return (fPmtNumber==right.fPmtNumber);
}


void MilliQPMTHit::Draw(){
  if(fDrawit&&fPhysVol){ //ReDraw only the PMTs that have hit counts > 0
    //Also need a physical volume to be able to draw anything
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if(pVVisManager){//Make sure that the VisManager exists
      G4VisAttributes attribs(G4Colour(1.,0.,0.));
      attribs.SetForceSolid(true);
      G4RotationMatrix rot;
      if(fPhysVol->GetRotation())//If a rotation is defined use it
        rot=*(fPhysVol->GetRotation());
      G4Transform3D trans(rot,fPhysVol->GetTranslation());//Create transform
      pVVisManager->Draw(*fPhysVol,attribs,trans);//Draw it
    }
  }
}


void MilliQPMTHit::Print() {}
