#include "globals.hh"

#include "MilliQDetectorBlockLV.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"

#include "G4LogicalSkinSurface.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4PhysicalConstants.hh"

#include "G4SystemOfUnits.hh"

MilliQDetectorBlockLV::MilliQDetectorBlockLV(G4VSolid* pSolid,
		G4Material* pMaterial, const G4String& pName,
		G4FieldManager* pFieldManager, G4VSensitiveDetector* pSD,
		G4UserLimits* pUserLimits, G4bool pOptimise,

		G4ThreeVector pScintillatorDimensions,
		G4double pScintillatorHousingThickness, G4double pLightGuideLength,
		G4double pScintillatorHousingReflectivity,

		G4double pPmtRadius, G4double pPmtPhotocathodeRadius,
		G4double pPmtPhotocathodeHeight, G4double pPmtPhotocathodeDepth,
		G4double pPmtHousingReflectivity, G4VSensitiveDetector* pPmtSD,
		G4VSensitiveDetector* pScintSD) :
		G4LogicalVolume(pSolid, pMaterial, pName, pFieldManager, pSD,
				pUserLimits, pOptimise) {

	//Assigning proper values, making it all consistent:
	G4double pPmtGlassHeight = pScintillatorHousingThickness;

	if (pLightGuideLength > 0.9 * pScintillatorDimensions.x())
		pLightGuideLength = 0.9 * pScintillatorDimensions.x();

	if (pPmtPhotocathodeHeight > pScintillatorHousingThickness)
		pPmtPhotocathodeHeight = pScintillatorHousingThickness;

	if (pPmtRadius
			> std::min(pScintillatorDimensions.y() / 2.,
					pScintillatorDimensions.z() / 2.)) {
		pPmtRadius = std::min(pScintillatorDimensions.y() / 2.,
				pScintillatorDimensions.z() / 2.);
	}

	if (pPmtPhotocathodeRadius > pPmtRadius)
		pPmtPhotocathodeRadius = pPmtRadius;

	if (pPmtPhotocathodeDepth > pPmtGlassHeight - pPmtPhotocathodeHeight)
		pPmtPhotocathodeDepth = pPmtGlassHeight - pPmtPhotocathodeHeight;

	//
	// Detector Block(this)
	//

	fDimensions = G4ThreeVector(
			pScintillatorDimensions.x() + 2 * pScintillatorHousingThickness,
			pScintillatorDimensions.y() + 2 * pScintillatorHousingThickness,
			pScintillatorDimensions.z() + 2 * pScintillatorHousingThickness);

	G4Box* detectorBlockV = new G4Box("Detector Block Volume",
			fDimensions.x() / 2., fDimensions.y() / 2., fDimensions.z() / 2.);

	this->SetSolid(detectorBlockV);

	//****************************************************************************************************************************

	// Scintillator Housing - Volume
	G4Box* scintillatorHousingV = new G4Box("Scintillator Housing Volume", //name
			pScintillatorDimensions.x() / 2. + pScintillatorHousingThickness, //half x dimension
			pScintillatorDimensions.y() / 2. + pScintillatorHousingThickness, //half y dimension
			pScintillatorDimensions.z() / 2. + pScintillatorHousingThickness); //half z dimension

	// Scintillator Housing - Logical Volume
	fScintillatorHousingLV = new G4LogicalVolume(scintillatorHousingV, //volume
			G4Material::GetMaterial("Aluminium"), //material
			"Scintillator Housing Logical Volume"); //name
	// Scintillator Housing - Physical Volume
	new G4PVPlacement(0,                                              //rotation
			G4ThreeVector(0., 0, 0),     //translation
			fScintillatorHousingLV,                             //logical volume
			"Scintillator Housing Physical Volume",                       //name
			this,                                        //mother logical volume
			false,                                                        //many
			0);                                                         //copy n

	// Scintillator - Volume
	G4Box* scintillatorV = new G4Box("Scintillator Volume",             //name
			pScintillatorDimensions.x() / 2. - pLightGuideLength, //half x dimension
			pScintillatorDimensions.y() / 2.,    //half y dimension
			pScintillatorDimensions.z() / 2.);   //half z dimension

	// Scintillator - Logical Volume
	fScintillatorLV = new G4LogicalVolume(scintillatorV,                //volume
			G4Material::GetMaterial("Scintillator"),  //material
			"Scintillator Logical Volume",			//name
			0, 										//field manager
			pScintSD, 								//sensitive detector
			0, 										//userlimits
			true); 									//optimise
	// Scintillator - Physical Volume
	new G4PVPlacement(0,                                    //rotation
			G4ThreeVector(-pLightGuideLength, 0, 0),               //translation
			fScintillatorLV,                      //logical volume
			"Scintillator Physical Volume",       //name
			fScintillatorHousingLV,               //mother logical volume
			false,                                //many
			0);                                   //copy n

	G4RotationMatrix* rotm = new G4RotationMatrix(); // This orientation makes the sensitive part farthest away from beam
	rotm->rotateX(90 * deg);
	rotm->rotateY(270 * deg);
	rotm->rotateZ(0 * deg);

	//Light Guide
	// Light Guide - Volume

	G4Trd* lightGuideV = new G4Trd("Light Guide Volume",
			pScintillatorDimensions.y() / 2, pPmtPhotocathodeRadius,
			pScintillatorDimensions.z() / 2, pPmtPhotocathodeRadius,
			pLightGuideLength);

	// Light Guide - Logical Volume
	fLightGuideLV = new G4LogicalVolume(lightGuideV,                    //volume
			G4Material::GetMaterial("Scintillator"),  //material
			"Light Guide Logical Volume",			//name
			0, 										//field manager
			pScintSD, 								//sensitive detector
			0, 										//userlimits
			true); 									//optimise
	// Light Guide - Physical Volume
	new G4PVPlacement(
			rotm,                                    //rotation
			G4ThreeVector(pScintillatorDimensions.x() / 2 - pLightGuideLength,
					0, 0),                 //translation
			fLightGuideLV,                       //logical volume
			"Light Guide Physical Volume",       //name
			fScintillatorHousingLV,               //mother logical volume
			false,                                //many
			0);                                   //copy n
//////////////////////

	//
	// PMT
	// Composed of "PMT Glass", which contains (on the farthest side from the scintillator)
	// the "PMT Photocathode Section".

	// PMT Glass - Volume
	G4Tubs* pmtGlassV = new G4Tubs("PMT Glass Volume",  //name
			0. * cm,               //inner radius
			pPmtRadius,          //outer radius
			pPmtGlassHeight / 2.,  //half height
			0. * deg,              //start angle
			360. * deg);           //end angle
	// PMT Glass - Logical Volume
	fPmtGlassLV = new G4LogicalVolume(pmtGlassV,                        //volume
			G4Material::GetMaterial("Glass"),   //material
			"PMT Glass Logical Volume");            //name
	// PMT Glass - Physical Volume
	new G4PVPlacement(
			rotm,                                                     //rotation
			G4ThreeVector(
					pScintillatorDimensions.x() / 2.
							+ pScintillatorHousingThickness / 2., 0, 0), //translation
			fPmtGlassLV,                                        //logical volume
			"PMT Glass Physical Volume",                                  //name
			fScintillatorHousingLV,                      //mother logical volume
			false,                                                        //many
			0);                                                         //copy n

	// PMT Photocathode Section - Volume
	G4Tubs* pmtPhotocathodeSectionV = new G4Tubs(
			"PMT Photocathode Section Volume", //name
			0. * cm,                                   //inner radius
			pPmtRadius,           //outer radius
			pPmtPhotocathodeHeight / 2.,          //half height
			0. * deg,                                  //start angle
			360. * deg);                               //end angle

	// PMT Photocathode Section - Logical Volume
	fPmtPhotocathodeSectionLV = new G4LogicalVolume(pmtPhotocathodeSectionV, //volume
			G4Material::GetMaterial("Aluminium"),  //material
			"PMT Photocathode Section Logical Volume", //name
			0, //field manager
			pPmtSD, //sensitive detector
			0, //userlimits
			true); //optimise

	// PMT Photocathode Section - Physical Volume
	new G4PVPlacement(
			0,                                            //rotation
			G4ThreeVector(0., 0,
					pScintillatorHousingThickness / 2.
							- pPmtPhotocathodeHeight / 2.
							- pPmtPhotocathodeDepth),  //translation
			fPmtPhotocathodeSectionLV,                    //logical volume
			"PMT Photocathode Section Physical Volume",        //name
			fPmtGlassLV,							//mother logical volume
			false,                                        //many
			0);

	VisAttributes();
	SurfaceProperties(pScintillatorHousingReflectivity,
			pPmtHousingReflectivity);
}

void MilliQDetectorBlockLV::VisAttributes() {
	fScintillatorHousingLV->SetVisAttributes(G4Colour::Red());
	fScintillatorLV->SetVisAttributes(G4Colour::Blue());
	fLightGuideLV->SetVisAttributes(G4Colour::Blue()); //(0.94, 0.5, 0.5)); // Pink

	//   fPmtHousingLV->SetVisAttributes(G4Colour::Yellow());
	fPmtGlassLV->SetVisAttributes(G4Colour::Cyan());
	//   fPmtVacuumSectionLV->SetVisAttributes(G4Colour::Brown());
	fPmtPhotocathodeSectionLV->SetVisAttributes(G4Colour::Magenta());

	this->SetVisAttributes(G4Colour::Gray());
}

//Settings for BC408
void MilliQDetectorBlockLV::SurfaceProperties(
		G4double pScintillatorHousingReflectivity,
		G4double pPmtHousingReflectivity) {

	G4double energy[] = { 2.38 * eV, 2.43 * eV, 2.48 * eV, 2.53 * eV, 2.58 * eV,
			2.64 * eV, 2.70 * eV, 2.76 * eV, 2.79 * eV, 2.82 * eV, 2.88 * eV,
			2.95 * eV, 2.98 * eV, 3.02 * eV, 3.10 * eV, 3.18 * eV };
	const G4int num = sizeof(energy) / sizeof(G4double);
	G4double pshr = pScintillatorHousingReflectivity;

	//**Scintillator housing properties
	G4double reflectivity[] = { pshr, pshr, pshr, pshr, pshr, pshr, pshr, pshr,
			pshr, pshr, pshr, pshr, pshr, pshr, pshr, pshr };
	assert(sizeof(reflectivity) == sizeof(energy));
	G4double efficiency[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	assert(sizeof(efficiency) == sizeof(energy));

	G4MaterialPropertiesTable* scintHsngPT = new G4MaterialPropertiesTable();
	scintHsngPT->AddProperty("REFLECTIVITY", energy, reflectivity, num);
	scintHsngPT->AddProperty("EFFICIENCY", energy, efficiency, num);
	G4OpticalSurface* OpScintHousingSurface = new G4OpticalSurface(
			"HousingSurface");
	OpScintHousingSurface->SetType(dielectric_metal);
	OpScintHousingSurface->SetFinish(polished);
	OpScintHousingSurface->SetModel(glisur);
	OpScintHousingSurface->SetMaterialPropertiesTable(scintHsngPT);


	//**Photocathode surface properties
	G4double photocath_EFF[] = { 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.,
			1., 1., 1., 1. }; //Enables 'detection' of photons
	assert(sizeof(photocath_EFF) == sizeof(energy));
	G4double photocath_ReR[] = { 1.92, 1.92, 1.92, 1.92, 1.92, 1.92, 1.92, 1.92,
			1.92, 1.92, 1.92, 1.92, 1.92, 1.92, 1.92, 1.92 };
	assert(sizeof(photocath_ReR) == sizeof(energy));
	G4double photocath_ImR[] = { 1.69, 1.69, 1.69, 1.69, 1.69, 1.69, 1.69, 1.69,
			1.69, 1.69, 1.69, 1.69, 1.69, 1.69, 1.69, 1.69 };
	assert(sizeof(photocath_ImR) == sizeof(energy));
	G4MaterialPropertiesTable* photocath_mt = new G4MaterialPropertiesTable();
	photocath_mt->AddProperty("EFFICIENCY", energy, photocath_EFF, num);
	photocath_mt->AddProperty("REALRINDEX", energy, photocath_ReR, num);
	photocath_mt->AddProperty("IMAGINARYRINDEX", energy, photocath_ImR, num);
	G4OpticalSurface* photocath_opsurf = new G4OpticalSurface(
			"photocath_opsurf", glisur, polished, dielectric_metal);
	photocath_opsurf->SetMaterialPropertiesTable(photocath_mt);

	//**Create logical skin surfaces
	new G4LogicalSkinSurface("photocath_surf",fScintillatorHousingLV,
			OpScintHousingSurface);
//	new G4LogicalSkinSurface("photocath_surf", this, OpScintHousingSurface);
	new G4LogicalSkinSurface("photocath_surf", fPmtPhotocathodeSectionLV,
			photocath_opsurf);
}

//****************************************************************************************************************************
// If running this version, uncomment some visattributes below

/*

 G4double detectorBlockHalfDimensionX = pScintillatorHousingThickness+(pScintillatorDimensions.x() + pPmtHousingThickness+pPmtHeight)/2.; //find half z dimension of detector block

 G4double detectorBlockHalfDimensionY = pScintillatorHousingThickness+(pScintillatorDimensions.y()/2.) > pPmtHousingThickness+pPmtRadius
 ? pScintillatorHousingThickness+(pScintillatorDimensions.y()/2.) : pPmtHousingThickness+pPmtRadius; //find max half y dimension of detector block

 G4double detectorBlockHalfDimensionZ = pScintillatorHousingThickness+(pScintillatorDimensions.z()/2.) > pPmtHousingThickness+pPmtRadius
 ? pScintillatorHousingThickness+(pScintillatorDimensions.z()/2.) : pPmtHousingThickness+pPmtRadius; //find max half x dimension of detector block

 fDimensions = G4ThreeVector(2.*detectorBlockHalfDimensionX,2.*detectorBlockHalfDimensionY,2.*detectorBlockHalfDimensionZ);



 //
 // Scintillator
 // Composed of "Scintillator Housing" with a block of "Scintillator" inside
 //

 // Scintillator Housing - Volume
 G4Box* scintillatorHousingV = new G4Box("Scintillator Housing Volume",                          //name
 pScintillatorDimensions.x()/2.+pScintillatorHousingThickness,  //half x dimension
 pScintillatorDimensions.y()/2.+pScintillatorHousingThickness,  //half y dimension
 pScintillatorDimensions.z()/2.+pScintillatorHousingThickness); //half z dimension

 // Scintillator Housing - Logical Volume
 fScintillatorHousingLV = new G4LogicalVolume(scintillatorHousingV, //volume
 G4Material::GetMaterial("Aluminium"), //material
 "Scintillator Housing Logical Volume"); //name
 // Scintillator Housing - Physical Volume
 new G4PVPlacement(0,                                                            //rotation
 G4ThreeVector(-(pPmtHeight+pPmtHousingThickness)/2.,0,0),     //translation
 fScintillatorHousingLV,                                       //logical volume
 "Scintillator Housing Physical Volume",                       //name
 this,                                                         //mother logical volume
 false,                                                        //many
 0);                                                           //copy n

 // Scintillator - Volume
 G4Box* scintillatorV = new G4Box("Scintillator Volume",             //name
 pScintillatorDimensions.x()/2.,    //half x dimension
 pScintillatorDimensions.y()/2.,    //half y dimension
 pScintillatorDimensions.z()/2.);   //half z dimension


 // Scintillator - Logical Volume
 fScintillatorLV = new G4LogicalVolume(scintillatorV,                            //volume
 G4Material::GetMaterial("Scintillator"),  //material
 "Scintillator Logical Volume",			//name
 0, 										//field manager
 pScintSD, 								//sensitive detector
 0, 										//userlimits
 true); 									//optimise
 // Scintillator - Physical Volume
 new G4PVPlacement(0,                                    //rotation
 G4ThreeVector(0,0,0),                 //translation
 fScintillatorLV,                      //logical volume
 "Scintillator Physical Volume",       //name
 fScintillatorHousingLV,               //mother logical volume
 false,                                //many
 0);                                   //copy n

 //
 // PMT
 // Composed of "PMT Housing" with "PMT Glass" inside.
 // Inside of "PMT Glass" is split.  The side farthest from the sinillator
 // is "PMT Photocathode Section" the rest is "PMT Vaccume Section".
 // The varible pPmtPhotocathodeDepth sets how far from the sinilator side of
 // the PMT the photocathode section starts and vaccume section ends.
 //

 // PMT Housing - Volume
 G4Tubs* pmtHousingV = new G4Tubs("PMT Housing Volume",                  //name
 0.*cm,                                 //inner radius
 pPmtRadius+pPmtHousingThickness,       //outer radius
 (pPmtHeight+pPmtHousingThickness)/2.,  //half height
 0.*deg,                                //start angle
 360.*deg);                             //end angle
 // PMT Housing - Logical Volume
 fPmtHousingLV = new G4LogicalVolume(pmtHousingV,                            //volume
 G4Material::GetMaterial("Aluminium"), //material
 "PMT Housing Logical Volume");          //name

 G4RotationMatrix* rotm  = new G4RotationMatrix(); // This orientation makes the sensitive part farthest away from beam
 rotm->rotateX(90*deg);
 rotm->rotateY(270*deg);
 rotm->rotateZ(0*deg);

 // PMT Housing - Physical Volume
 new G4PVPlacement(rotm,                                                                             //rotation
 G4ThreeVector((fDimensions.x()/2.)-(pPmtHeight+pPmtHousingThickness)/2.,0,0),     //translation
 fPmtHousingLV,                                                                    //logical volume
 "PMT Housing Physical Volume",                                                    //name
 this,                                                                             //mother logical volume
 false,                                                                            //many
 0);                                                                               //copy n

 // PMT Glass - Volume
 G4Tubs* pmtGlassV = new G4Tubs("PMT Glass Volume",  //name
 0.*cm,               //inner radius
 pPmtRadius,          //outer radius
 pPmtHeight/2.,       //half height
 0.*deg,              //start angle
 360.*deg);           //end angle
 // PMT Glass - Logical Volume
 fPmtGlassLV = new G4LogicalVolume(pmtGlassV,                                //volume
 G4Material::GetMaterial("Glass"),   //material
 "PMT Glass Logical Volume");            //name
 // PMT Glass - Physical Volume
 new G4PVPlacement(0,                                                                //rotation
 G4ThreeVector(0,0,-pPmtHousingThickness/2.),                      //translation
 fPmtGlassLV,                                                      //logical volume
 "PMT Glass Physical Volume",                                      //name
 fPmtHousingLV,                                                    //mother logical volume
 false,                                                            //many
 0);                                                               //copy n

 // PMT Vacuum Section - Volume
 G4Tubs* pmtVacuumSectionV = new G4Tubs("PMT Vacuum Section Volume", //name
 0.*cm,                               //inner radius
 pPmtRadius-pPmtGlassThickness,       //outer radius
 pPmtPhotocathodeDepth/2.,            //half height
 0.*deg,                              //start angle
 360.*deg);                           //end angle
 // PMT Vacuum Section - Logical Volume
 fPmtVacuumSectionLV = new G4LogicalVolume(pmtVacuumSectionV,                      //volume
 G4Material::GetMaterial("Vacuum"),  //material
 "PMT Vacuum Section Logical Volume");           //name
 // PMT Vacuum Section - Physical Volume
 new G4PVPlacement(0,                                                            //rotation
 G4ThreeVector(0,0,-(pPmtHeight-pPmtPhotocathodeDepth)/2.),    //translation
 fPmtVacuumSectionLV,                                          //logical volume
 "PMT Vacumm Scection Physical Volume",                        //name
 fPmtGlassLV,                                                  //mother logical volume
 false,                                                        //many
 0);                                                           //copy n

 // PMT Photocathode Section - Volume
 G4Tubs* pmtPhotocathodeSectionV = new G4Tubs("PMT Photocathode Section Volume", //name
 0.*cm,                                   //inner radius
 pPmtRadius-pPmtGlassThickness,           //outer radius
 (pPmtHeight-pPmtPhotocathodeDepth)/2.,   //half height
 0.*deg,                                  //start angle
 360.*deg);                               //end angle

 //this->SetSolid(pmtPhotocathodeSectionV);

 // PMT Photocathode Section - Logical Volume
 fPmtPhotocathodeSectionLV = new G4LogicalVolume(pmtPhotocathodeSectionV, //volume
 G4Material::GetMaterial("Aluminium"),  //material
 "PMT Photocathode Section Logical Volume", //name
 0, //field manager
 pPmtSD, //sensitive detector
 0, //userlimits
 true); //optimise
 //  fPmtPhotocathodeSectionLV->SetSensitiveDetector(pPmtSD);

 // PMT Photocathode Section - Physical Volume
 new G4PVPlacement(0,                                            //rotation
 G4ThreeVector(0,0,pPmtPhotocathodeDepth/2.),  //translation
 fPmtPhotocathodeSectionLV,                    //logical volume
 "PMT Vacumm Scection Physical Volume",        //name
 fPmtGlassLV,							//mother logical volume
 false,                                        //many
 0);
 */
