
#include "MilliQDetectorConstruction.hh"
#include "MilliQPMTSD.hh"
#include "MilliQScintSD.hh"
#include "MilliQDetectorMessenger.hh"
#include "MilliQDetectorBlockLV.hh"
#include "MilliQDetectorStack.hh"

#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include "G4OpticalSurface.hh"
#include "G4MaterialTable.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4UImanager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"


MilliQDetectorConstruction::MilliQDetectorConstruction()
: fScintillator_mt(NULL)
{
  fExperimentalHall_box = NULL;
  fExperimentalHall_log = NULL;
  fExperimentalHall_phys = NULL;

  fScintillator = fAl = fAir = fVacuum = fGlass = NULL;

  fN = fO = fC = fH = NULL;

  SetDefaults();

  fDetectorMessenger = new MilliQDetectorMessenger(this);
}


void MilliQDetectorConstruction::DefineMaterials(){
    G4double a;  // atomic mass
    G4double z;  // atomic number
    G4double density;
    
    G4NistManager* nist = G4NistManager::Instance();
    nist->SetVerbose(0);

    //***Elements
    fH = new G4Element("H", "H", z=1., a=1.01*g/mole);
    fC = new G4Element("C", "C", z=6., a=12.01*g/mole);
    fN = new G4Element("N", "N", z=7., a= 14.01*g/mole);
    fO = new G4Element("O", "O", z=8., a= 16.00*g/mole);

    //
    // Materials
    //
    
    //Scintillator
    fScintillator = new G4Material("Scintillator Material",density=3.020*g/cm3,2);
    fScintillator->AddElement(fC,10);
    fScintillator->AddElement(fH,11);
    //Aluminum
    fAl = new G4Material("Al",z=13.,a=26.98*g/mole,density=2.7*g/cm3);
    //Vacuum
    fVacuum = new G4Material("Vacuum",z=1.,a=1.01*g/mole,
                          density=universe_mean_density,kStateGas,0.1*kelvin,
                          1.e-19*pascal);
    //Air
    fAir = new G4Material("Air", density= 1.29*mg/cm3, 2);
    fAir->AddElement(fN, 70*perCent);
    fAir->AddElement(fO, 30*perCent);
    //Glass
    fGlass = new G4Material("Glass", density=1.032*g/cm3,2);
    fGlass->AddElement(fC,91.533*perCent);
    fGlass->AddElement(fH,8.467*perCent);
    //Concrete
    fConcrete = nist->FindOrBuildMaterial("G4_CONCRETE");

    //***Material properties tables

    G4double MilliQ_Energy[]    = { 7.0*eV , 7.07*eV, 7.14*eV };
    const G4int MilliQnum = sizeof(MilliQ_Energy)/sizeof(G4double);

    G4double MilliQ_SCINT[] = { 0.1, 1.0, 0.1 };
    assert(sizeof(MilliQ_SCINT) == sizeof(MilliQ_Energy));
    G4double MilliQ_RIND[]  = { 1.59 , 1.57, 1.54 };
    assert(sizeof(MilliQ_RIND) == sizeof(MilliQ_Energy));
    G4double MilliQ_ABSL[]  = { 35.*cm, 35.*cm, 35.*cm};
    assert(sizeof(MilliQ_ABSL) == sizeof(MilliQ_Energy));
    fScintillator_mt = new G4MaterialPropertiesTable();
    fScintillator_mt->AddProperty("FASTCOMPONENT", MilliQ_Energy, MilliQ_SCINT, MilliQnum);
    fScintillator_mt->AddProperty("SLOWCOMPONENT", MilliQ_Energy, MilliQ_SCINT, MilliQnum);
    fScintillator_mt->AddProperty("RINDEX",        MilliQ_Energy, MilliQ_RIND,  MilliQnum);
    fScintillator_mt->AddProperty("ABSLENGTH",     MilliQ_Energy, MilliQ_ABSL,  MilliQnum);
    fScintillator_mt->AddConstProperty("SCINTILLATIONYIELD",12./MeV);
    fScintillator_mt->AddConstProperty("RESOLUTIONSCALE",1.0);
    fScintillator_mt->AddConstProperty("FASTTIMECONSTANT",20.*ns);
    fScintillator_mt->AddConstProperty("SLOWTIMECONSTANT",45.*ns);
    fScintillator_mt->AddConstProperty("YIELDRATIO",1.0);
    fScintillator->SetMaterialPropertiesTable(fScintillator_mt);

    // Set the Birks Constant for the MilliQ scintillator

    fScintillator->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

    G4double glass_RIND[]={1.49,1.49,1.49};
    assert(sizeof(glass_RIND) == sizeof(MilliQ_Energy));
    G4double glass_AbsLength[]={420.*cm,420.*cm,420.*cm};
    assert(sizeof(glass_AbsLength) == sizeof(MilliQ_Energy));
    G4MaterialPropertiesTable *glass_mt = new G4MaterialPropertiesTable();
    glass_mt->AddProperty("ABSLENGTH",MilliQ_Energy,glass_AbsLength,MilliQnum);
    glass_mt->AddProperty("RINDEX",MilliQ_Energy,glass_RIND,MilliQnum);
    fGlass->SetMaterialPropertiesTable(glass_mt);

    G4double vacuum_Energy[]={2.0*eV,7.0*eV,7.14*eV};
    const G4int vacnum = sizeof(vacuum_Energy)/sizeof(G4double);
    G4double vacuum_RIND[]={1.,1.,1.};
    assert(sizeof(vacuum_RIND) == sizeof(vacuum_Energy));
    G4MaterialPropertiesTable *vacuum_mt = new G4MaterialPropertiesTable();
    vacuum_mt->AddProperty("RINDEX", vacuum_Energy, vacuum_RIND,vacnum);
    fVacuum->SetMaterialPropertiesTable(vacuum_mt);
    fAir->SetMaterialPropertiesTable(vacuum_mt);//Give air the same rindex
}


G4VPhysicalVolume* MilliQDetectorConstruction::Construct(){

  if (fExperimentalHall_phys) {
     G4GeometryManager::GetInstance()->OpenGeometry();
     G4PhysicalVolumeStore::GetInstance()->Clean();
     G4LogicalVolumeStore::GetInstance()->Clean();
     G4SolidStore::GetInstance()->Clean();
     G4LogicalSkinSurface::CleanSurfaceTable();
     G4LogicalBorderSurface::CleanSurfaceTable();
  }

  DefineMaterials();
  return ConstructDetector();
}


G4VPhysicalVolume* MilliQDetectorConstruction::ConstructDetector()
{
    //The experimental hall walls are all 1m away from housing walls
    G4double expHall_x = 10.*m;
    G4double expHall_y = 10.*m;
    G4double expHall_z = 20.*m;

    //Create experimental hall
    fExperimentalHall_box
    = new G4Box("expHall_box",expHall_x,expHall_y,expHall_z);
    fExperimentalHall_log = new G4LogicalVolume(fExperimentalHall_box,
                                              G4Material::GetMaterial("Air"),
                                              "expHall_log",
                                              0,0,0);
    fExperimentalHall_phys = new G4PVPlacement(0,G4ThreeVector(),
                              fExperimentalHall_log,"expHall",0,false,0);

    fExperimentalHall_log->SetVisAttributes(G4VisAttributes::Invisible);

    //Place the main volume
    
    fDetectorStack = new MilliQDetectorStack(0,
                                             G4ThreeVector(0.,0.,10.*m),
                                             fExperimentalHall_log,
                                             false,
                                             0,
                                             G4ThreeVector(fScint_x,fScint_y,fScint_z),
                                             fD_mtl,
                                             fOuterRadius_pmt,
                                             fRefl,
                                             fPmt_SD.Get());
    
    G4Box* wall = new G4Box("Wall V",5.*m,5.*m,.5*m);
    G4LogicalVolume* wallLV = new G4LogicalVolume(wall,
                                                fConcrete,
                                                "Wall LV",
                                                0,0,0);
    wallLV->SetVisAttributes(G4Colour(.5,.5,.5,.5));
    /*new G4PVPlacement(0,
                      G4ThreeVector(0,0,3.*m),
                      wallLV,
                      "Wall PV",
                      fExperimentalHall_log,
                      0,
                      0,
                      0);*/
    
  return fExperimentalHall_phys;
}


void MilliQDetectorConstruction::ConstructSDandField() {

    if (!fDetectorStack) return;

    // PMT SD

    if (!fPmt_SD.Get()) {
    //Created here so it exists as pmts are being placed
    G4cout << "Construction /MilliQDet/pmtSD" << G4endl;
    MilliQPMTSD* pmt_SD = new MilliQPMTSD("/MilliQDet/pmtSD");
    fPmt_SD.Put(pmt_SD);
    }
    

    for(int i=0; i<fDetectorStack->GetLogicalVolume()->GetNoDaughters(); i++){
        //MilliQDetectorBlockLV* aBlockLV = (MilliQDetectorBlockLV*)fDetectorStack->GetLogicalVolume()->GetDaughter(i);
        //SetSensitiveDetector(aBlockLV->GetPhotocathodeLV(), fPmt_SD.Get());
        //MilliQDetectorBlockLV* aBlockLV = (MilliQDetectorBlockLV*)fDetectorStack->GetLogicalVolume()->GetDaughter(i);
        //aBlockLV->GetPhotocathodeLV()->SetSensitiveDetector(fPmt_SD.Get());
    }
}


void MilliQDetectorConstruction::SetHousingThickness(G4double d_mtl) {
  this->fD_mtl=d_mtl;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}


void MilliQDetectorConstruction::SetPMTRadius(G4double outerRadius_pmt) {
  this->fOuterRadius_pmt=outerRadius_pmt;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}


void MilliQDetectorConstruction::SetDefaults() {

  //Resets to default values
  fD_mtl=5.*mm;

  fScint_x = 20.*cm;
  fScint_y = 10.*cm;
  fScint_z = 140*cm;

  fOuterRadius_pmt = 5.*cm;

  fRefl=1.0;

  fDetectorStack=NULL;

  G4UImanager::GetUIpointer()
    ->ApplyCommand("/MilliQ/detector/scintYieldFactor 1.");

  if(fScintillator_mt)fScintillator_mt->AddConstProperty("SCINTILLATIONYIELD",12./MeV);

  G4RunManager::GetRunManager()->ReinitializeGeometry();
}


void MilliQDetectorConstruction::SetHousingReflectivity(G4double r) {
  fRefl=r;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}


void MilliQDetectorConstruction::SetMainScintYield(G4double y) {
  fScintillator_mt->AddConstProperty("SCINTILLATIONYIELD",y/MeV);
}
