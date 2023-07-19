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

#include "GRAMSDetectorMessenger.hh"

#include "GRAMSDetectorConstruction.hh"
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

GRAMSDetectorMessenger::GRAMSDetectorMessenger(GRAMSDetectorConstruction* myDet)
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

// Verbose level for Detector Type, 0 for reference, 1 for full geometry, 2 for TPC only (no TOF, no atmosphere, no scoring)
  DetectorTypeCmd = new G4UIcmdWithAnInteger("/DetectorType", this );
  DetectorTypeCmd->SetGuidance("DetectorType");
  DetectorTypeCmd->SetParameterName("flag",true);
  DetectorTypeCmd->SetDefaultValue(1);
	
// Output Type, 0 for ASCII, 1 for ROOT
  OutputTypeCmd = new G4UIcmdWithAnInteger("/OutputType", this );
  OutputTypeCmd->SetGuidance("OutputType");
  OutputTypeCmd->SetParameterName("flag",true);
  OutputTypeCmd->SetDefaultValue(0);	

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

// Stop Event, 0 for all, 1 for stop event (trackID = 1), 2 for both stop and inelastic scattering/inflight annihilation
  StopEventCmd = new G4UIcmdWithAnInteger("/StopEvent", this );
  StopEventCmd->SetGuidance("StopEvent");
  StopEventCmd->SetParameterName("flag",true);
  StopEventCmd->SetDefaultValue(0);
	
// Particle Source, 0 for particle gun, 1 for GPS
  ParticleSourceCmd = new G4UIcmdWithAnInteger("/ParticleSource", this );
  ParticleSourceCmd->SetGuidance("ParticleSource");
  ParticleSourceCmd->SetParameterName("flag",true);
  ParticleSourceCmd->SetDefaultValue(0);
	
// min E for the primary particle
  EminCmd = new G4UIcmdWithADoubleAndUnit("/PrimaryParticle/Emin",this);  
  EminCmd->SetGuidance("min E (MeV)");
  EminCmd->SetParameterName("Emin",true);
	EminCmd->SetRange("Emin >= 0."); 
	EminCmd->SetDefaultValue(0.0);
	EminCmd->SetDefaultUnit("MeV");

// max E for the primary particle
  EmaxCmd = new G4UIcmdWithADoubleAndUnit("/PrimaryParticle/Emax",this);  
  EmaxCmd->SetGuidance("max E (MeV)");
  EmaxCmd->SetParameterName("Emax",true);
	EmaxCmd->SetRange("Emax >= 0."); 
	EmaxCmd->SetDefaultValue(0.0);
	EmaxCmd->SetDefaultUnit("MeV");	

// area for primary particle generated
  areaCmd = new G4UIcmdWithADoubleAndUnit("/PrimaryParticle/area",this);  
  areaCmd->SetGuidance("area length (cm)");
  areaCmd->SetParameterName("area",true);
	areaCmd->SetRange("area >= 0."); 
	areaCmd->SetDefaultValue(1000.0);
	areaCmd->SetDefaultUnit("cm");

// Detector Visualization
  DetectorVisualizationCmd = new G4UIcmdWithAnInteger("/Visualization",this);  
  DetectorVisualizationCmd->SetGuidance("Detector Visualization");
  DetectorVisualizationCmd->SetParameterName("DetectorVisualization",false);
	DetectorVisualizationCmd-> SetDefaultValue(0);

// Gondola angle
  Gondola_AngleCmd = new G4UIcmdWithADoubleAndUnit("/Gondola_Angle",this);
  Gondola_AngleCmd->SetGuidance("Gondola rotation angle (deg)");
  Gondola_AngleCmd->SetParameterName("Gondola_Angle",true);
  Gondola_AngleCmd->SetRange("Gondola_Angle > -90.");
  Gondola_AngleCmd->SetDefaultValue(0.0);
  Gondola_AngleCmd->SetDefaultUnit("deg");

// NbOfLArLayer
  NbOfLArLayerCmd = new G4UIcmdWithAnInteger("/NbOfLArLayer",this);
  NbOfLArLayerCmd->SetGuidance("NbOfLArLayer");
  NbOfLArLayerCmd->SetParameterName("NbOfLArLayer",false);
	NbOfLArLayerCmd-> SetDefaultValue(1);
  
// NbOfLXeLayer
  NbOfLXeLayerCmd = new G4UIcmdWithAnInteger("/NbOfLXeLayer",this);
  NbOfLXeLayerCmd->SetGuidance("NbOfLXeLayer");
  NbOfLXeLayerCmd->SetParameterName("NbOfLXeLayer",false);
  NbOfLXeLayerCmd-> SetDefaultValue(1);
  
// NbOfCellL
  NbOfCellLCmd = new G4UIcmdWithAnInteger("/NbOfCellL",this);
  NbOfCellLCmd->SetGuidance("NbOfCellL");
  NbOfCellLCmd->SetParameterName("NbOfCellL",false);
  NbOfCellLCmd-> SetDefaultValue(1);

// NbOfLArCellZ
  NbOfLArCellZCmd = new G4UIcmdWithAnInteger("/NbOfLArCellZ",this);
  NbOfLArCellZCmd->SetGuidance("NbOfLArCellZ");
  NbOfLArCellZCmd->SetParameterName("NbOfLArCellZ",false);
  NbOfLArCellZCmd-> SetDefaultValue(1);

// NbOfLXeCellZ
  NbOfLXeCellZCmd = new G4UIcmdWithAnInteger("/NbOfLXeCellZ",this);
  NbOfLXeCellZCmd->SetGuidance("NbOfLXeCellZ");
  NbOfLXeCellZCmd->SetParameterName("NbOfLXeCellZ",false);
  NbOfLXeCellZCmd-> SetDefaultValue(1);
	
// LArTPC thickness
  LArTPC_ZCmd = new G4UIcmdWithADoubleAndUnit("/LArTPC_Z",this);
  LArTPC_ZCmd->SetGuidance("LArTPC thickness (cm)");
  LArTPC_ZCmd->SetParameterName("LArTPC_Z",true);
	LArTPC_ZCmd->SetRange("LArTPC_Z >= 0.");
	LArTPC_ZCmd->SetDefaultValue(15.0);
	LArTPC_ZCmd->SetDefaultUnit("cm");

// LXeTPC thickness
  LXeTPC_ZCmd = new G4UIcmdWithADoubleAndUnit("/LXeTPC_Z",this);
  LXeTPC_ZCmd->SetGuidance("LXeTPC thickness (cm)");
  LXeTPC_ZCmd->SetParameterName("LXeTPC_Z",true);
  LXeTPC_ZCmd->SetRange("LXeTPC_Z >= 0.");
  LXeTPC_ZCmd->SetDefaultValue(15.0);
  LXeTPC_ZCmd->SetDefaultUnit("cm");
  
// TPC space
  TPC_spaceCmd = new G4UIcmdWithADoubleAndUnit("/TPC_space",this);
  TPC_spaceCmd->SetGuidance("TPC space (cm)");
  TPC_spaceCmd->SetParameterName("TPC_space",true);
  TPC_spaceCmd->SetRange("TPC_space >= 0.");
  TPC_spaceCmd->SetDefaultValue(1.0);
  TPC_spaceCmd->SetDefaultUnit("cm");
	
// TPC length
  TPC_LCmd = new G4UIcmdWithADoubleAndUnit("/TPC_L",this);  
  TPC_LCmd->SetGuidance("TPC length (cm)");
  TPC_LCmd->SetParameterName("TPC_L",true);
	TPC_LCmd->SetRange("TPC_L > 0."); 
	TPC_LCmd->SetDefaultValue(200.0);
	TPC_LCmd->SetDefaultUnit("cm");

// TOFout thickness
  TOFout_ZCmd = new G4UIcmdWithADoubleAndUnit("/TOFout_Z",this);  
  TOFout_ZCmd->SetGuidance("TOFout thickness (cm)");
  TOFout_ZCmd->SetParameterName("TOFout_Z",true);
	TOFout_ZCmd->SetRange("TOFout_Z > 0."); 
	TOFout_ZCmd->SetDefaultValue(0.3);
	TOFout_ZCmd->SetDefaultUnit("cm");
	
// TOFout top length
  TOFout_LCmd = new G4UIcmdWithADoubleAndUnit("/TOFout_L",this);  
  TOFout_LCmd->SetGuidance("TOFout length (cm)");
  TOFout_LCmd->SetParameterName("TOFout_L",true);
	TOFout_LCmd->SetRange("TOFout_L > 0."); 
	TOFout_LCmd->SetDefaultValue(200.0);
	TOFout_LCmd->SetDefaultUnit("cm");
	
// TOFout side height
  TOFout_HCmd = new G4UIcmdWithADoubleAndUnit("/TOFout_H",this);  
  TOFout_HCmd->SetGuidance("TOFout height (cm)");
  TOFout_HCmd->SetParameterName("TOFout_H",true);
	TOFout_HCmd->SetRange("TOFout_H >= 0."); 
	TOFout_HCmd->SetDefaultValue(200.0);
	TOFout_HCmd->SetDefaultUnit("cm");
	
// TOFout side angle
  TOFout_AngleCmd = new G4UIcmdWithADoubleAndUnit("/TOFout_Angle",this);  
  TOFout_AngleCmd->SetGuidance("TOFout rotation angle (deg)");
  TOFout_AngleCmd->SetParameterName("TOFout_Angle",true);
	TOFout_AngleCmd->SetRange("TOFout_Angle > 0."); 
	TOFout_AngleCmd->SetDefaultValue(90.0);
	TOFout_AngleCmd->SetDefaultUnit("deg");
	
// TOFin thickness
  TOFin_ZCmd = new G4UIcmdWithADoubleAndUnit("/TOFin_Z",this);  
  TOFin_ZCmd->SetGuidance("TOFin thickness (cm)");
  TOFin_ZCmd->SetParameterName("TOFin_Z",true);
	TOFin_ZCmd->SetRange("TOFin_Z > 0."); 
	TOFin_ZCmd->SetDefaultValue(0.3);
	TOFin_ZCmd->SetDefaultUnit("cm");
	
// Atmos thickness
  Atmos_ZCmd = new G4UIcmdWithADoubleAndUnit("/Atmos_Z",this);  
  Atmos_ZCmd->SetGuidance("Atmos thickness (cm)");
  Atmos_ZCmd->SetParameterName("Atmos_Z",true);
	Atmos_ZCmd->SetRange("Atmos_Z >= 0."); 
	Atmos_ZCmd->SetDefaultValue(3.9);
	Atmos_ZCmd->SetDefaultUnit("cm");

// Detector overlap check
  CheckOverlapCmd = new G4UIcmdWithAnInteger("/CheckOverlap", this );
  CheckOverlapCmd->SetGuidance("Verbose level of GRAMSs");
  CheckOverlapCmd->SetParameterName("flag",true);
  CheckOverlapCmd->SetDefaultValue(0);
	
// update geometry
	UpdateCmd = new G4UIcmdWithoutParameter("/update",this);
  UpdateCmd->SetGuidance("Update geometry.");
  UpdateCmd->AvailableForStates(G4State_Idle);
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GRAMSDetectorMessenger::~GRAMSDetectorMessenger()
{
	delete OutDirCmd;
  delete OutFileCmd;
	delete InDirCmd;
  delete InFileCmd;
  delete GunSeedCmd;
	delete DetectorTypeCmd;
	delete DetectorVisualizationCmd;
	delete NbOfLArLayerCmd;
  delete NbOfLXeLayerCmd;
  delete NbOfCellLCmd;
  delete NbOfLArCellZCmd;
  delete NbOfLXeCellZCmd;
	delete OutputTypeCmd;
	delete OutputFormatCmd;
	delete TrackTypeCmd;
  delete TrackEdepCmd;
	delete StopEventCmd;
	delete ParticleSourceCmd;
	delete EminCmd;
	delete EmaxCmd;
	delete areaCmd;
  delete TPC_spaceCmd;
	delete LArTPC_ZCmd;
  delete LXeTPC_ZCmd;
	delete TPC_LCmd;
	delete TOFout_ZCmd;
	delete TOFout_LCmd;
	delete TOFout_HCmd;
	delete TOFout_AngleCmd;
	delete TOFin_ZCmd;
  delete Gondola_AngleCmd;
	delete Atmos_ZCmd;
	delete CheckOverlapCmd;
	delete UpdateCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GRAMSDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
 extern global_struct global;
	if(command == OutDirCmd) strcpy(global.outdir, newValue );
	if(command == OutFileCmd) strcpy(global.outfile, newValue );
	if(command == InDirCmd) strcpy(global.indir, newValue );
	if(command == InFileCmd) strcpy(global.infile, newValue );
  if(command == GunSeedCmd) global.seed=(GunSeedCmd->GetNewIntValue(newValue));
	if(command == DetectorTypeCmd) global.DetectorType = (DetectorTypeCmd->GetNewIntValue(newValue));
	if(command == DetectorVisualizationCmd) global.DetectorVisualization = (DetectorVisualizationCmd->GetNewIntValue(newValue));
  if(command == OutputTypeCmd) global.OutputType = (OutputTypeCmd->GetNewIntValue(newValue));
  if(command == OutputFormatCmd) global.OutputFormat = (OutputFormatCmd->GetNewIntValue(newValue));
  if(command == TrackTypeCmd) global.TrackType = (TrackTypeCmd->GetNewIntValue(newValue));
  if(command == TrackEdepCmd) global.TrackEdep = (TrackEdepCmd->GetNewIntValue(newValue));
  if(command == StopEventCmd) global.StopEvent = (StopEventCmd->GetNewIntValue(newValue));
  if(command == ParticleSourceCmd) global.ParticleSource = (ParticleSourceCmd->GetNewIntValue(newValue));
  if(command == EminCmd) global.Emin = (EminCmd->GetNewDoubleValue(newValue));
  if(command == EmaxCmd) global.Emax = (EmaxCmd->GetNewDoubleValue(newValue));
  if(command == areaCmd) global.area = (areaCmd->GetNewDoubleValue(newValue));
  if(command == NbOfLArLayerCmd) global.NbLArTPC = (NbOfLArLayerCmd->GetNewIntValue(newValue));
  if(command == NbOfLXeLayerCmd) global.NbLXeTPC = (NbOfLXeLayerCmd->GetNewIntValue(newValue));
  if(command == NbOfCellLCmd) global.NbCellL = (NbOfCellLCmd->GetNewIntValue(newValue));
  if(command == NbOfLArCellZCmd) global.NbLArCellZ = (NbOfLArCellZCmd->GetNewIntValue(newValue));
  if(command == NbOfLXeCellZCmd) global.NbLXeCellZ = (NbOfLXeCellZCmd->GetNewIntValue(newValue));
  if(command == TPC_spaceCmd) myDetector->SetTPC_space(TPC_spaceCmd->GetNewDoubleValue(newValue));
  if(command == LArTPC_ZCmd) myDetector->SetLArTPC_Z(LArTPC_ZCmd->GetNewDoubleValue(newValue));
  if(command == LXeTPC_ZCmd) myDetector->SetLXeTPC_Z(LXeTPC_ZCmd->GetNewDoubleValue(newValue));
  if(command == TPC_LCmd) myDetector->SetTPC_L(TPC_LCmd->GetNewDoubleValue(newValue));
	if(command == TOFout_ZCmd) myDetector->SetTOFout_Z(TOFout_ZCmd->GetNewDoubleValue(newValue));
	if(command == TOFout_LCmd) myDetector->SetTOFout_L(TOFout_LCmd->GetNewDoubleValue(newValue));
	if(command == TOFout_HCmd) myDetector->SetTOFout_H(TOFout_HCmd->GetNewDoubleValue(newValue));
	if(command == TOFout_AngleCmd) myDetector->SetTOFout_Angle(TOFout_AngleCmd->GetNewDoubleValue(newValue));
	if(command == TOFin_ZCmd) myDetector->SetTOFin_Z(TOFin_ZCmd->GetNewDoubleValue(newValue));
  if(command == Gondola_AngleCmd) myDetector->SetGondola_Angle(Gondola_AngleCmd->GetNewDoubleValue(newValue));
	if(command == Atmos_ZCmd) myDetector->SetAtmos_Z(Atmos_ZCmd->GetNewDoubleValue(newValue));
	if(command == CheckOverlapCmd) global.CheckOverlap = (CheckOverlapCmd->GetNewIntValue(newValue));
	if(command == UpdateCmd) myDetector->UpdateGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
