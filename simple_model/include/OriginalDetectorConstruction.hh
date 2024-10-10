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
// $Id: OriginalDetectorConstruction.hh,v 1.8 2006/06/29 17:47:30 gunter Exp $
// GEANT4 tag $Name: geant4-09-00 $
//
// @author Tsuguo Aramaki
// @date 2015 March 23
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef OriginalDetectorConstruction_h
#define OriginalDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "OriginalDetectorSD.hh"
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
class OriginalDetectorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class OriginalDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
	OriginalDetectorConstruction();
    ~OriginalDetectorConstruction();
	void DefineMaterials();
	void UpdateGeometry();

  public:
  
	G4VPhysicalVolume* Construct();
	G4double GetWorldFullLength()   {return WorldLength;}; 
		 
  private:
	
	G4VPhysicalVolume* ConstructOriginalDetector();
    
    G4VSolid*           SolidWorld;         // pointer to the solid world
    G4LogicalVolume*    LVWorld;            // pointer to the logical world
    G4VPhysicalVolume*  PVWorld;            // pointer to the physical world
    
    G4VSolid*           SolidDetector;       // pointer to the solid Ge Detector
    G4LogicalVolume*    LVDetector;          // pointer to the logical Ge Detector
    G4VPhysicalVolume*  PVDetector;          // pointer to the physical Ge Detector
    
    G4Material* Hydrogen;
    G4Material* Air;
    G4Material* Si;
    G4Material* Al;
    G4Material* Cu;
    G4Material* Fe;
    G4Material* Ge;
    G4Material* Pb;
	
	G4double test[10000];

	G4VSensitiveDetector*	DetectorSD;
	OriginalDetectorMessenger* detectorMessenger;  // pointer to the Messenger
    
    G4Material*     MaterialWorld;          // pointer to the world material
    G4Material*     MaterialDetector;     // pointer to the Ge Detector material
    
    G4double WorldLength;            // Full length of the world volume
    G4int overlap;
    
    G4double rDetector;                   // radius of Ge Detector
    G4double tDetector;                   // thickness of Ge Detector
    G4double lDetector;                   // length of Ge Detector
    
    G4int CopyWorld;
    G4int CopyDetector;
    
    G4VisAttributes* WhiteVisAtt;
    G4VisAttributes* RedVisAtt;
    G4VisAttributes* GreenVisAtt;
    G4VisAttributes* BlueVisAtt;
    G4VisAttributes* GrayVisAtt;
    G4VisAttributes* CyanVisAtt;
    G4VisAttributes* MagentaVisAtt;
    G4VisAttributes* YellowVisAtt;
	
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
