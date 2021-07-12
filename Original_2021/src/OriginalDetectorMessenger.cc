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
// $Id: OriginalDetectorMessenger.cc,v 1.11 2006/06/29 17:48:02 gunter Exp $
// GEANT4 tag $Name: geant4-09-00 $
//
// @author Tsuguo Aramaki
// @date 2015 March 23
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "OriginalDetectorMessenger.hh"

#include "OriginalDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "globals.hh"
#include "global.h"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OriginalDetectorMessenger::OriginalDetectorMessenger(OriginalDetectorConstruction* myDet)
:myDetector(myDet)
{ 
// output directry
  OutDirCmd = new G4UIcmdWithAString("/OutputDirectory", this);
  OutDirCmd->SetGuidance("Directory for output files");
  OutDirCmd->SetParameterName("OutputDirectory", true );
  OutDirCmd->SetDefaultValue("./tmp");
	
// output file
  OutFileCmd = new G4UIcmdWithAString("/OutputFile", this );
  OutFileCmd->SetGuidance("Name of output file");
  OutFileCmd->SetParameterName("OutputFile",true);
  OutFileCmd->SetDefaultValue("test");

// input directry
  InDirCmd = new G4UIcmdWithAString("/InputDirectory", this);
  InDirCmd->SetGuidance("Directory for input files");
  InDirCmd->SetParameterName("InputDirectory", true );
  InDirCmd->SetDefaultValue("./");

//input file
  InFileCmd = new G4UIcmdWithAString("/InputFile", this );
  InFileCmd->SetGuidance("Name of input file");
  InFileCmd->SetParameterName("InputFile",true);
  InFileCmd->SetDefaultValue("test");
	
// seed value for random number generator
  GunSeedCmd = new G4UIcmdWithAnInteger( "/gun/seed", this );
  GunSeedCmd->SetGuidance("Seed value for random number generator");
  GunSeedCmd->SetParameterName( "num", true );
  GunSeedCmd->SetDefaultValue(9876);

// Output Format, 0 for simple mode (no text), 1 include texts (process name etc.)
  OutputFormatCmd = new G4UIcmdWithAnInteger("/OutputFormat", this );
  OutputFormatCmd->SetGuidance("OutputFormat");
  OutputFormatCmd->SetParameterName("flag",true);
  OutputFormatCmd->SetDefaultValue(0);
	
// Track Type, 0 for all particles, 1 for only primary particle (trackID = 1)
  TrackTypeCmd = new G4UIcmdWithAnInteger("/TrackType", this );
  TrackTypeCmd->SetGuidance("TrackType");
  TrackTypeCmd->SetParameterName("flag",true);
  TrackTypeCmd->SetDefaultValue(1);
    
// Track Edep, 0 for all particles, 1 for only eDeo > 0.keV
  TrackEdepCmd = new G4UIcmdWithAnInteger("/TrackEdep", this );
  TrackEdepCmd->SetGuidance("TrackEdep");
  TrackEdepCmd->SetParameterName("flag",true);
  TrackEdepCmd->SetDefaultValue(1);
	
// General Particle Source, 0 for off (particle gun), 1 for on (GPS) 
  GPSCmd = new G4UIcmdWithAnInteger("/GeneralParticleSource", this );
  GPSCmd->SetGuidance("GeneralParticleSource");
  GPSCmd->SetParameterName("flag",true);
  GPSCmd->SetDefaultValue(0);
	
// Detector Visualization
  DetectorVisualizationCmd = new G4UIcmdWithAnInteger("/Visualization",this);  
  DetectorVisualizationCmd->SetGuidance("Detector Visualization");
  DetectorVisualizationCmd->SetParameterName("DetectorVisualization",false);
	DetectorVisualizationCmd-> SetDefaultValue(0);

// Detector overlap check
  CheckOverlapCmd = new G4UIcmdWithAnInteger("/CheckOverlap", this );
  CheckOverlapCmd->SetGuidance("Verbose level of Originals");
  CheckOverlapCmd->SetParameterName("flag",true);
  CheckOverlapCmd->SetDefaultValue(0);
	
// update geometry
	UpdateCmd = new G4UIcmdWithoutParameter("/update",this);
  UpdateCmd->SetGuidance("Update geometry.");
  UpdateCmd->AvailableForStates(G4State_Idle);
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OriginalDetectorMessenger::~OriginalDetectorMessenger()
{
	delete OutDirCmd;
  delete OutFileCmd;
	delete InDirCmd;
  delete InFileCmd;
  delete GunSeedCmd;
	delete OutputFormatCmd;
	delete TrackTypeCmd;
  delete TrackEdepCmd;
	delete GPSCmd;
  delete DetectorVisualizationCmd;
	delete CheckOverlapCmd;
	delete UpdateCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OriginalDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
 extern global_struct global;
	if(command == OutDirCmd) strcpy(global.outdir, newValue );
	if(command == OutFileCmd) strcpy(global.outfile, newValue );
	if(command == InDirCmd) strcpy(global.indir, newValue );
	if(command == InFileCmd) strcpy(global.infile, newValue );
  if(command == GunSeedCmd) global.seed=(GunSeedCmd->GetNewIntValue(newValue));
	if(command == OutputFormatCmd) global.OutputFormat = (OutputFormatCmd->GetNewIntValue(newValue));
	if(command == TrackTypeCmd) global.TrackType = (TrackTypeCmd->GetNewIntValue(newValue));
  if(command == TrackEdepCmd) global.TrackEdep = (TrackEdepCmd->GetNewIntValue(newValue));
	if(command == GPSCmd) global.GPS = (GPSCmd->GetNewIntValue(newValue));
  if(command == DetectorVisualizationCmd) global.DetectorVisualization = (DetectorVisualizationCmd->GetNewIntValue(newValue));
	if(command == CheckOverlapCmd) global.CheckOverlap = (CheckOverlapCmd->GetNewIntValue(newValue));
	if(command == UpdateCmd) myDetector->UpdateGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
