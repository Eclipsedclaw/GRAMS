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

#ifndef GRAMSDetectorConstruction_h
#define GRAMSDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "GRAMSDetectorSD.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"

class G4Box;
class G4Tubs;
class G4Sphere;
class G4Ellipsoid;
class G4Trd;
class G4Polyhedra;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4VSolid;
class G4UnionSolid;
class G4SubtractionSolid;
class G4IntersectionSolid;
class G4Material;
class GRAMSDetectorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class GRAMSDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
	GRAMSDetectorConstruction();
    ~GRAMSDetectorConstruction();
	void DefineMaterials();
	void SetTPC_space(G4double);
  void SetLArTPC_Z(G4double);
  void SetLXeTPC_Z(G4double);
	void SetTPC_L(G4double);
	void SetTOFout_Z(G4double);
	void SetTOFout_L(G4double);
	void SetTOFout_H(G4double);
	void SetTOFout_Angle(G4double);
  void SetGondola_Angle(G4double);
	void SetTOFin_Z(G4double);
	void SetAtmos_Z(G4double);
	void UpdateGeometry();

  public:
  
	G4VPhysicalVolume* Construct();
     
	const
	G4double GetWorldFullLength()   {return WorldLength;}; 
		 
  private:
	
	G4VPhysicalVolume* ConstructGRAMSDetector();
	
	G4Box*             solidWorld;    // pointer to the solid world 
	G4LogicalVolume*   logicWorld;    // pointer to the logical world
	G4VPhysicalVolume* physiWorld;    // pointer to the physical world
	
  G4Box*             solidGondola;    // pointer to the solid Gondola
  G4LogicalVolume*   logicGondola;    // pointer to the logical Gondola
  G4VPhysicalVolume* physiGondola;    // pointer to the physical Gondola
  
	G4Box*             solidAtmos;  // pointer to the solid Atmos
	G4LogicalVolume*   logicAtmos;  // pointer to the logical Atmos
	G4VPhysicalVolume* physiAtmos;  // pointer to the physical Atmos
		                
	G4Box*             solidScore1;  // pointer to the solid Score
	G4LogicalVolume*   logicScore1;  // pointer to the logical Score
	G4VPhysicalVolume* physiScore1;  // pointer to the physical Score

	G4Box*             solidScore2;  // pointer to the solid Score
	G4LogicalVolume*   logicScore2;  // pointer to the logical Score
	G4VPhysicalVolume* physiScore2;  // pointer to the physical Score
  
  G4Box*             solidChamber;    // pointer to the solid Chamber
  G4LogicalVolume*   logicChamber;    // pointer to the logical Chamber
  G4VPhysicalVolume* physiChamber;    // pointer to the physical Chamber
  
  G4Box*             solidChamberInside;    // pointer to the solid ChamberInside
  G4LogicalVolume*   logicChamberInside;    // pointer to the logical ChamberInside
  G4VPhysicalVolume* physiChamberInside;    // pointer to the physical ChamberInside
  
	G4Box*             solidLArTPC;  // pointer to the solid LArTPC
	G4LogicalVolume*   logicLArTPC;  // pointer to the logical LArTPC
	G4VPhysicalVolume* physiLArTPC;  // pointer to the physical LArTPC
	
	G4Box*             solidLArCell;  // pointer to the solid LAr Cell
	G4LogicalVolume*   logicLArCell;  // pointer to the logical LAr Cell
	G4VPhysicalVolume* physiLArCell;  // pointer to the physical LAr Cell

  G4VSolid*          solidLArColumn;   // pointer to the solid LAr Column
  G4LogicalVolume*   logicLArColumn;   // pointer to the logical LAr Column
  G4VPhysicalVolume* physiLArColumn;   // pointer to the physical LAr Column
  
  G4VSolid*          solidLArLayer;  // pointer to the solid LAr Layer
  G4LogicalVolume*   logicLArLayer;  // pointer to the logical LAr Layer
  G4VPhysicalVolume* physiLArLayer;  // pointer to the physical LAr Layer
  
	G4Box*             solidTOFin1;  // pointer to the solid TOF
	G4LogicalVolume*   logicTOFin1;  // pointer to the logical TOF
	G4VPhysicalVolume* physiTOFin1;  // pointer to the physical TOF

	G4Box*             solidTOFin2;  // pointer to the solid TOF
	G4LogicalVolume*   logicTOFin2;  // pointer to the logical TOF
	G4VPhysicalVolume* physiTOFin2;  // pointer to the physical TOF

	G4Box*             solidTOFout1;  // pointer to the solid TOF
	G4LogicalVolume*   logicTOFout1;  // pointer to the logical TOF
	G4VPhysicalVolume* physiTOFout1;  // pointer to the physical TOF

	G4Box*             solidTOFout2;  // pointer to the solid TOF
	G4LogicalVolume*   logicTOFout2;  // pointer to the logical TOF
	G4VPhysicalVolume* physiTOFout2;  // pointer to the physical TOF

	G4Polyhedra*			 solidHex;			// pointer to the solid Hex
	G4Box*             solidBox;			// pointer to the solid Box
	G4Tubs*            solidCylinder; // pointer to the solid Cylinder
 
  G4Box*             solidLXeTPC;  // pointer to the solid LXeTPC
  G4LogicalVolume*   logicLXeTPC;  // pointer to the logical LXeTPC
  G4VPhysicalVolume* physiLXeTPC;  // pointer to the physical LXeTPC
  
  G4VSolid*          solidLXeColumn;   // pointer to the solid LXe Column
  G4LogicalVolume*   logicLXeColumn;   // pointer to the logical LXe Column
  G4VPhysicalVolume* physiLXeColumn;   // pointer to the physical LXe Column
  
  G4VSolid*          solidLXeLayer;  // pointer to the solid LXe Layer
  G4LogicalVolume*   logicLXeLayer;  // pointer to the logical LXe Layer
  G4VPhysicalVolume* physiLXeLayer;  // pointer to the physical LXe Layer
	
  G4VSolid*          solidLXeCell;  // pointer to the solid LXe Cell
  G4LogicalVolume*   logicLXeCell;  // pointer to the logical LXe Cell
  G4VPhysicalVolume* physiLXeCell;  // pointer to the physical LXe Cell
  
	G4VSolid*          solidSiLi[10];  // pointer to the solid SiLi
	G4LogicalVolume*   logicSiLi[10];  // pointer to the logical SiLi
	G4VPhysicalVolume* physiSiLi[10];  // pointer to the physical SiLi
	
	G4Material* Air;
	G4Material* CompressedAir;
	G4Material* Vacuum;
	G4Material* FlightAir;
  G4Material* LAr;
  G4Material* LXe;
	G4Material* Silicon;
	G4Material* Plastic;
	G4Material* Al;
	G4Material* Carbon;
	G4Material* Fe;
	G4Material* Pb;
	
	G4Material*        worldMaterial; // pointer to the world material
	G4Material*        atmosMaterial; // pointer to the atmos material
	G4Material*        scoreMaterial; // pointer to the Score material
	G4Material*        ChamberMaterial; // pointer to the Chamber material
  G4Material*        TPCMaterial; // pointer to the TPC material
	G4Material*        TOFMaterial; // pointer to the TOF frame material
     
	G4double WorldLength;            // Full length of the world volume
	G4double atmosL;
	G4double atmosZ;
	G4double scoreL;
	G4double scoreZ;
	G4double angle;
  G4double space;
	G4double TPCL;					    //TPC length
	G4double TPCZ;					    //TPC thickness
  G4double ChamberT;          //Chamber thickness
  G4double LArTPCZ;           //LArTPC thickness
  G4double LXeTPCZ;           //LXeTPC thickness
  G4double TPCspace;          //TPC space
  G4double CellL;             //Cell length
  G4double CellLArZ;          //LArCell thickness
  G4double CellLXeZ;          //LXeCell thickness
  G4double TOFinL;					  //Full length of the TOF length
	G4double TOFinH;					
	G4double TOFinZ;					  //Full thickness of the TOF length
	G4double TOFspaceL;				  // distance between TOFin and TOFout
  G4double TOFspaceH;          // distance between TOFin and TOFout
	G4double TOFoutL;					  //Full length of the TOF length
	G4double TOFoutH;
	G4double TOFoutAngle;
  G4double GondolaAngle;
	G4double TOFoutZ;					  //Full thickness of the TOF length
  G4int overlap;
	 
	G4int NbOfLayer;
  G4int NbOfLArLayer;
  G4int NbOfLXeLayer;
	G4int copyL;
  G4int copyLArZ;
  G4int copyLXeZ;
	
	G4int copyWorld;
	G4int copyAtmosphere;
	G4int copyScore1;
	G4int copyScore2;
	G4int copyTPC;
  G4int copyCell;
	G4int copyTOFout;
	G4int copyTOFin;
	
	G4VSensitiveDetector* ScoreSD;
	G4VSensitiveDetector* TOFSD;
	G4VSensitiveDetector* TPCSD;
	GRAMSDetectorMessenger* detectorMessenger;  // pointer to the Messenger
	
	G4VisAttributes* whiteVisAtt;
	G4VisAttributes* redVisAtt;
	G4VisAttributes* greenVisAtt;
	G4VisAttributes* blueVisAtt;
  G4VisAttributes* yellowVisAtt;
  G4VisAttributes* greyVisAtt;
	
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
