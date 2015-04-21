
#include "globals.hh"

#include "MilliQDetectorBlockLV.hh"

#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4SystemOfUnits.hh"


MilliQDetectorBlockLV::MilliQDetectorBlockLV(G4VSolid*              pSolid,
                                             G4Material*            pMaterial,
                                             const G4String&        name,
                                             G4FieldManager*        pFieldMgr,
                                             G4VSensitiveDetector*  pSDetector,
                                             G4UserLimits*          pULimits,
                                             G4bool                 optimise,
                                             
                                             G4ThreeVector  scintillatorSize,
                                             G4double       housingThickness,
                                             G4double       pmtRadius,
                                             G4double       reflectivity,
                                             MilliQPMTSD*   pSD)
                    :G4LogicalVolume(pSolid,
                                     pMaterial,
                                     name,
                                     pFieldMgr,
                                     pSDetector,
                                     pULimits,
                                     optimise)
{
    _reflectivity = reflectivity;

    //
    // Scintillator
    //
    _scintillatorV = new G4Box( "Scintillator Volume" ,
                             scintillatorSize.x()/2. , scintillatorSize.y()/2. , scintillatorSize.z()/2. );
    _scintillatorLV = new G4LogicalVolume(_scintillatorV,
                                     G4Material::GetMaterial("Scintillator Material"),
                                     "Scintillator Logical Volume",
                                     0,0,0);


    new G4PVPlacement(0,
                      G4ThreeVector(),
                      _scintillatorLV,
                      "Scintillator",
                      this,
                      false,
                      0);
    //
    // PMT
    //
    G4double innerRadius_pmt    = 0.    *cm;
    G4double height_pmt         = housingThickness/2.;
    G4double startAngle_pmt     = 0.    *deg;
    G4double spanningAngle_pmt  = 360.  *deg;

    _pmtV = new G4Tubs("PMT Volume",
                       innerRadius_pmt,
                       pmtRadius,
                       height_pmt,
                       startAngle_pmt,
                       spanningAngle_pmt);
    _pmtLV = new G4LogicalVolume(_pmtV,
                                 G4Material::GetMaterial("Glass"),
                                 "PMT Logical Volume");
    new G4PVPlacement(0,
                      G4ThreeVector(0,0,scintillatorSize.z()/2. + height_pmt),
                      _pmtLV,
                      "PMT",
                      this,
                      false,
                      0);
    
    _photocathodeV = new G4Tubs("photocath_tube",
                                innerRadius_pmt,
                                pmtRadius,
                                height_pmt/2.,
                                startAngle_pmt,
                                spanningAngle_pmt);
    _photocathodeLV = new G4LogicalVolume(_photocathodeV,
                                       G4Material::GetMaterial("Al"),
                                       "Photocathode Logical Volume",
                                          0,
                                          pSD,
                                          0,
                                          true);
    new G4PVPlacement(0,
                      G4ThreeVector(0,0,-height_pmt/2),
                      _photocathodeLV,
                      "Photocathode",
                      _pmtLV,
                      false,
                      0);

    VisAttributes();
    SurfaceProperties();

}


void MilliQDetectorBlockLV::VisAttributes(){
    _scintillatorLV->SetVisAttributes(G4Colour(1.0,0.5,0.0,.4));
    _pmtLV->SetVisAttributes(G4Colour(0.0,1.0,1.0,.4));
    this->SetVisAttributes(G4Colour(1.0,0.0,1.0,.2));
}


void MilliQDetectorBlockLV::SurfaceProperties(){
  G4double energy[] = {7.0*eV, 7.14*eV};
  const G4int num = sizeof(energy)/sizeof(G4double);

  //**Scintillator housing properties
  G4double reflectivity[] = {_reflectivity, _reflectivity};
  assert(sizeof(reflectivity) == sizeof(energy));
  G4double efficiency[] = {0.0, 0.0};
  assert(sizeof(efficiency) == sizeof(energy));
  G4MaterialPropertiesTable* scintHsngPT = new G4MaterialPropertiesTable();
  scintHsngPT->AddProperty("REFLECTIVITY", energy, reflectivity, num);
  scintHsngPT->AddProperty("EFFICIENCY", energy, efficiency, num);
  G4OpticalSurface* OpScintHousingSurface =
    new G4OpticalSurface("HousingSurface",unified,polished,dielectric_metal);
  OpScintHousingSurface->SetMaterialPropertiesTable(scintHsngPT);
 
  //**Photocathode surface properties
  G4double photocath_EFF[]={1.,1.}; //Enables 'detection' of photons
  assert(sizeof(photocath_EFF) == sizeof(energy));
  G4double photocath_ReR[]={1.92,1.92};
  assert(sizeof(photocath_ReR) == sizeof(energy));
  G4double photocath_ImR[]={1.69,1.69};
  assert(sizeof(photocath_ImR) == sizeof(energy));
  G4MaterialPropertiesTable* photocath_mt = new G4MaterialPropertiesTable();
  photocath_mt->AddProperty("EFFICIENCY",energy,photocath_EFF,num);
  photocath_mt->AddProperty("REALRINDEX",energy,photocath_ReR,num);
  photocath_mt->AddProperty("IMAGINARYRINDEX",energy,photocath_ImR,num);
  G4OpticalSurface* photocath_opsurf=
    new G4OpticalSurface("photocath_opsurf",glisur,polished,
                         dielectric_metal);
  photocath_opsurf->SetMaterialPropertiesTable(photocath_mt);

  //**Create logical skin surfaces
  new G4LogicalSkinSurface("photocath_surf",this,
                           OpScintHousingSurface);
  new G4LogicalSkinSurface("photocath_surf",_photocathodeLV,photocath_opsurf);
}
