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
// $Id: OriginalDetectorConstruction.cc,v 1.19 2007/05/11 14:35:01 gcosmo Exp $
// GEANT4 tag $Name: geant4-09-00 $
//
// @author Tsuguo Aramaki
// @date 2015 March 23
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "OriginalDetectorConstruction.hh"
#include "OriginalDetectorMessenger.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OriginalDetectorConstruction::OriginalDetectorConstruction()
:SolidWorld(0),LVWorld(0),PVWorld(0)
{
  DefineMaterials();
	detectorMessenger = new OriginalDetectorMessenger(this);
    
    extern global_struct global;
	// sprintf(global.outdir, "tmp");
	// sprintf(global.outfile, "test");
    
    // default parameter
    rDetector = 5.0*cm;
    tDetector = rDetector;
    lDetector = 1.0*m;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OriginalDetectorConstruction::~OriginalDetectorConstruction()
{
  delete detectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OriginalDetectorConstruction::DefineMaterials()
{
    // Material definition with NIST Manager
    G4double a, z;
    G4double density, temperature, pressure;
    G4int nel;
    
    G4NistManager* nistManager = G4NistManager::Instance();
    
    Air = nistManager->FindOrBuildMaterial("G4_AIR");
    Al = nistManager->FindOrBuildMaterial("G4_Al");
    Si = nistManager->FindOrBuildMaterial("G4_Si");
    Ge = nistManager->FindOrBuildMaterial("G4_Ge");
    Fe = nistManager->FindOrBuildMaterial("G4_Fe");
    Cu = nistManager->FindOrBuildMaterial("G4_Cu");
    Pb = nistManager->FindOrBuildMaterial("G4_Pb");
    // Vacuum
    density= 2.376e-15*g/cm3;
    temperature= 300*kelvin;
    pressure= 2.0e-7*bar;
    Vacuum = new G4Material("Vacuum", density, nel=1, kStateGas,temperature,pressure);
    Vacuum-> AddMaterial(Air, 100*perCent);
    // Liquid Argone
    LAr = new G4Material("LAr",z=18.,a=39.95*g/mole,density=1.4*g/cm3);
    // Liquid Xenon
    LXe = new G4Material("LXe",z=54.,a=131.29*g/mole,density=3.020*g/cm3);
    
    // Print materials
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

G4VPhysicalVolume* OriginalDetectorConstruction::Construct()
{
	return ConstructOriginalDetector();
}
G4VPhysicalVolume* OriginalDetectorConstruction::ConstructOriginalDetector()
{
	extern global_struct global;
	overlap = global.CheckOverlap;
	
    //------------------------------------------------
	// Sensitive detectors
	//------------------------------------------------
	
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
	if(!DetectorSD)
	{
		DetectorSD = new OriginalDetectorSD("DetectorSD");
		
		SDman->AddNewDetector(DetectorSD);
	}

    //------------------------------------------------
    // Copy Number
    //------------------------------------------------
    
    CopyDetector = 1;
    CopyWorld = -1;
    
    //--------- Definitions of Solids, Logical Volumes, Physical Volumes ---------
    
    
    //------------------------------------------------
    // Material
    //------------------------------------------------
    
    MaterialWorld = Vacuum;
    MaterialDetector = LAr;
    
	//------------------------------------------------

    //------------------------------------------------
    // World
    //------------------------------------------------
    
    WorldLength = 100.0*m;
    // G4GeometryManager::GetInstance()->SetWorldMaximumExtent(WorldLength);
    //  G4cout << "Computed tolerance = "
    //         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
    //         << " mm" << G4endl;
    
    SolidWorld = new G4Box("world",WorldLength*0.5,WorldLength*0.5,WorldLength*0.5);
    LVWorld= new G4LogicalVolume(SolidWorld, MaterialWorld, "World", 0, 0, 0);
    
    //  Must place the World Physical volume unrotated at (0,0,0).
    //
    PVWorld = new G4PVPlacement(0,                // no rotation
                                G4ThreeVector(),  // at (0,0,0)
                                LVWorld,     // its logical volume
                                "World",          // its name
                                0,                // its mother  volume
                                false,            // no boolean operations
                                CopyWorld,        // copy number
                                true);
    
    //------------------------------------------------
    // Detector
    //------------------------------------------------
    
    SolidDetector = new G4Box("SolidDetector",lDetector*1.5,lDetector*1.5,lDetector*0.2);
    LVDetector = new G4LogicalVolume(SolidDetector,MaterialDetector,"LVDetector",0,0,0);
    PVDetector = new G4PVPlacement(0,
                                  G4ThreeVector(),     // at (x,y,z)
                                  LVDetector,       // its logical volume
                                  "Detector",            // its name
                                  LVWorld,        // its mother  volume
                                  false,               // no boolean operations
                                  CopyDetector,          // copy number
                                  overlap);
  
    LVDetector->SetSensitiveDetector(DetectorSD);
  
    //------------------------------------------------
    // Visualization attributes
    //------------------------------------------------
    
    WhiteVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0,0.2));
    RedVisAtt= new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.2));
    GreenVisAtt= new G4VisAttributes(G4Colour(0.0,1.0,0.0,0.2));
    BlueVisAtt= new G4VisAttributes(G4Colour(0.0,0.0,1.0,0.2));
    GrayVisAtt= new G4VisAttributes(G4Colour(0.5,0.5,0.5,0.2));
    CyanVisAtt= new G4VisAttributes(G4Colour(0.0,1.0,1.0,0.2));
    MagentaVisAtt= new G4VisAttributes(G4Colour(1.0,0.0,1.0,0.2));
    YellowVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,0.0,0.2));
    
    LVWorld->SetVisAttributes(WhiteVisAtt);
    LVDetector->SetVisAttributes(WhiteVisAtt);
    
	return PVWorld;
}

void OriginalDetectorConstruction::UpdateGeometry()
{
	// Cleanup old geometry
	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();
	
	G4RunManager::GetRunManager()->DefineWorldVolume(ConstructOriginalDetector());
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

