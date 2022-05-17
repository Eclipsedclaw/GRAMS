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

#include "GRAMSDetectorConstruction.hh"
#include "GRAMSDetectorMessenger.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4EllipticalTube.hh"
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
#include "G4PVReplica.hh"
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
 
 copyTPC = 0;
 copyCell = 0;
 copyTOFout = -10000;
 copyTOFin = -11000;
 copyScore1 = -12000;
 copyScore2 = -12001; // reference plane
 copyAtmosphere = -12002;
 copyWorld = -1;
 
*/


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GRAMSDetectorConstruction::GRAMSDetectorConstruction()
:solidWorld(0),logicWorld(0),physiWorld(0)
{
  DefineMaterials();
	detectorMessenger = new GRAMSDetectorMessenger(this);
	
	// fixed value
	atmosL = 50.0*m;
	scoreL = atmosL;
	scoreZ = 0.001*cm;
	TOFspaceL = 100.0*cm;
  TOFspaceH = 150.0*cm;
  space = 5.0*cm;
	
	// default parameter
	TPCL = 100.0*cm;
  LArTPCZ = 20.0*cm;
  LXeTPCZ = 20.0*cm;
  TPCZ = LArTPCZ+LXeTPCZ;
  TPCspace = 4.0*cm;
	TOFinZ = 0.5*cm;
	TOFoutZ = 0.5*cm;
	TOFoutL = 2.0*TOFspaceL+TOFinL;
	TOFoutH = TOFspaceH+0.5*TOFinH;
	TOFoutAngle = 90.0*deg;
  GondolaAngle = 0.0*deg;
	atmosZ = 3.9*cm;
  ChamberT = 1.0*cm;
  
  NbOfLayer = 10;
  copyL = 500;
  copyLArZ = 1;
  copyLXeZ = 1;
	
	extern global_struct global;
	sprintf(global.outdir, "tmp");
	sprintf(global.outfile, "test");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GRAMSDetectorConstruction::~GRAMSDetectorConstruction()
{
  delete detectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GRAMSDetectorConstruction::DefineMaterials()
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
	
  // Liquid Argone
  LAr = new G4Material("LAr",z=18.,a=39.95*g/mole,density=1.4*g/cm3);
  
  // Liquid Xenon
  LXe = new G4Material("LXe",z=54.,a=131.29*g/mole,density=3.020*g/cm3);
  
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

G4VPhysicalVolume* GRAMSDetectorConstruction::Construct()
{
	return ConstructGRAMSDetector();
}
G4VPhysicalVolume* GRAMSDetectorConstruction::ConstructGRAMSDetector()
{
	extern global_struct global;
	overlap = global.CheckOverlap;
  copyL = global.NbCellL;
  copyLArZ = global.NbLArCellZ;
  copyLXeZ = global.NbLXeCellZ;
  NbOfLArLayer = global.NbLArTPC;
  NbOfLXeLayer = global.NbLXeTPC;
  NbOfLayer = NbOfLArLayer+NbOfLXeLayer;
	// TPCZ = LArTPCZ+LXeTPCZ;
  TPCZ = (NbOfLArLayer-1)*TPCspace+LArTPCZ;
	//------------------------------------------------
	// parameter setup
	//------------------------------------------------
		
  CellL = TPCL/copyL;
  if(copyLArZ == 0) CellLArZ = 0;
  else CellLArZ = LArTPCZ/copyLArZ;
  if(copyLXeZ == 0) CellLXeZ = 0;
  else CellLXeZ = LXeTPCZ/copyLXeZ;
  TOFinL = TPCL+space*2.;
	// TOFinH = TPCZ+TPCspace+space*2.0+TOFinZ;
  TOFinH = (NbOfLArLayer-1)*TPCspace+LArTPCZ+ChamberT+space*2.0;
  TOFoutH = TOFspaceH+TOFinH+space;
  TOFoutL = 2.0*TOFspaceL+TOFinL;
	G4double referenceL = TOFinL;
  
	//------------------------------------------------
	// Sensitive detectors
	//------------------------------------------------
	
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
	if(!ScoreSD)
	{
		ScoreSD = new GRAMSDetectorSD("ScoreSD");
		SDman->AddNewDetector(ScoreSD);
	}
	if(!TOFSD)
	{
		TOFSD = new GRAMSDetectorSD("TOFSD");
		SDman->AddNewDetector(TOFSD);
	}
	if(!TPCSD)
	{
		TPCSD = new GRAMSDetectorSD("TPCSD");
		SDman->AddNewDetector(TPCSD);
	}
	
	//------------------------------------------------
	// Copy Number
	//------------------------------------------------
	
	copyTPC = 0;
  copyCell = 0;
	copyTOFout = -10000;
	copyTOFin = -11000;
	copyScore1 = -12000;
	copyScore2 = -12001; // reference plane
	copyAtmosphere = -12002;
	copyWorld = -1;
	
	//--------- Definitions of Solids, Logical Volumes, Physical Volumes ---------
 
  //------------------------------------------------
  // Material
  //------------------------------------------------

  worldMaterial = FlightAir;
  atmosMaterial = CompressedAir;
  scoreMaterial = Vacuum;
  ChamberMaterial = Fe;
  TOFMaterial = Plastic;
  TPCMaterial = LAr;
 
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
																 overlap);
  
	//------------------------------------------------
	// global.beamZ = TOFoutL*2.0;
	// global.beamZ = TOFspaceH+0.5*TOFinH+TOFoutZ+atmosZ+scoreZ;
	//	global.beamZ = 200.0*cm+0.5*TOFoutZ+atmosZ+scoreZ;
  global.beamZ = TOFspaceH+0.5*TOFinH+TOFoutZ+space+atmosZ+scoreZ;
	cout << "*******************" << endl;
	
	if(global.DetectorType == 0) cout << "Reference TPC length: " << referenceL/cm << " cm" << endl;
	if(global.DetectorType > 0)
	{
		cout << "Gondola  angle: " << GondolaAngle/deg << " deg" << endl;
    cout << "Number of TPCs: " << NbOfLayer << endl;
		cout << "TPC length: " << TPCL/cm << " cm" << endl;
    cout << "TPC thickness: " << TPCZ/cm << " cm" << endl;
		cout << "TOFin thickness: " << TOFinZ/cm << " cm" << endl;
		cout << "TOFin length: " << TOFinL/cm << " cm" << endl;
		cout << "TOFin height: " << TOFinH/cm << " cm" << endl;
		cout << "TOFout thickness: " << TOFoutZ/cm << " cm" << endl;
		cout << "TOFout length: " << TOFoutL/cm << " cm" << endl;
		cout << "TOFout height: " << TOFoutH/cm << " cm" << endl;
		cout << "TOFout angle: " << TOFoutAngle/deg << " deg" << endl;
		cout << "Atmos depth: " << atmosZ/cm << " g/cm2" << endl;
	}
	
	if(global.ParticleSource == 0)
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
		
		if(global.DetectorType == 0) output_geometry << "Reference TPC length: " << referenceL/cm << " cm" << endl;
		if(global.DetectorType > 0)
		{
			output_geometry << "Gondola angle: " << GondolaAngle/deg << " deg" << endl;
      output_geometry << "Number of TPCs: " << NbOfLayer << endl;
			output_geometry << "TPC length: " << TPCL/cm << " cm" << endl;
      output_geometry << "TPC thickness: " << TPCZ/cm << " cm" << endl;
 			output_geometry << "TOFin thickness: " << TOFinZ/cm << " cm" << endl;
			output_geometry << "TOFin length: " << TOFinL/cm << " cm" << endl;
			output_geometry << "TOFin height: " << TOFinH/cm << " cm" << endl;
			output_geometry << "TOFout thickness: " << TOFoutZ/cm << " cm" << endl;
			output_geometry << "TOFout length: " << TOFoutL/cm << " cm" << endl;
			output_geometry << "TOFout height: " << TOFoutH/cm << " cm" << endl;
			output_geometry << "TOFout angle: " << TOFoutAngle/deg << " deg" << endl;
			output_geometry << "Atmos depth: " << atmosZ/cm << " g/cm2" << endl;
		}
		
		if(global.DetectorType <= 1)
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
		if(global.DetectorType != 2)
    {
      physiAtmos = new G4PVPlacement(0,
																	 G4ThreeVector(0.0,0.0,global.beamZ-scoreZ-0.5*atmosZ), // at (x,y,z)
																	 logicAtmos,    // its logical volume
																	 "Atmosphere",       // its name
																	 logicWorld,      // its mother  volume
																	 false,           // no boolean operations
																	 copyAtmosphere,								// copy number
																	 overlap);
    }
	}
	
  //------------------------------------------------
  // scoring
  //------------------------------------------------
	
	solidScore1 = new G4Box("ScoreSolid1",scoreL*0.5,scoreL*0.5,scoreZ*0.5);
	logicScore1 = new G4LogicalVolume(solidScore1,scoreMaterial,"ScoreLogical1",0,0,0);
  if(global.DetectorType != 2)
  {
    physiScore1 = new G4PVPlacement(0,
																	G4ThreeVector(0.0,0.0,global.beamZ-0.5*scoreZ), // at (x,y,z)
																	logicScore1,    // its logical volume
																	"Score1",       // its name
																	logicWorld,      // its mother  volume
																	false,           // no boolean operations
																	copyScore1,								// copy number
																	overlap);
	
    if(global.DetectorType <= 1) logicScore1->SetSensitiveDetector(ScoreSD);
  }
	
	if(global.DetectorType == 0)
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
    // Gondola
    //------------------------------------------------
    
    G4RotationMatrix* rotY = new G4RotationMatrix();
    rotY -> rotateY(GondolaAngle);
    // cubic with TOFoutL
    solidGondola= new G4Box("Gondola",TOFoutL*0.5+TOFoutZ+space,TOFoutL*0.5+TOFoutZ+space,TOFspaceH+0.5*(TOFinH+TOFoutZ)+space);
    logicGondola= new G4LogicalVolume(solidGondola, worldMaterial, "Gondola", 0, 0, 0);
    physiGondola = new G4PVPlacement(rotY,
                                     G4ThreeVector(),   // at (0,0,0)
                                     logicGondola,      // its logical volume
                                     "Gondola",         // its name
                                     logicWorld,        // its mother  volume
                                     false,             // no boolean operations
                                     copyWorld,       // copy number
                                     overlap);

    
    //------------------------------------------------
    // Chamber
    //------------------------------------------------
    
    solidChamber= new G4Box("Chamber",TPCL*0.5+ChamberT,TPCL*0.5+ChamberT,(NbOfLArLayer-1)*TPCspace*0.5+LArTPCZ*0.5+ChamberT);
    logicChamber= new G4LogicalVolume(solidChamber, ChamberMaterial, "Chamber", 0, 0, 0);
    physiChamber = new G4PVPlacement(0,
                                     G4ThreeVector(),   // at (0,0,0)
                                     logicChamber,      // its logical volume
                                     "Chamber",         // its name
                                     logicGondola,        // its mother  volume
                                     false,             // no boolean operations
                                     copyWorld,       // copy number
                                     overlap);
    
    logicChamber->SetSensitiveDetector(TPCSD);
    
    solidChamberInside= new G4Box("ChamberInside",TPCL*0.5,TPCL*0.5,(NbOfLArLayer-1)*TPCspace*0.5+LArTPCZ*0.5);
    logicChamberInside= new G4LogicalVolume(solidChamberInside, Vacuum, "ChamberInside", 0, 0, 0);
    physiChamberInside = new G4PVPlacement(0,
                                     G4ThreeVector(),   // at (0,0,0)
                                     logicChamberInside,      // its logical volume
                                     "ChamberInside",         // its name
                                     logicChamber,        // its mother  volume
                                     false,             // no boolean operations
                                     copyWorld,       // copy number
                                     overlap);
    
    //------------------------------------------------
		// TPC
		//------------------------------------------------
    /*
    // LXe
    if(LXeTPCZ > 0.0 && NbOfLXeLayer > 0)
    {
      solidLXeTPC = new G4Box("LXeTPC",TPCL*0.5,TPCL*0.5,LXeTPCZ*0.5);
      logicLXeTPC = new G4LogicalVolume(solidLXeTPC,LXe,"LXeTPC",0,0,0);
      physiLXeTPC = new G4PVPlacement(0,
                                      G4ThreeVector(0.0,0.0,-0.5*(LArTPCZ+TPCspace)), // at (x,y,z)
                                      logicLXeTPC,    // its logical volume
                                      "LXeTPC",       // its name
                                      logicChamberInside,      // its mother  volume
                                      false,           // no boolean operations
                                      copyTPC,                // copy number
                                      overlap);
      copyTPC++;
      //logicLXeTPC->SetSensitiveDetector(TPCSD);
      
      // layer
      solidLXeLayer = new G4Box("LXeLayer",TPCL*0.5,TPCL*0.5,CellLXeZ*0.5);
      logicLXeLayer = new G4LogicalVolume(solidLXeLayer,LXe,"LXeLayer",0,0,0);
      physiLXeLayer = new G4PVReplica("LXeLayer",logicLXeLayer,logicLXeTPC,kZAxis,copyLXeZ,CellLXeZ);
      // column
      solidLXeColumn = new G4Box("LXeColumn",TPCL*0.5,CellL*0.5,CellLXeZ*0.5);
      logicLXeColumn = new G4LogicalVolume(solidLXeColumn,LXe,"LXeColumn",0,0,0);
      physiLXeColumn = new G4PVReplica("LXeColumn",logicLXeColumn,logicLXeLayer,kYAxis,copyL,CellL);
      // cell
      solidLXeCell = new G4Box("LXeCell",CellL*0.5,CellL*0.5,CellLXeZ*0.5);
      logicLXeCell = new G4LogicalVolume(solidLXeCell,LXe,"LXeCell",0,0,0);
      physiLXeCell = new G4PVReplica("LXeCell",logicLXeCell,logicLXeColumn,kXAxis,copyL,CellL);
      
      logicLXeCell->SetSensitiveDetector(TPCSD);
    }
    */
    // LAr
    if(LArTPCZ > 0.0 && NbOfLArLayer > 0)
    {
      G4EllipticalTube *solidLArTPC = new G4EllipticalTube("LArTPC",TPCL*0.5,TPCL*0.5,LArTPCZ*0.5);
      logicLArTPC = new G4LogicalVolume(solidLArTPC,LAr,"LArTPC",0,0,0);
      for(int i=0; i<NbOfLArLayer;i++)
      {
        physiLArTPC = new G4PVPlacement(0,
                                      G4ThreeVector(0.0,0.0,(i-(NbOfLArLayer-1)*0.5)*TPCspace), // at (x,y,z)
                                      logicLArTPC,    // its logical volume
                                      "LArTPC",       // its name
                                      logicChamberInside,      // its mother  volume
                                      false,           // no boolean operations
                                      copyTPC,                // copy number
                                      overlap);
        copyTPC++;
        logicLArTPC->SetSensitiveDetector(TPCSD);
      }
      
      // layer
      //G4EllipticalTube *solidLArLayer = new G4EllipticalTube("LArLayer",TPCL*0.5,TPCL*0.5,CellLArZ*0.5);
      //logicLArLayer = new G4LogicalVolume(solidLArLayer,LAr,"LArLayer",0,0,0);
      //physiLArLayer = new G4PVReplica("LArLayer",logicLArLayer,logicLArTPC,kZAxis,copyLArZ,CellLArZ);
      // column
      //G4EllipticalTube *solidLArColumn = new G4EllipticalTube("LArColumn",TPCL*0.5,CellL*0.5,CellLArZ*0.5);
      //logicLArColumn = new G4LogicalVolume(solidLArColumn,LAr,"LArColumn",0,0,0);
      //physiLArColumn = new G4PVReplica("LArColumn",logicLArColumn,logicLArLayer,kYAxis,copyL,CellL);
      // cell
      // solidLArCell = new G4Box("LArCell",CellL*0.5,CellL*0.5,CellLArZ*0.5);
      //logicLArCell = new G4LogicalVolume(solidLArCell,LAr,"LArCell",0,0,0);
      //physiLArCell = new G4PVReplica("LArCell",logicLArCell,logicLArColumn,kXAxis,copyL,CellL);
      
      //logicLArCell->SetSensitiveDetector(TPCSD);
      
    }
   
		G4cout << "***** There are " << copyTPC << " TPC."<< G4endl;
    //G4cout << "***** There are " << copyL*copyL*(copyLArZ+copyLXeZ) << " TPC Cells."<< G4endl;
		
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
		
		
    // top and bottom
    solidTOFout1 = new G4Box("TOFout1",TOFoutL*0.5,TOFoutL*0.5,TOFoutZ*0.5);
    logicTOFout1 = new G4LogicalVolume(solidTOFout1,TOFMaterial,"TOFout1",0,0,0);
    if(global.DetectorType != 2)
    {
      physiTOFout1 = new G4PVPlacement(0,
                                     G4ThreeVector(0.0,0.0,TOFspaceH+0.5*TOFinH), // at (x,y,z)
                                     logicTOFout1,    // its logical volume
                                     "TOFoutBottom",       // its name
                                     logicGondola,      // its mother  volume
                                     false,           // no boolean operations
                                     copyTOFout,								// copy number
                                     overlap);
      copyTOFout--;
    
      //physiTOFout1 = new G4PVPlacement(0,
                                     //G4ThreeVector(0.0,0.0,TOFspaceH+0.5*TOFinH-(TOFoutH+TOFoutZ*0.5)), // at (x,y,z)
                                     //logicTOFout1,    // its logical volume
                                     //"TOFoutTop",       // its name
                                     //logicGondola,      // its mother  volume
                                     //false,           // no boolean operations
                                     //copyTOFout,								// copy number
                                     //overlap);
      copyTOFout--;
      // logicTOFout1->SetSensitiveDetector(TOFSD);
      logicTOFout1->SetSensitiveDetector(TPCSD);
    }
    //side
    if(TOFoutH > 0.) // no side if TOFoutH = 0
    {
      solidTOFout2 = new G4Box("TOFout2",TOFoutL*0.5,TOFoutH*0.5-0.3*TOFinH,TOFoutZ*0.5);
      logicTOFout2 = new G4LogicalVolume(solidTOFout2,TOFMaterial,"TOFout2",0,0,0);
      if(global.DetectorType != 2)
      {
        physiTOFout2 = new G4PVPlacement(rotXTOF2,
                                       G4ThreeVector(0.0,-0.5*(TOFoutL+TOFoutZ+TOFoutH*cos(TOFoutAngle)),TOFspaceH+0.8*TOFinH-0.5*TOFoutH*sin(TOFoutAngle)), // at (x,y,z)
                                       logicTOFout2,    // its logical volume
                                       "TOFoutSide",       // its name
                                       logicGondola,      // its mother  volume
                                       false,           // no boolean operations
                                       copyTOFout,								// copy number
                                       overlap);
        copyTOFout--;
        physiTOFout2 = new G4PVPlacement(rotXTOF1,
                                       G4ThreeVector(0.0,0.5*(TOFoutL+TOFoutZ+TOFoutH*cos(TOFoutAngle)),TOFspaceH+0.8*TOFinH-0.5*TOFoutH*sin(TOFoutAngle)), // at (x,y,z)
                                       logicTOFout2,    // its logical volume
                                       "TOFoutSide",       // its name
                                       logicGondola,      // its mother  volume
                                       false,           // no boolean operations
                                       copyTOFout,								// copy number
                                       overlap);
        copyTOFout--;
        physiTOFout2 = new G4PVPlacement(rotZXTOF2,
                                       G4ThreeVector(-0.5*(TOFoutL+TOFoutZ+TOFoutH*cos(TOFoutAngle)),0.0,TOFspaceH+0.8*TOFinH-0.5*TOFoutH*sin(TOFoutAngle)), // at (x,y,z)
                                       logicTOFout2,    // its logical volume
                                       "TOFoutSide",       // its name
                                       logicGondola,      // its mother  volume
                                       false,           // no boolean operations
                                       copyTOFout,								// copy number
                                       overlap);
        copyTOFout--;
        physiTOFout2 = new G4PVPlacement(rotZXTOF1,
                                       G4ThreeVector(0.5*(TOFoutL+TOFoutZ+TOFoutH*cos(TOFoutAngle)),0.0,TOFspaceH+0.8*TOFinH-0.5*TOFoutH*sin(TOFoutAngle)), // at (x,y,z)
                                       logicTOFout2,    // its logical volume
                                       "TOFoutSide",       // its name
                                       logicGondola,      // its mother  volume
                                       false,           // no boolean operations
                                       copyTOFout,								// copy number
                                       overlap);
        copyTOFout--;
        // logicTOFout2->SetSensitiveDetector(TOFSD);
        logicTOFout2->SetSensitiveDetector(TPCSD);
      }
    }
		
		//------------------------------------------------
		// inner TOF
		//------------------------------------------------
		
		// top and bottom
		solidTOFin1 = new G4Box("TOFin1",TOFinL*0.5,TOFinL*0.5,TOFinZ*0.5);
		logicTOFin1 = new G4LogicalVolume(solidTOFin1,TOFMaterial,"TOFin1",0,0,0);
		
		for(int i=0; i<2;i++)
		{
			if(global.DetectorType == 2) break;
      physiTOFin1 = new G4PVPlacement(0,
																			G4ThreeVector(0.0,0.0,-(i-0.5)*TOFinH), // at (x,y,z)
																			logicTOFin1,    // its logical volume
																			"TOFin1",       // its name
																			logicGondola,      // its mother  volume
																			false,           // no boolean operations
																			copyTOFin,								// copy number
																			overlap);
			copyTOFin--;
		}
		// side
		solidTOFin2 = new G4Box("TOFin2",TOFinL*0.5,TOFinH*0.5,TOFinZ*0.5);
		logicTOFin2 = new G4LogicalVolume(solidTOFin2,TOFMaterial,"TOFin2",0,0,0);
		
		for(int i=0; i<2;i++)
		{
			if(global.DetectorType == 2) break;
      physiTOFin2 = new G4PVPlacement(rotX,
																			G4ThreeVector(0.0,-(i-0.5)*(TOFinL+TOFinZ),0.0), // at (x,y,z)
																			logicTOFin2,    // its logical volume
																			"TOFin2",       // its name
																			logicGondola,      // its mother  volume
																			false,           // no boolean operations
																			copyTOFin,								// copy number
																			overlap);
			copyTOFin--;
		}
		for(int i=0; i<2;i++)
		{
			if(global.DetectorType == 2) break;
      physiTOFin2 = new G4PVPlacement(rotZX,
																			G4ThreeVector(-(i-0.5)*(TOFinL+TOFinZ),0.0,0.0), // at (x,y,z)
																			logicTOFin2,    // its logical volume
																			"TOFin2",       // its name
																			logicGondola,      // its mother  volume
																			false,           // no boolean operations
																			copyTOFin,								// copy number
																			overlap);
			copyTOFin--;
		}
    if(global.DetectorType != 2)
    {
      // logicTOFin1->SetSensitiveDetector(TOFSD);
      // logicTOFin2->SetSensitiveDetector(TOFSD);
      logicTOFin1->SetSensitiveDetector(TPCSD);
      logicTOFin2->SetSensitiveDetector(TPCSD);
    }
    //--------- Visualization attributes -------------------------------

    whiteVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,1.0,0.2));
    redVisAtt = new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.2));
    greenVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,0.0,0.2));
    blueVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0,0.2));
    yellowVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0,0.2));
    greyVisAtt = new G4VisAttributes(G4Colour(153/255.,153/255.,153/255.));
    logicWorld->SetVisAttributes(whiteVisAtt);
    if(logicGondola) logicGondola->SetVisAttributes(greyVisAtt);
    if(logicChamber) logicChamber->SetVisAttributes(yellowVisAtt);
    if(logicTOFin1) logicTOFin1->SetVisAttributes(greenVisAtt);
    if(logicTOFin2) logicTOFin2->SetVisAttributes(greenVisAtt);
    if(logicTOFout1) logicTOFout1->SetVisAttributes(greenVisAtt);
    if(logicTOFout2) logicTOFout2->SetVisAttributes(greenVisAtt);
    // if(logicLArTPC) logicLArTPC->SetVisAttributes(greyVisAtt);
    if(logicLArCell) logicLArCell->SetVisAttributes(redVisAtt);
    if(logicLXeTPC) logicLXeTPC->SetVisAttributes(blueVisAtt);
  }
	return physiWorld;
}

void GRAMSDetectorConstruction::SetTPC_space(G4double NewValue)
{
  TPCspace = NewValue;
}
void GRAMSDetectorConstruction::SetLArTPC_Z(G4double NewValue)
{
	LArTPCZ = NewValue;
}
void GRAMSDetectorConstruction::SetLXeTPC_Z(G4double NewValue)
{
  LXeTPCZ = NewValue;
}
void GRAMSDetectorConstruction::SetTPC_L(G4double NewValue)
{
	TPCL = NewValue;
}
void GRAMSDetectorConstruction::SetTOFout_Z(G4double NewValue)
{
	TOFoutZ = NewValue;
}
void GRAMSDetectorConstruction::SetTOFout_L(G4double NewValue)
{
	TOFoutL = NewValue;
}
void GRAMSDetectorConstruction::SetTOFout_H(G4double NewValue)
{
	TOFoutH = NewValue;
}
void GRAMSDetectorConstruction::SetTOFout_Angle(G4double NewValue)
{
	TOFoutAngle = NewValue;
}
void GRAMSDetectorConstruction::SetGondola_Angle(G4double NewValue)
{
  GondolaAngle = NewValue;
}
void GRAMSDetectorConstruction::SetTOFin_Z(G4double NewValue)
{
	TOFinZ = NewValue;
}
void GRAMSDetectorConstruction::SetAtmos_Z(G4double NewValue)
{
	atmosZ = NewValue;
}

void GRAMSDetectorConstruction::UpdateGeometry()
{
	// Cleanup old geometry
	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();
	
	G4RunManager::GetRunManager()->DefineWorldVolume(ConstructGRAMSDetector());
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

