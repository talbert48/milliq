
#include "MilliQDetectorConstruction.hh"
#include "MilliQPMTSD.hh"
#include "MilliQDetectorMessenger.hh"
#include "MilliQDetectorBlockLV.hh"
#include "MilliQDetectorStackLV.hh"

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
#include "G4SubtractionSolid.hh"

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
    fWorldPV = NULL;

    fScintillatorMaterial = fAluminiumMaterial = fAirMaterial = fVacuumMaterial = fGlassMaterial = NULL;

    SetDefaults();

    fDetectorMessenger = new MilliQDetectorMessenger(this);
}


void MilliQDetectorConstruction::DefineMaterials(){
    
    G4NistManager* nist = G4NistManager::Instance();
    nist->SetVerbose(0);

    // Elements
    //                            name          symbole number  wieght
    G4Element* fH = new G4Element("Hydrogen",   "H",    1.,     1.01*g/mole);
    G4Element* fC = new G4Element("Carbon",     "C",    6.,     12.01*g/mole);
    G4Element* fN = new G4Element("Nitrogen",   "N",    7.,     14.01*g/mole);
    G4Element* fO = new G4Element("Oxegen",     "O",    8.,     16.00*g/mole);

    //
    // Materials
    //
    
    //Scintillator
    fScintillatorMaterial = new G4Material("Scintillator", //name
                                           3.020*g/cm3, //density
                                           2); //n elements
    fScintillatorMaterial->AddElement(fC,10);
    fScintillatorMaterial->AddElement(fH,11);
    
    //Aluminum
    fAluminiumMaterial = new G4Material("Aluminium", //name
                                        13., //number
                                        26.98*g/mole, //wieght
                                        2.7*g/cm3); //density
    
    //Vacuum
    fVacuumMaterial = new G4Material("Vacuum", //name
                                     1., //atomic number
                                     1.01*g/mole, //weight
                                     universe_mean_density, //density
                                     kStateGas, //state
                                     0.1*kelvin, //tempature
                                     1.e-19*pascal); //presure
    //Air
    fAirMaterial = new G4Material("Air", //name
                                  1.29*mg/cm3, //density
                                  2); //n elements
    fAirMaterial->AddElement(fN, 70*perCent); //compose of nitrogen
    fAirMaterial->AddElement(fO, 30*perCent); //compose of oxegen
    
    //Glass
    fGlassMaterial = new G4Material("Glass", //name
                                    1.032*g/cm3, //density
                                    2); //n elements
    fGlassMaterial->AddElement(fC,91.533*perCent); //compose of carbon
    fGlassMaterial->AddElement(fH,8.467*perCent); //compose of hydrogen
    
    //Concrete
    fConcreteMaterial = nist->FindOrBuildMaterial("G4_CONCRETE");

    //Shielding Material
    led = G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb");
   	polyethylene = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE");


    //
    //Material properties tables
    //
    
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
    fScintillatorMaterial->SetMaterialPropertiesTable(fScintillator_mt);

    // Set the Birks Constant for the scintillator

    fScintillatorMaterial->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

    G4double glass_RIND[]={1.49,1.49,1.49};
    assert(sizeof(glass_RIND) == sizeof(MilliQ_Energy));
    G4double glass_AbsLength[]={420.*cm,420.*cm,420.*cm};
    assert(sizeof(glass_AbsLength) == sizeof(MilliQ_Energy));
    G4MaterialPropertiesTable *glass_mt = new G4MaterialPropertiesTable();
    glass_mt->AddProperty("ABSLENGTH",MilliQ_Energy,glass_AbsLength,MilliQnum);
    glass_mt->AddProperty("RINDEX",MilliQ_Energy,glass_RIND,MilliQnum);
    fGlassMaterial->SetMaterialPropertiesTable(glass_mt);

    G4double vacuum_Energy[]={2.0*eV,7.0*eV,7.14*eV};
    const G4int vacnum = sizeof(vacuum_Energy)/sizeof(G4double);
    G4double vacuum_RIND[]={1.,1.,1.};
    assert(sizeof(vacuum_RIND) == sizeof(vacuum_Energy));
    G4MaterialPropertiesTable *vacuum_mt = new G4MaterialPropertiesTable();
    vacuum_mt->AddProperty("RINDEX", vacuum_Energy, vacuum_RIND,vacnum);
    fVacuumMaterial->SetMaterialPropertiesTable(vacuum_mt);
    fAirMaterial->SetMaterialPropertiesTable(vacuum_mt);//Give air the same rindex
}


G4VPhysicalVolume* MilliQDetectorConstruction::Construct(){

  if (fWorldPV) {
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
    //
    //World
    //
    
    //World - Volume
    G4Box* worldV = new G4Box("World Volume", //name
                              50.*m, 50.*m, 50.*m); //dimentions
    //World - Logical Volume
    G4LogicalVolume* worldLV = new G4LogicalVolume(worldV,
                                                   G4Material::GetMaterial("Air"),
                                                   "World Logical Volume",
                                                   0,0,0);
    //World - Physical Volume
    fWorldPV = new G4PVPlacement(0, //rotation
                                 G4ThreeVector(), //translation
                                 worldLV, //logical volume
                                 "World Physical Volume", //name
                                 0,false,0);
    
    //worldLV->SetVisAttributes(G4VisAttributes::Invisible);

    //Photocathode Sensitive Detectors setup
    //
    MilliQPMTSD* pmt_SD = new MilliQPMTSD("/MilliQDet/pmtSD");
    G4SDManager* sDManager = G4SDManager::GetSDMpointer();
    sDManager->AddNewDetector(pmt_SD);
    
    
    //
    // Detection Room
    //
    
    // Detection Room - Volume
    G4Box* detectionRoomV = new G4Box("Detection Room Volume",  //name
                                      10.*m, 20.*m, 20.*m); //temp dimentions
    
    // Detection Room - Logical Volume
    G4LogicalVolume* detectionRoomLV
    = new G4LogicalVolume(detectionRoomV, //volume
                          G4Material::GetMaterial("Air"), //material
                          "Detection Room Logical Volume"); //name
    
    G4ThreeVector DetectionRoomTrans = G4ThreeVector(15.*m,0.*m,0.*m);

    // Detection Room - Physical Volume
    new G4PVPlacement(0, //rotation
    				  DetectionRoomTrans, //translation
                      detectionRoomLV, //logical volume
                      "Detection Room Physical Volume", //name
                      worldLV, //mother logical volume
                      false, //many
                      0); //copy n





    //
    // Detector Stacks
    //

    // Detector Stacks - Volume
    G4Box* stackHouseingV = new G4Box("Detector Stack Housing Volume" ,
                                      1.*m , 1.*m , 1.*m );//temp dimension

    // Detector Stacks - Logical Volume
    MilliQDetectorStackLV* aDetectorStackLV
    = new MilliQDetectorStackLV(stackHouseingV, //volume
                                G4Material::GetMaterial("Air"), //material
                                "Detector Stack H", //name
                                0, //field manager
                                0, //sensitve detector
                                0, //user limits
                                true, //optimise

                                G4ThreeVector(1.,20.,10.),//20.,10.), //number of blocks
                                G4ThreeVector(1.*cm,1.*cm,1.*cm), //between block spacing

                                G4ThreeVector(140.*cm,10.*cm,20.*cm), //scintillator dimensions
                                1.*mm, //scintillator housing thickness
                                1., //scintillator housing reflectivity

                                2.5*cm, //pmt radius
                                7.*cm, //pmt height
                                2.*cm, //pmt photocathode depth from fount of pmt
                                1.*mm, //pmt housing thickness
                                4.*mm, //pmt glass thickness
                                1., //pmt housing reflective
                                pmt_SD); //pmt sensitive detector

    G4double TotalStackStart = -5.*m;
    G4double TotalStackEnd = 10.*m;

    // Detector Stacks - Parameterisation
    MilliQDetectorStackParameterisation* fDetectorStackParameterisation
    = new MilliQDetectorStackParameterisation(3, //n
                                              aDetectorStackLV->GetDimensions(), //block dimensions
                                              G4ThreeVector(-1.,0.,0.), //alignment vector
											  TotalStackStart, //start depth
											  TotalStackEnd); //end depth

    // Detector Stacks - Physical Volume
    new G4PVParameterised("Detector Stack Housing Physical Volume",
                          aDetectorStackLV,
						  detectionRoomLV,
                          kZAxis,
                          fDetectorStackParameterisation->GetNumberOfBlocks(),
                          fDetectorStackParameterisation);


    G4ThreeVector stackContainerDimentions = fDetectorStackParameterisation->GetStackDimensions();
 //   detectorStacksContainerV->SetXHalfLength(stackContainerDimentions.x()/2.);
 //   detectorStacksContainerV->SetYHalfLength(stackContainerDimentions.y()/2.);
 //   detectorStacksContainerV->SetZHalfLength(stackContainerDimentions.z()/2.);






    //
    //Shielding Around Experiment
    //

    G4ThreeVector SingleStackDimension = aDetectorStackLV->GetDimensions();
	// The shield is an open box of a certain thickness
	//Define shielding thickness
	G4ThreeVector shield1Thick = G4ThreeVector(10.0*cm,10.0*cm,10.0*cm);
	G4ThreeVector shield2Thick = G4ThreeVector(10.0*cm,10.0*cm,10.0*cm);
	//Define led shielding inner half length(polyethylene butted around led)
	G4ThreeVector shield1InnerHL= G4ThreeVector(TotalStackStart+TotalStackEnd+70*cm,SingleStackDimension.y(),SingleStackDimension.z());
	//Define global center for shielding
	G4ThreeVector centreGlobalShield = G4ThreeVector(15.0*m,0,0);
	//Define center location of led shielding (relative to global center)
	G4ThreeVector centreShield1= G4ThreeVector(0,0,0);


	//
	// Polyethylene Shielding Container (Radiation Shield)
	//
	G4ThreeVector shield2InnerHL = shield1InnerHL + shield1Thick;
	G4ThreeVector BoxOutSideShield2HL = shield2InnerHL+shield2Thick;

	G4Box *boxInSideShield2 =
			new G4Box("BoxInSideShield2", shield2InnerHL.getX(), shield2InnerHL.getY(), shield2InnerHL.getZ());

	G4Box *boxOutSideShield2=
			new G4Box("BoxOutSideShield2", BoxOutSideShield2HL.getX(), BoxOutSideShield2HL.getY(), BoxOutSideShield2HL.getZ());

	// boolean logic subtraction

	G4SubtractionSolid* OutMinusInBoxShield2=
			new G4SubtractionSolid("OutMinusInBoxShield2", boxOutSideShield2, boxInSideShield2, 0, centreGlobalShield);

	G4LogicalVolume *OutMinusInBoxShield2LV =
			new G4LogicalVolume(OutMinusInBoxShield2, polyethylene, "OutMinusInBoxShield2LV", 0, 0, 0);
			new G4PVPlacement(0, centreGlobalShield, OutMinusInBoxShield2LV, "OutMinusInBoxShield2PV", worldLV, false,0);

	// Visualisation attributes of Shield2
	G4VisAttributes * blueBox = new G4VisAttributes(G4Colour(0. ,0. ,1.));
	blueBox -> SetVisibility(true);
	blueBox -> SetForceWireframe(true);
	blueBox -> SetForceAuxEdgeVisible(true);

	OutMinusInBoxShield2LV-> SetVisAttributes(blueBox);


	//
	// Led Shielding Container (Neutron Shield)
	//

	G4ThreeVector BoxOutSideShield1HL = shield1InnerHL+shield1Thick;

	G4Box *boxInSideShield1 =
			new G4Box("BoxInSideShield1", shield1InnerHL.getX(), shield1InnerHL.getY(), shield1InnerHL.getZ());

	G4Box *boxOutSideShield1=
			new G4Box("BoxOutSideShield1", BoxOutSideShield1HL.getX(), BoxOutSideShield1HL.getY(), BoxOutSideShield1HL.getZ());

	// boolean logic subtraction

	G4SubtractionSolid* OutMinusInBoxShield1=
			new G4SubtractionSolid("OutMinusInBoxShield1", boxOutSideShield1, boxInSideShield1, 0, centreGlobalShield);

	G4LogicalVolume *OutMinusInBoxShield1LV =
			new G4LogicalVolume(OutMinusInBoxShield1, led, "OutMinusInBoxShield1LV", 0, 0, 0);
			new G4PVPlacement(0, G4ThreeVector(),OutMinusInBoxShield1LV, "OutMinusInBoxShield1PV", OutMinusInBoxShield2LV, false,0);

	// Visualisation attributes of Shield1
	G4VisAttributes * grayBox = new G4VisAttributes(G4Colour(0.5 ,0.5 ,0.5));
	grayBox -> SetVisibility(true);
	grayBox -> SetForceWireframe(true);
	grayBox -> SetForceAuxEdgeVisible(true);

	OutMinusInBoxShield1LV-> SetVisAttributes(grayBox);

    
    
    
    //
    // Concrete Wall
    //
    
    G4Box* wallV = new G4Box("Wall V",.5*m,10.*m,15.*m);
    G4LogicalVolume* wallLV = new G4LogicalVolume(wallV,
                                                fConcreteMaterial,
                                                "Wall LV",
                                                0,0,0);
    wallLV->SetVisAttributes(G4Colour(.5,.5,.5,.5));
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), wallLV, "Wall PV", worldLV, 0, 0, 0);

  return fWorldPV;
}


void MilliQDetectorConstruction::ConstructSDandField()
{
    if (!fDetectorStack) return;

    // PMT SD
    
    //MilliQPMTSD* pmt_SD = new MilliQPMTSD("/MilliQDet/pmtSD");

    
    
    /*
    if (!fPmt_SD.Get()) {
        //Created here so it exists as pmts are being placed
        G4cout << "Construction /MilliQDet/pmtSD" << G4endl;
        MilliQPMTSD* pmt_SD = new MilliQPMTSD("/MilliQDet/pmtSD");
        fPmt_SD.Put(pmt_SD);
    }
    */
    //((MilliQDetectorBlockLV*)fDetectorStack->GetLogicalVolume()->GetDaughter(0)->GetLogicalVolume())->GetPhotocathodeLV()->SetSensitiveDetector(fPmt_SD.Get());
    
    
    //for(int a=0; a<fDetectorStack->GetLogicalVolume()->GetNoDaughters(); a++){
        //fDetectorStack->GetLogicalVolume()->GetDaughter(0)->GetLogicalVolume()->GetDaughter(1)->GetLogicalVolume()->GetDaughter(1)->GetLogicalVolume()->GetName()
        
        //MilliQDetectorBlockLV* aBlockLV = (MilliQDetectorBlockLV*)fDetectorStack->GetLogicalVolume()->GetDaughter(a);

        //MilliQDetectorBlockLV* aBlockLV = (MilliQDetectorBlockLV*)fDetectorStack->GetLogicalVolume()->GetDaughter(i);
        //SetSensitiveDetector(aBlockLV->GetPhotocathodeLV(), fPmt_SD.Get());
        //MilliQDetectorBlockLV* aBlockLV = (MilliQDetectorBlockLV*)fDetectorStack->GetLogicalVolume()->GetDaughter(i);
        //aBlockLV->GetPhotocathodeLV()->SetSensitiveDetector(fPmt_SD.Get());
    //}
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
  fScint_z = 40.*cm;//140.*cm;

  fOuterRadius_pmt = 2.5*cm;

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
