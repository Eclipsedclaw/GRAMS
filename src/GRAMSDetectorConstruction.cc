//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// @author Tsuguo Aramaki
// @date 2015 March 23
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "GAPSDetectorConstruction.hh"
#include "GAPSDetectorMessenger.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Ellipsoid.hh"
#include "G4Trd.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4AssemblyVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4RunManager.hh"

#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4ProductionCuts.hh"

#include "G4String.hh"
#include "G4UserLimits.hh"
#include "G4Colour.hh"
#include "G4ios.hh"
#include "global.h"
#include "G4SystemOfUnits.hh"

/* copy number
 maximum layer: 50
 maximum channel: 10
 maximum channel/layer: 2000
 
 // copy number
 copySiLi = 0;
 copyLayer = 0;
 copyFrame = 2000;
 copyTOFout = 10000;
 copyTOFin = 11000;
 copyScore1 = 12000;
 copyScore2 = 12001; // reference plane
 copyAtmosphere = 12002;
 copyOutlet = 12003;
 copyPipe = 12004;
 copyElectronics = 12005;
 copyWorld = -1;
*/


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GAPSDetectorConstruction::GAPSDetectorConstruction()
:solidWorld(0),logicWorld(0),physiWorld(0)
{
  DefineMaterials();
	detectorMessenger = new GAPSDetectorMessenger(this);
	
	// fixed value
	atmosL = 50.0*m;
	scoreL = atmosL;
	scoreZ = 0.001*cm;
	TOFspace = 100.0*cm;
	OutletT = 10.0*cm;
	OutletL = 15.0*cm;
	rPipe = 0.5*cm;//0.625*cm, 0.95*cm;
	
	// default parameter
	NbOfLayers = 10;
	LayerSpace = 20.0*cm;
	LayerL = 200.0*cm;
	zSiLi = 0.25*cm;
	FrameZ = 0.32*cm;
	TOFinZ = 0.5*cm;
	TOFoutZ = 0.5*cm;
	TOFoutL = 2.0*TOFspace+TOFinL;
	TOFoutH = TOFspace+0.5*TOFinH;
	TOFoutAngle = 90.0*deg;
	atmosZ = 3.9*cm;

	
	extern global_struct global;
	sprintf(global.outdir, "tmp");
	sprintf(global.outfile, "test");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GAPSDetectorConstruction::~GAPSDetectorConstruction()
{
  delete detectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GAPSDetectorConstruction::DefineMaterials()
{
	//------------------------------------------------
	// Material definition
	//------------------------------------------------
	
  G4double a, z;
  G4double density, temperature, pressure;
  G4int nel;
	
  //Air
  G4Element* N = new G4Element("Nitrogen", "N", z=7., a= 14.01*g/mole);
  G4Element* O = new G4Element("Oxygen"  , "O", z=8., a= 16.00*g/mole);
	
  Air = new G4Material("Air", density= 1.29*mg/cm3, nel=2);
  Air->AddElement(N, 70*perCent);
  Air->AddElement(O, 30*perCent);
	
	// Vacuum
  density= 2.376e-15*g/cm3;
  temperature= 300*kelvin;
  pressure= 2.0e-7*bar;
  Vacuum = new G4Material("Vacuum", density, nel=1, kStateGas,temperature,pressure);
  Vacuum-> AddMaterial(Air, 100*perCent);
	
	//Compressed Air
  CompressedAir = new G4Material("CompressedAir", density= 1.0*g/cm3, nel=2);
  CompressedAir->AddElement(N, 70*perCent);
  CompressedAir->AddElement(O, 30*perCent);
	
	//Air at flight altitude
	density= 1.0e-6*g/cm3;
  temperature= 225*kelvin;
  pressure= 1.0e-3*bar;
  FlightAir = new G4Material("FlightAir", density, nel=1, kStateGas,temperature,pressure);
  FlightAir-> AddMaterial(Air, 100*perCent);
	
  //Silicon
	Silicon= new G4Material("Silicon", z=14., a=28.0855*g/mole, density=2.330*g/cm3);
	
	//Plastic
	G4Element* elH = new G4Element("Hydrogen", "H", z=1., a = 1.00*g/mole);
	G4Element* elC = new G4Element("Carbon","C", z=6., a = 12.00*g/mole);
	Plastic = new G4Material("Plastic", density = 1.032*g/cm3, nel=2);
	Plastic->AddElement(elH, 0.085);
	Plastic->AddElement(elC, 0.915);
	
  //Al
	Al = new G4Material("Al",z= 13., a= 26.98*g/mole, density= 2.7*g/cm3);
	
	
	//Carbon
	Carbon = new G4Material("Carbon", z= 6., a= 12.01*g/mole, density= 2.265*g/cm3);
	
  //Fe
	Fe = new G4Material("Fe", z=26., a = 55.845*g/mole, density = 7.87*g/cm3 );
	
  //Pb
	Pb = new G4Material("Pb", z=82., a = 207.19*g/mole, density = 11.35*g/cm3);
	
  // Print all the materials defined.
  //
  G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

G4VPhysicalVolume* GAPSDetectorConstruction::Construct()
{
	return ConstructGAPSDetector();
}
G4VPhysicalVolume* GAPSDetectorConstruction::ConstructGAPSDetector()
{
	extern global_struct global;
	overlap = global.CheckOverlap;
	
	//------------------------------------------------
	// parameter setup
	//------------------------------------------------
	
	int cType = 0; // cooling type: 0 for new design, 1 for old design
	copyX = 12;
	copyY = 12;
	FrameL = 13.0*cm;
		
	TOFinL = LayerL;
	TOFinH = LayerSpace*NbOfLayers+TOFinZ;
	LayerZ = max(FrameZ,zSiLi);
	G4double referenceL = TOFinL;
    if(global.SimulationType == 5) FrameZ = zSiLi;
    
	if(cType == 1)
	{
		LayerZ = LayerSpace;
		G4cout << "***** old cooling design *****"<< G4endl;
	}
	OutletZ = TOFinH;
	
	if(global.DetectorType == 0) // 4 inch detector, 1 strips
	{
		rSiLi = 5.0*cm;
		NbOfStrips = 1;
	}
	else if(global.DetectorType == 1) // 4 inch detector, 2 strips
	{
		rSiLi = 5.0*cm;
		NbOfStrips = 2;
	}
  else if(global.DetectorType == 2) // 4 inch detector, 4 strips
  {
    rSiLi = 5.0*cm;
    NbOfStrips = 4;
  }
  else if(global.DetectorType == 3) // 4 inch detector, 8 strips
  {
    rSiLi = 5.0*cm;
    NbOfStrips = 8;
  }
	else if(global.DetectorType == 4) // 2 inch detector, 1 strip
	{
		copyX = copyX*2;
		copyY = copyY*2;
		rSiLi = 2.5*cm;
		FrameL = FrameL*0.5;
		NbOfStrips = 1;
	}
	else if(global.DetectorType == 5) // 2 inch detector, 2 strips
	{
		copyX = copyX*2;
		copyY = copyY*2;
		rSiLi = 2.5*cm;
		FrameL = FrameL*0.5;
		NbOfStrips = 2;
	}
  else if(global.DetectorType == 6) // 2 inch detector, 2 strips
  {
    copyX = copyX*2;
    copyY = copyY*2;
    rSiLi = 2.5*cm;
    FrameL = FrameL*0.5;
    NbOfStrips = 2;
  }
	else cout << "***** choose proper detector type *****" << endl;
    
	NbOfSiLi = copyX*copyY; // Si(Li)/frame per layer
	
	//------------------------------------------------
	// Sensitive detectors
	//------------------------------------------------
	
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
	if(!ScoreSD)
	{
		ScoreSD = new GAPSDetectorSD("ScoreSD");
		SDman->AddNewDetector(ScoreSD);
	}
	if(!TOFSD)
	{
		TOFSD = new GAPSDetectorSD("TOFSD");
		SDman->AddNewDetector(TOFSD);
	}
	if(!SiliconSD)
	{
		SiliconSD = new GAPSDetectorSD("SiliconSD");
		
		SDman->AddNewDetector(SiliconSD);
	}
	if(!FrameSD)
	{
		FrameSD = new GAPSDetectorSD("FrameSD");
		SDman->AddNewDetector(FrameSD);
	}
	
	//------------------------------------------------
	// Copy Number
	//------------------------------------------------
	
	copySiLi = 0;
	copyLayer = 0;
	copyFrame = 2000;
	copyTOFout = 10000;
	copyTOFin = 11000;
	copyScore1 = 12000;
	copyScore2 = 12001; // reference plane
	copyAtmosphere = 12002;
	copyOutlet = 12003;
	copyPipe = 12004;
	copyElectronics = 12005;
	copyWorld = -1;
	
	//--------- Definitions of Solids, Logical Volumes, Physical Volumes ---------
 
    
    //------------------------------------------------
    // Material
    //------------------------------------------------

    worldMaterial = FlightAir;
    atmosMaterial = CompressedAir;
    scoreMaterial = Vacuum;
    layerMaterial = FlightAir;
    if(global.SimulationType == 6) layerMaterial = Al;
    TOFframeMaterial = Plastic;
    SiLiFrameMaterial = Al;
    if(global.SimulationType == 5) SiLiFrameMaterial = Silicon;
    SiLiMaterial = Silicon;
    if(global.SimulationType == 6) SiLiMaterial = Al;
    outletMaterial = Al;
    pipeMaterial = Carbon;
    
	//------------------------------------------------
	// World
	//------------------------------------------------
	
	
  WorldLength = 100.0*m;
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(WorldLength);
	//  G4cout << "Computed tolerance = "
	//         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
	//         << " mm" << G4endl;
	
  solidWorld= new G4Box("world",WorldLength*0.5,WorldLength*0.5,WorldLength*0.5);
  logicWorld= new G4LogicalVolume( solidWorld, worldMaterial, "World", 0, 0, 0);
  
  //  Must place the World Physical volume unrotated at (0,0,0).
  //
  physiWorld = new G4PVPlacement(0,               // no rotation
                                 G4ThreeVector(), // at (0,0,0)
                                 logicWorld,      // its logical volume
																 "World",         // its name
                                 0,               // its mother  volume
                                 false,           // no boolean operations
                                 copyWorld,								// copy number
																 true);
	
	
	//------------------------------------------------
	
	
	global.beamZ = TOFspace+0.5*TOFinH+0.5*TOFoutZ+atmosZ+scoreZ;
	//	global.beamZ = 200.0*cm+0.5*TOFoutZ+atmosZ+scoreZ;
	cout << "*******************" << endl;
	
	if(global.SimulationType == 0) cout << "Reference layer length: " << referenceL/cm << " cm" << endl;
	if(global.SimulationType > 0)
	{
		cout << "Number of Layers: " << NbOfLayers << endl;
		cout << "Layer length: " << LayerL/cm << " cm" << endl;
		cout << "Layer space: " << LayerSpace/cm << " cm" << endl;
    cout << "Layer thickness: " << LayerZ/cm << " cm" << endl;
    cout << "Number of Si(Li) per Layers: " << NbOfSiLi << endl;
    cout << "Number of Strips per Si(Li) wafer: " << NbOfStrips << endl;
    cout << "Si(Li) diameter: " << rSiLi/cm*2.0 << " cm" << endl;
    cout << "Si(Li) thickness: " << zSiLi/cm << " cm" << endl;
    cout << "Si(Li) frame material: " << SiLiFrameMaterial->GetName() << endl;
    cout << "Si(Li) frame thickness: " << FrameZ/cm << " cm" << endl;
		cout << "TOFin thickness: " << TOFinZ/cm << " cm" << endl;
		cout << "TOFin length: " << TOFinL/cm << " cm" << endl;
		cout << "TOFin height: " << TOFinH/cm << " cm" << endl;
		cout << "TOFout thickness: " << TOFoutZ/cm << " cm" << endl;
		cout << "TOFout length: " << TOFoutL/cm << " cm" << endl;
		cout << "TOFout height: " << TOFoutH/cm << " cm" << endl;
		cout << "TOFout angle: " << TOFoutAngle/deg << " deg" << endl;
		cout << "Atmos depth: " << atmosZ/cm << " g/cm2" << endl;
	}
	
	if(global.SimulationType <= 1)
	{
		cout << "primary particle area: " << global.area/m*global.area/m << " m^2" << endl;
		cout << "primary particle minimum energy: " << global.Emin/MeV << " MeV" << endl;
		cout << "primary particle maximum energy: " << global.Emax/MeV << " MeV" << endl;
	}
	
	// output to file
	char fname[100];
	G4String str = global.outdir;
	if(str != "tmp")
	{
		sprintf(fname, "%s/%s_geometry.txt", global.outdir, global.outfile );
		G4cout << "Geometry output file: " << fname << G4endl;
		ofstream output_geometry;
		output_geometry.open (fname);
		
		if(global.SimulationType == 0) output_geometry << "Reference layer length: " << referenceL/cm << " cm" << endl;
		if(global.SimulationType > 0)
		{
			output_geometry << "Number of Layers: " << NbOfLayers << endl;
			output_geometry << "Layer length: " << LayerL/cm << " cm" << endl;
			output_geometry << "Layer space: " << LayerSpace/cm << " cm" << endl;
      output_geometry << "Layer thickness: " << LayerZ/cm << " cm" << endl;
      output_geometry << "Number of Si(Li) per layer: " << NbOfSiLi << endl;
      output_geometry << "Number of Strips per Si(Li) wafer: " << NbOfStrips << endl;
      output_geometry << "Si(Li) diameter: " << rSiLi/cm*2.0 << " cm" << endl;
      output_geometry << "Si(Li) thickness: " << zSiLi/cm << " cm" << endl;
      output_geometry << "Si(Li) frame material: " << SiLiFrameMaterial->GetName() << endl;
      output_geometry << "Si(Li) frame thickness: " << FrameZ/cm << " cm" << endl;
			output_geometry << "TOFin thickness: " << TOFinZ/cm << " cm" << endl;
			output_geometry << "TOFin length: " << TOFinL/cm << " cm" << endl;
			output_geometry << "TOFin height: " << TOFinH/cm << " cm" << endl;
			output_geometry << "TOFout thickness: " << TOFoutZ/cm << " cm" << endl;
			output_geometry << "TOFout length: " << TOFoutL/cm << " cm" << endl;
			output_geometry << "TOFout height: " << TOFoutH/cm << " cm" << endl;
			output_geometry << "TOFout angle: " << TOFoutAngle/deg << " deg" << endl;
			output_geometry << "Atmos depth: " << atmosZ/cm << " g/cm2" << endl;
		}
		
		if(global.SimulationType <= 1)
		{
			output_geometry << "primary particle area: " << global.area/m*global.area/m << " m^2" << endl;
			output_geometry << "primary particle minimum energy: " << global.Emin/MeV << " MeV" << endl;
			output_geometry << "primary particle maximum energy: " << global.Emax/MeV << " MeV" << endl;
		}
		output_geometry.close();
	}
	
	//------------------------------------------------
	// Atmosphere
	//------------------------------------------------
	
	if(atmosZ > 0.0)
	{
		solidAtmos = new G4Box("AtmosSolid",atmosL*0.5,atmosL*0.5,atmosZ*0.5);
		logicAtmos = new G4LogicalVolume(solidAtmos,atmosMaterial,"AtmosLogical",0,0,0);
		physiAtmos = new G4PVPlacement(0,
																	 G4ThreeVector(0.0,0.0,global.beamZ-scoreZ-0.5*atmosZ), // at (x,y,z)
																	 logicAtmos,    // its logical volume
																	 "Atmosphere",       // its name
																	 logicWorld,      // its mother  volume
																	 false,           // no boolean operations
																	 copyAtmosphere,								// copy number
																	 overlap);
	}
	
  //------------------------------------------------
  // scoring
  //------------------------------------------------
	
	solidScore1 = new G4Box("ScoreSolid1",scoreL*0.5,scoreL*0.5,scoreZ*0.5);
	logicScore1 = new G4LogicalVolume(solidScore1,scoreMaterial,"ScoreLogical1",0,0,0);
	physiScore1 = new G4PVPlacement(0,
																	G4ThreeVector(0.0,0.0,global.beamZ-0.5*scoreZ), // at (x,y,z)
																	logicScore1,    // its logical volume
																	"Score1",       // its name
																	logicWorld,      // its mother  volume
																	false,           // no boolean operations
																	copyScore1,								// copy number
																	overlap);
	
	if(global.SimulationType <= 1) logicScore1->SetSensitiveDetector(ScoreSD);
	
	
	if(global.SimulationType == 0)
	{
		cout << "**********" << endl;
		cout << "reference geometry is built" << endl;
		solidScore2 = new G4Box("ScoreSolid2",referenceL*0.5,referenceL*0.5,scoreZ*0.5);
		logicScore2 = new G4LogicalVolume(solidScore2,scoreMaterial,"ScoreLogical2",0,0,0);
		physiScore2 = new G4PVPlacement(0,
																		G4ThreeVector(0.0,0.0,0.0), // at (x,y,z)
																		logicScore2,    // its logical volume
																		"Score2",       // its name
																		logicWorld,      // its mother  volume
																		false,           // no boolean operations
																		copyScore2,								// copy number
																		overlap);
		
		logicScore2->SetSensitiveDetector(ScoreSD);
	}
	else
	{
		//------------------------------------------------
		// Layer
		//------------------------------------------------
		
		solidLayer = new G4Box("layer",LayerL*0.5,LayerL*0.5,LayerZ*0.5);
		logicLayer = new G4LogicalVolume(solidLayer,layerMaterial,"Layer",0,0,0);
		for(int i=0; i<NbOfLayers;i++)
		{
			physiLayer = new G4PVPlacement(0,
																		 G4ThreeVector(0.0,0.0,-(i-(NbOfLayers-1)*0.5)*(LayerSpace)), // at (x,y,z)
																		 logicLayer,    // its logical volume
																		 "Layer",       // its name
																		 logicWorld,      // its mother  volume
																		 false,           // no boolean operations
																		 copyLayer,								// copy number
																		 overlap);
			copyLayer++;
		}
		G4cout << "***** There are " << copyLayer << " layers."<< G4endl;
		if(global.SimulationType == 6) logicLayer->SetSensitiveDetector(FrameSD);
		
		//------------------------------------------------
		// outer TOF
		//------------------------------------------------
		
		G4RotationMatrix* rotX = new G4RotationMatrix();
		rotX -> rotateX(90.*deg);
		G4RotationMatrix* rotZX = new G4RotationMatrix();
		rotZX -> rotateZ(90.*deg);
		rotZX -> rotateX(90.*deg);
		
		G4RotationMatrix* rotXTOF1 = new G4RotationMatrix();
		rotXTOF1 -> rotateX(TOFoutAngle);
		G4RotationMatrix* rotZXTOF1 = new G4RotationMatrix();
		rotZXTOF1 -> rotateZ(90.*deg);
		rotZXTOF1 -> rotateX(TOFoutAngle);
		
		G4RotationMatrix* rotXTOF2 = new G4RotationMatrix();
		rotXTOF2 -> rotateX(-TOFoutAngle);
		G4RotationMatrix* rotZXTOF2 = new G4RotationMatrix();
		rotZXTOF2 -> rotateZ(90.*deg);
		rotZXTOF2 -> rotateX(-TOFoutAngle);
		
		
    // top
    solidTOFout1 = new G4Box("TOFout1",TOFoutL*0.5,TOFoutL*0.5,TOFoutZ*0.5);
    logicTOFout1 = new G4LogicalVolume(solidTOFout1,TOFframeMaterial,"TOFout1",0,0,0);
    physiTOFout1 = new G4PVPlacement(0,
                                     G4ThreeVector(0.0,0.0,TOFspace+0.5*TOFinH), // at (x,y,z)
                                     logicTOFout1,    // its logical volume
                                     "TOFout1",       // its name
                                     logicWorld,      // its mother  volume
                                     false,           // no boolean operations
                                     copyTOFout,								// copy number
                                     overlap);
    copyTOFout++;
    logicTOFout1->SetSensitiveDetector(TOFSD);
    
    //side
    if(TOFoutH > 0.) // no side if TOFoutH = 0
    {
      solidTOFout2 = new G4Box("TOFout2",TOFoutL*0.5,TOFoutH*0.5,TOFoutZ*0.5);
      logicTOFout2 = new G4LogicalVolume(solidTOFout2,TOFframeMaterial,"TOFout2",0,0,0);
      physiTOFout2 = new G4PVPlacement(rotXTOF2,
                                       G4ThreeVector(0.0,-0.5*(TOFoutL+TOFoutZ+TOFoutH*cos(TOFoutAngle)),TOFspace+0.5*TOFinH-0.5*TOFoutH*sin(TOFoutAngle)), // at (x,y,z)
                                       logicTOFout2,    // its logical volume
                                       "TOFout2",       // its name
                                       logicWorld,      // its mother  volume
                                       false,           // no boolean operations
                                       copyTOFout,								// copy number
                                       overlap);
      copyTOFout++;
      physiTOFout2 = new G4PVPlacement(rotXTOF1,
                                       G4ThreeVector(0.0,0.5*(TOFoutL+TOFoutZ+TOFoutH*cos(TOFoutAngle)),TOFspace+0.5*TOFinH-0.5*TOFoutH*sin(TOFoutAngle)), // at (x,y,z)
                                       logicTOFout2,    // its logical volume
                                       "TOFout2",       // its name
                                       logicWorld,      // its mother  volume
                                       false,           // no boolean operations
                                       copyTOFout,								// copy number
                                       overlap);
      copyTOFout++;
      physiTOFout2 = new G4PVPlacement(rotZXTOF2,
                                       G4ThreeVector(-0.5*(TOFoutL+TOFoutZ+TOFoutH*cos(TOFoutAngle)),0.0,TOFspace+0.5*TOFinH-0.5*TOFoutH*sin(TOFoutAngle)), // at (x,y,z)
                                       logicTOFout2,    // its logical volume
                                       "TOFout2",       // its name
                                       logicWorld,      // its mother  volume
                                       false,           // no boolean operations
                                       copyTOFout,								// copy number
                                       overlap);
      copyTOFout++;
      physiTOFout2 = new G4PVPlacement(rotZXTOF1,
                                       G4ThreeVector(0.5*(TOFoutL+TOFoutZ+TOFoutH*cos(TOFoutAngle)),0.0,TOFspace+0.5*TOFinH-0.5*TOFoutH*sin(TOFoutAngle)), // at (x,y,z)
                                       logicTOFout2,    // its logical volume
                                       "TOFout2",       // its name
                                       logicWorld,      // its mother  volume
                                       false,           // no boolean operations
                                       copyTOFout,								// copy number
                                       overlap);
      copyTOFout++;
      logicTOFout2->SetSensitiveDetector(TOFSD);
    }
		
		//------------------------------------------------
		// inner TOF
		//------------------------------------------------
		
		// top and bottom
		solidTOFin1 = new G4Box("TOFin1",TOFinL*0.5,TOFinL*0.5,TOFinZ*0.5);
		logicTOFin1 = new G4LogicalVolume(solidTOFin1,TOFframeMaterial,"TOFin1",0,0,0);
		
		for(int i=0; i<2;i++)
		{
			physiTOFin1 = new G4PVPlacement(0,
																			G4ThreeVector(0.0,0.0,-(i-0.5)*TOFinH), // at (x,y,z)
																			logicTOFin1,    // its logical volume
																			"TOFin",       // its name
																			logicWorld,      // its mother  volume
																			false,           // no boolean operations
																			copyTOFin,								// copy number
																			overlap);
			copyTOFin++;
		}
		// side
		solidTOFin2 = new G4Box("TOFin2",TOFinL*0.5,TOFinH*0.5,TOFinZ*0.5);
		logicTOFin2 = new G4LogicalVolume(solidTOFin2,TOFframeMaterial,"TOFin2",0,0,0);
		
		for(int i=0; i<2;i++)
		{
			if(global.SimulationType == 6) break;
      physiTOFin2 = new G4PVPlacement(rotX,
																			G4ThreeVector(0.0,-(i-0.5)*(TOFinL+TOFinZ),0.0), // at (x,y,z)
																			logicTOFin2,    // its logical volume
																			"TOFin2",       // its name
																			logicWorld,      // its mother  volume
																			false,           // no boolean operations
																			copyTOFin,								// copy number
																			overlap);
			copyTOFin++;
		}
		for(int i=0; i<2;i++)
		{
			if(global.SimulationType == 6) break;
      physiTOFin2 = new G4PVPlacement(rotZX,
																			G4ThreeVector(-(i-0.5)*(TOFinL+TOFinZ),0.0,0.0), // at (x,y,z)
																			logicTOFin2,    // its logical volume
																			"TOFin2",       // its name
																			logicWorld,      // its mother  volume
																			false,           // no boolean operations
																			copyTOFin,								// copy number
																			overlap);
			copyTOFin++;
		}
		
		logicTOFin1->SetSensitiveDetector(TOFSD);
		logicTOFin2->SetSensitiveDetector(TOFSD);
		
        
    //------------------------------------------------
    // Si(Li), Frame
    //------------------------------------------------
    
    int NbFrame = 0, NbChannel = 0;
    
    // Frame
    solidBox = new G4Box("box",FrameL*0.5,FrameL*0.5,FrameZ*0.5);
    solidCylinder = new G4Tubs("Cylinder",0.0,rSiLi,FrameZ*0.51,0*deg,360*deg);
    solidFrame = new G4SubtractionSolid("Frame", solidBox, solidCylinder, 0, G4ThreeVector(0.,0.,0.));
    logicFrame = new G4LogicalVolume(solidFrame,SiLiFrameMaterial,"Frame",0,0,0);
    
    // Si(Li)
    G4Tubs* solidSiLiWafer = new G4Tubs("SiLi",0.0,rSiLi,zSiLi*0.5,0*deg,360*deg);
    if(NbOfStrips == 8)
    {
      G4double StripWidth1 = rSiLi*1.717/4.701; // Based on SEMIKON's design
      G4double StripWidth3 = rSiLi*1.085/4.701;
      G4double StripWidth2 = rSiLi*0.97/4.701;
      G4double StripWidth4 = rSiLi*0.929/4.701;
      
      G4double StripY1 = StripWidth4+StripWidth3+StripWidth2+StripWidth1*0.5;
      G4double StripY2 = StripWidth4+StripWidth3+StripWidth2*0.5;
      G4double StripY3 = StripWidth4+StripWidth3*0.5;
      G4double StripY4 = StripWidth4*0.5;
      
      G4Box* solidStrip1 = new G4Box("solidStrip1",rSiLi,StripWidth1*0.5,zSiLi*0.5);
      G4Box* solidStrip2 = new G4Box("solidStrip2",rSiLi,StripWidth2*0.5,zSiLi*0.5);
      G4Box* solidStrip3 = new G4Box("solidStrip3",rSiLi,StripWidth3*0.5,zSiLi*0.5);
      G4Box* solidStrip4 = new G4Box("solidStrip4",rSiLi,StripWidth4*0.5,zSiLi*0.5);
      
      solidSiLi[0] = new G4IntersectionSolid("Strip1", solidSiLiWafer, solidStrip1, 0, G4ThreeVector(0.0,StripY1,0.0));
      solidSiLi[1] = new G4IntersectionSolid("Strip2", solidSiLiWafer, solidStrip2, 0, G4ThreeVector(0.0,StripY2,0.0));
      solidSiLi[2] = new G4IntersectionSolid("Strip3", solidSiLiWafer, solidStrip3, 0, G4ThreeVector(0.0,StripY3,0.0));
      solidSiLi[3] = new G4IntersectionSolid("Strip4", solidSiLiWafer, solidStrip4, 0, G4ThreeVector(0.0,StripY4,0.0));
      solidSiLi[4] = new G4IntersectionSolid("Strip5", solidSiLiWafer, solidStrip4, 0, G4ThreeVector(0.0,-StripY4,0.0));
      solidSiLi[5] = new G4IntersectionSolid("Strip6", solidSiLiWafer, solidStrip3, 0, G4ThreeVector(0.0,-StripY3,0.0));
      solidSiLi[6] = new G4IntersectionSolid("Strip7", solidSiLiWafer, solidStrip2, 0, G4ThreeVector(0.0,-StripY2,0.0));
      solidSiLi[7] = new G4IntersectionSolid("Strip8", solidSiLiWafer, solidStrip1, 0, G4ThreeVector(0.0,-StripY1,0.0));
    }
    if(NbOfStrips == 4)
    {
      G4double StripWidth1 = rSiLi*(1.717+1.085)/4.701; // Based on SEMIKON's design
      G4double StripWidth2 = rSiLi*(0.97+0.929)/4.701;
      
      G4double StripY1 = StripWidth2+StripWidth1*0.5;
      G4double StripY2 = StripWidth2*0.5;
      
      G4Box* solidStrip1 = new G4Box("solidStrip1",rSiLi,StripWidth1*0.5,zSiLi*0.5);
      G4Box* solidStrip2 = new G4Box("solidStrip2",rSiLi,StripWidth2*0.5,zSiLi*0.5);
      
      solidSiLi[0] = new G4IntersectionSolid("Strip1", solidSiLiWafer, solidStrip1, 0, G4ThreeVector(0.0,StripY1,0.0));
      solidSiLi[1] = new G4IntersectionSolid("Strip2", solidSiLiWafer, solidStrip2, 0, G4ThreeVector(0.0,StripY2,0.0));
      solidSiLi[2] = new G4IntersectionSolid("Strip3", solidSiLiWafer, solidStrip2, 0, G4ThreeVector(0.0,-StripY2,0.0));
      solidSiLi[3] = new G4IntersectionSolid("Strip4", solidSiLiWafer, solidStrip1, 0, G4ThreeVector(0.0,-StripY1,0.0));
    }
    else if(NbOfStrips == 2)
    {
      G4double StripWidth1 = rSiLi;
      G4double StripY1 = StripWidth1*0.5;
      G4Box* solidStrip1 = new G4Box("solidStrip1",rSiLi,StripWidth1*0.5,zSiLi*0.5);
      
      solidSiLi[0] = new G4IntersectionSolid("Strip1", solidSiLiWafer, solidStrip1, 0, G4ThreeVector(0.0,StripY1,0.0));
      solidSiLi[1] = new G4IntersectionSolid("Strip2", solidSiLiWafer, solidStrip1, 0, G4ThreeVector(0.0,-StripY1,0.0));
    }
    else if(NbOfStrips == 1)
    {
      G4double StripWidth1 = rSiLi*2.0;
      G4double StripY1 = 0.0;
      G4Box* solidStrip1 = new G4Box("solidStrip1",rSiLi,StripWidth1*0.5,zSiLi*0.5);
      
      solidSiLi[0] = new G4IntersectionSolid("Strip1", solidSiLiWafer, solidStrip1, 0, G4ThreeVector(0.0,StripY1,0.0));
    }
    for(int i=0; i<NbOfStrips; i++)
    {
      logicSiLi[i] = new G4LogicalVolume(solidSiLi[i],SiLiMaterial,"SiLiLogical",0,0,0);
    }
    
    for(int i=0; i<NbOfSiLi; i++)
    {
      if(global.SimulationType == 6) break;
      /*
       if(global.DetectorVisualization == 1)
       {
       if(i > copyX-1) break; // place only one raw to visualize
       }
       */
      overlap = global.CheckOverlap;
      row = i/copyY;
      column = i%copyY;
      xFrame[i] = -(row-(copyX-1)*0.5)*FrameL;
      yFrame[i] = -(column-(copyY-1)*0.5)*FrameL;
      zFrame[i] = 0.0*cm;
      
      // Si(Li) and Frame for 2 inch and 4 inch
      // frame
      physiFrame = new G4PVPlacement(0,
                                     G4ThreeVector(xFrame[i],yFrame[i],zFrame[i]),
                                     logicFrame,    // its logical volume
                                     "Frame",       // its name
                                     logicLayer,      // its mother  volume
                                     false,           // no boolean operations
                                     copyFrame,								// copy number
                                     overlap);
      copyFrame++;
      NbFrame++;
      
      // Si(Li)
      for(int ii=0; ii<NbOfStrips; ii++)
      {
        overlap = global.CheckOverlap;
        physiSiLi[ii] = new G4PVPlacement(0,
                                          G4ThreeVector(xFrame[i],yFrame[i],zFrame[i]),
                                          logicSiLi[ii],			// its logical volume
                                          "Si(Li)",						// its name
                                          logicLayer,					// its mother  volume
                                          false,							// no boolean operations
                                          copySiLi,						// copy number
                                          overlap);
        copySiLi++;
        NbChannel++;
      }
    }
    
    // sensitivie detector
    logicFrame->SetSensitiveDetector(FrameSD);
    
    for(int ii=0; ii<NbOfStrips; ii++)
    {
      logicSiLi[ii]->SetSensitiveDetector(SiliconSD);
    }
    
    G4cout << "***** there are "<< NbFrame << " Si(Li) wafers/frames per layer."<< G4endl;
    G4cout << "***** there are " << NbChannel << " Si(Li) channels per layer."<< G4endl;
    /*
    //------------------------------------------------
    // outlet: PMT, light guide and cables
    //------------------------------------------------
    
    solidOutlet = new G4Box("Outlet",OutletT*0.5,OutletL*0.5,TOFinH*0.5);
    logicOutlet = new G4LogicalVolume(solidOutlet,outletMaterial,"Outlet",0,0,0);
    
    G4RotationMatrix* rotZ45 = new G4RotationMatrix();
    rotZ45 -> rotateZ(45.*deg);
    G4RotationMatrix* rotZ135 = new G4RotationMatrix();
    rotZ135 -> rotateZ(135.*deg);
    
    if(global.SimulationType != 6)
    {
      // dead area for outlets: PMT, light guide and cable
      physiOutlet = new G4PVPlacement(rotZ45,
                                      G4ThreeVector((TOFinZ+TOFinL+OutletL/sqrt(2.0))*0.5,(TOFinZ+TOFinL+OutletL/sqrt(2.0))*0.5,0.0), // at (x,y,z)
                                      logicOutlet,    // its logical volume
                                      "Outlet",       // its name
                                      logicWorld,      // its mother  volume
                                      false,           // no boolean operations
                                      copyOutlet,								// copy number
                                      overlap);
      
      physiOutlet = new G4PVPlacement(rotZ135,
                                      G4ThreeVector(-(TOFinZ+TOFinL+OutletL/sqrt(2.0))*0.5,(TOFinZ+TOFinL+OutletL/sqrt(2.0))*0.5,0.0), // at (x,y,z)
                                      logicOutlet,    // its logical volume
                                      "Outlet",       // its name
                                      logicWorld,      // its mother  volume
                                      false,           // no boolean operations
                                      copyOutlet,								// copy number
                                      overlap);
      
      physiOutlet = new G4PVPlacement(rotZ135,
                                      G4ThreeVector((TOFinZ+TOFinL+OutletL/sqrt(2.0))*0.5,-(TOFinZ+TOFinL+OutletL/sqrt(2.0))*0.5,0.0), // at (x,y,z)
                                      logicOutlet,    // its logical volume
                                      "Outlet",       // its name
                                      logicWorld,      // its mother  volume
                                      false,           // no boolean operations
                                      copyOutlet,								// copy number
                                      overlap);
      
      physiOutlet = new G4PVPlacement(rotZ45,
                                      G4ThreeVector(-(TOFinZ+TOFinL+OutletL/sqrt(2.0))*0.5,-(TOFinZ+TOFinL+OutletL/sqrt(2.0))*0.5,0.0), // at (x,y,z)
                                      logicOutlet,    // its logical volume
                                      "Outlet",       // its name
                                      logicWorld,      // its mother  volume
                                      false,           // no boolean operations
                                      copyOutlet,								// copy number
                                      overlap);
    }
    
    // sensitivie detector
    logicOutlet->SetSensitiveDetector(FrameSD);
    */
    /*
     solidSiLiLayer = new G4Box("SiLiLayer",LayerL*0.5,LayerL*0.5,zSiLi*0.5);
     logicSiLiLayer = new G4LogicalVolume(solidSiLiLayer,SiLiMaterial,"Layer",0,0,0);
     physiSiLiLayer = new G4PVPlacement(0,
     G4ThreeVector(0.0,0.0,0.0),  // at (x,y,z)
     logicSiLiLayer,              // its logical volume
     "SiLiLayer",                // its name
     logicLayer,                  // its mother  volume
     false,                       // no boolean operations
     copySiLi,                    // copy number
     overlap);
     
     // sensitivie detector
     logicSiLiLayer->SetSensitiveDetector(SiliconSD);
     */
    
    /*
    //------------------------------------------------
    // cooling tube
    //------------------------------------------------
    
    int NbPipe = 0;
    if(cType == 0) // new cooling design
    {
      lPipe = 0.5*(TOFoutL-TOFinL-TOFinZ)*sqrt(2.0)-OutletL-rPipe;
      if(lPipe < 0.) lPipe = 1.0*cm;
      solidPipe = new G4Tubs("PipeSolid",0.0,rPipe,lPipe*0.5,0*deg,360*deg);
      logicPipe = new G4LogicalVolume(solidPipe,pipeMaterial,"PipeLogical",0,0,0);
      
      G4RotationMatrix* rotX90Z45 = new G4RotationMatrix();
      rotX90Z45 -> rotateX(90.*deg);
      rotX90Z45 -> rotateY(-45.*deg);
      G4RotationMatrix* rotX90Z135 = new G4RotationMatrix();
      rotX90Z135 -> rotateX(90.*deg);
      rotX90Z135 -> rotateY(-135.*deg);
      
      for(int i=0; i<NbOfLayers;i++)
      {
        if(global.SimulationType == 6) break;
        physiPipe = new G4PVPlacement(rotX90Z45,
                                      G4ThreeVector(0.5*(TOFoutL-lPipe/sqrt(2.0))-rPipe/sqrt(2.0),0.5*(TOFoutL-lPipe/sqrt(2.0))-rPipe/sqrt(2.0),-(i-(NbOfLayers-1)*0.5)*(LayerSpace)), // at (x,y,z)
                                      logicPipe,    // its logical volume
                                      "Pipe",       // its name
                                      logicWorld,      // its mother  volume
                                      false,           // no boolean operations
                                      copyPipe,								// copy number
                                      overlap);
        NbPipe++;
        
        physiPipe = new G4PVPlacement(rotX90Z135,
                                      G4ThreeVector((0.5*(TOFoutL-lPipe/sqrt(2.0))-rPipe/sqrt(2.0)),-(0.5*(TOFoutL-lPipe/sqrt(2.0))-rPipe/sqrt(2.0)),-(i-(NbOfLayers-1)*0.5)*(LayerSpace)), // at (x,y,z)
                                      logicPipe,    // its logical volume
                                      "Pipe",       // its name
                                      logicWorld,      // its mother  volume
                                      false,           // no boolean operations
                                      copyPipe,								// copy number
                                      overlap);
        NbPipe++;
      }
      // sensitivie detector
      logicPipe->SetSensitiveDetector(FrameSD);
      G4cout << "***** there are "<< NbPipe << " pipes."<< G4endl;
    }
    
    if(cType == 1) // old cooling design
    {
      NbOfPipe = NbOfSiLi/4; // per layer, old design
      rPipe = 0.625*cm;
      lPipe = (LayerSpace-FrameZ)*0.5;
      solidPipe = new G4Tubs("PipeSolid",0.0,rPipe,lPipe*0.5,0*deg,360*deg);
      logicPipe = new G4LogicalVolume(solidPipe,pipeMaterial,"PipeLogical",0,0,0);
      
      for(int i=0; i<NbOfPipe; i++)
      {
        overlap = global.CheckOverlap;
        if(global.SimulationType == 6) break;
        row = i/(copyY/2);
        column = i%(copyY/2);
        pipeX = -(row-((copyX/2)-1)*0.5)*FrameL*2.0;
        pipeY = -(column-((copyY/2)-1)*0.5)*FrameL*2.0;
        pipeZ = 0.5*(LayerSpace - lPipe);
        
        physiPipe = new G4PVPlacement(0,
                                      G4ThreeVector(pipeX,pipeY,pipeZ), // at (x,y,z)
                                      logicPipe,			// its logical volume
                                      "Pipe",					// its name
                                      logicLayer,      // its mother  volume
                                      false,           // no boolean operations
                                      copyPipe,								// copy number
                                      overlap);
        
        physiPipe = new G4PVPlacement(0,
                                      G4ThreeVector(pipeX,pipeY,-pipeZ), // at (x,y,z)
                                      logicPipe,			// its logical volume
                                      "Pipe",					// its name
                                      logicLayer,      // its mother  volume
                                      false,           // no boolean operations
                                      copyPipe,								// copy number
                                      overlap);
        NbPipe++;
      }
      
      // sensitivie detector
      logicPipe->SetSensitiveDetector(FrameSD);
      G4cout << "there are "<< NbPipe << " pipes per layer."<< G4endl;
    }
    */
        ///////////////////////////////////////////////////////////
    
    
        //--------- Visualization attributes -------------------------------
    
        whiteVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0,0.2));
        redVisAtt= new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.2));
        greenVisAtt= new G4VisAttributes(G4Colour(0.0,1.0,0.0,0.2));
        blueVisAtt= new G4VisAttributes(G4Colour(0.0,0.0,1.0,0.2));
        logicWorld->SetVisAttributes(whiteVisAtt);
        for(int i=0; i<NbOfLayers;i++) logicLayer->SetVisAttributes(whiteVisAtt);
        if(logicTOFin1) logicTOFin1->SetVisAttributes(blueVisAtt);
        if(logicTOFin2) logicTOFin2->SetVisAttributes(blueVisAtt);
        if(logicTOFout1) logicTOFout1->SetVisAttributes(greenVisAtt);
        if(logicTOFout2) logicTOFout2->SetVisAttributes(greenVisAtt);
        for(int i=0; i<NbOfStrips;i++) if(logicSiLi[i]) logicSiLi[i]->SetVisAttributes(redVisAtt);
        if(logicPipe) logicPipe->SetVisAttributes(redVisAtt);
        if(global.SimulationType == 6) logicLayer->SetVisAttributes(redVisAtt);
    }
	return physiWorld;
}

void GAPSDetectorConstruction::SetNbOfLayers(G4int NewValue)
{
	NbOfLayers = NewValue;
}
void GAPSDetectorConstruction::SetLayerSpace(G4double NewValue)
{
	LayerSpace = NewValue;
}
void GAPSDetectorConstruction::SetLayer_L(G4double NewValue)
{
	LayerL = NewValue;
}
void GAPSDetectorConstruction::SetSiLi_Z(G4double NewValue)
{
	zSiLi = NewValue;
}
void GAPSDetectorConstruction::SetFrame_Z(G4double NewValue)
{
	FrameZ = NewValue;
}
void GAPSDetectorConstruction::SetTOFout_Z(G4double NewValue)
{
	TOFoutZ = NewValue;
}
void GAPSDetectorConstruction::SetTOFout_L(G4double NewValue)
{
	TOFoutL = NewValue;
}
void GAPSDetectorConstruction::SetTOFout_H(G4double NewValue)
{
	TOFoutH = NewValue;
}
void GAPSDetectorConstruction::SetTOFout_Angle(G4double NewValue)
{
	TOFoutAngle = NewValue;
}
void GAPSDetectorConstruction::SetTOFin_Z(G4double NewValue)
{
	TOFinZ = NewValue;
}
void GAPSDetectorConstruction::SetAtmos_Z(G4double NewValue)
{
	atmosZ = NewValue;
}

void GAPSDetectorConstruction::UpdateGeometry()
{
	// Cleanup old geometry
	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();
	
	G4RunManager::GetRunManager()->DefineWorldVolume(ConstructGAPSDetector());
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

