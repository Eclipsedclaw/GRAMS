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

#ifndef GRAMSDetectorMessenger_h
#define GRAMSDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"
#include "global.h"

class GRAMSDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class GRAMSDetectorMessenger: public G4UImessenger
{
  public:
    GRAMSDetectorMessenger(GRAMSDetectorConstruction*);
   ~GRAMSDetectorMessenger();
    
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
  private:
	
	GRAMSDetectorConstruction* myDetector;
	G4UIcmdWithAString*	       OutDirCmd;
	G4UIcmdWithAString*	       OutFileCmd;
	G4UIcmdWithAString*	       InDirCmd;
	G4UIcmdWithAString*	       InFileCmd;
	G4UIcmdWithAnInteger*      GunSeedCmd;
	G4UIcmdWithAnInteger*      CheckOverlapCmd;
	G4UIcmdWithAnInteger*      DetectorTypeCmd;
	G4UIcmdWithAnInteger*			 DetectorVisualizationCmd;
	G4UIcmdWithAnInteger*			 NbOfLArLayerCmd;
  G4UIcmdWithAnInteger*      NbOfLXeLayerCmd;
  G4UIcmdWithAnInteger*      NbOfCellLCmd;
  G4UIcmdWithAnInteger*      NbOfLArCellZCmd;
  G4UIcmdWithAnInteger*      NbOfLXeCellZCmd;
	G4UIcmdWithAnInteger*      OutputTypeCmd;
	G4UIcmdWithAnInteger*      OutputFormatCmd;
	G4UIcmdWithAnInteger*      TrackTypeCmd;
  G4UIcmdWithAnInteger*      TrackEdepCmd;
	G4UIcmdWithAnInteger*      StopEventCmd;
	G4UIcmdWithAnInteger*      ParticleSourceCmd;
	G4UIcmdWithADoubleAndUnit* EminCmd;
	G4UIcmdWithADoubleAndUnit* EmaxCmd;
	G4UIcmdWithADoubleAndUnit* areaCmd;
	G4UIcmdWithADoubleAndUnit* TPC_spaceCmd;
  G4UIcmdWithADoubleAndUnit* LArTPC_ZCmd;
  G4UIcmdWithADoubleAndUnit* LXeTPC_ZCmd;
	G4UIcmdWithADoubleAndUnit* TPC_LCmd;
	G4UIcmdWithADoubleAndUnit* TOFout_ZCmd;
	G4UIcmdWithADoubleAndUnit* TOFout_LCmd;
	G4UIcmdWithADoubleAndUnit* TOFout_HCmd;
	G4UIcmdWithADoubleAndUnit* TOFout_AngleCmd;
	G4UIcmdWithADoubleAndUnit* TOFin_ZCmd;
  G4UIcmdWithADoubleAndUnit* Gondola_AngleCmd;
	G4UIcmdWithADoubleAndUnit* Atmos_ZCmd;
	G4UIcmdWithoutParameter*   UpdateCmd;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

