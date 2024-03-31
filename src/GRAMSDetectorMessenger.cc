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

#include "GAPSDetectorMessenger.hh"

#include "GAPSDetectorConstruction.hh"
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

GAPSDetectorMessenger::GAPSDetectorMessenger(GAPSDetectorConstruction* myDet)
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

// Verbose level for GAPSs
  GAPSVerboseCmd = new G4UIcmdWithAnInteger("/myVerbose/GAPS", this );
  GAPSVerboseCmd->SetGuidance("Verbose level of GAPSs");
  GAPSVerboseCmd->SetParameterName("flag",true);
  GAPSVerboseCmd->SetDefaultValue(0);

// Verbose level for Simulation Type, 0 for reference, 1 for grasp, 2 for Pbar/Dbar Stop event, generate only one particle, 3 for Pbar/Dbar Stop event, generate all particles
  SimulationTypeCmd = new G4UIcmdWithAnInteger("/SimulationType", this );
  SimulationTypeCmd->SetGuidance("SimulationType");
  SimulationTypeCmd->SetParameterName("flag",true);
  SimulationTypeCmd->SetDefaultValue(1);
	
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

// Stop Event, 0 for all, 1 for stop event (trackID = 1)
  StopEventCmd = new G4UIcmdWithAnInteger("/StopEvent", this );
  StopEventCmd->SetGuidance("StopEvent");
  StopEventCmd->SetParameterName("flag",true);
  StopEventCmd->SetDefaultValue(0);
	
// General Particle Source, 0 for off (particle gun), 1 for on (GPS) 
  GPSCmd = new G4UIcmdWithAnInteger("/GeneralParticleSource", this );
  GPSCmd->SetGuidance("GeneralParticleSource");
  GPSCmd->SetParameterName("flag",true);
  GPSCmd->SetDefaultValue(0);
	
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
	
// Detector Type
  DetectorTypeCmd = new G4UIcmdWithAnInteger("/DetectorType",this);  
  DetectorTypeCmd->SetGuidance("detector type");
  DetectorTypeCmd->SetParameterName("DetectorType",false);
	DetectorTypeCmd-> SetDefaultValue(0);

// NbOfLayers
  NbOfLayersCmd = new G4UIcmdWithAnInteger("/NbOfLayers",this);  
  NbOfLayersCmd->SetGuidance("number of layers");
  NbOfLayersCmd->SetParameterName("NbOfLayers",false);
	NbOfLayersCmd-> SetDefaultValue(13);
	
// layer space
  LayerSpaceCmd = new G4UIcmdWithADoubleAndUnit("/LayerSpace",this);  
  LayerSpaceCmd->SetGuidance("layer space (cm)");
  LayerSpaceCmd->SetParameterName("LayerSpace",true);
	LayerSpaceCmd->SetRange("LayerSpace > 0."); 
	LayerSpaceCmd->SetDefaultValue(15.0);
	LayerSpaceCmd->SetDefaultUnit("cm");
	
// layer length
  Layer_LCmd = new G4UIcmdWithADoubleAndUnit("/Layer_L",this);  
  Layer_LCmd->SetGuidance("layer length (cm)");
  Layer_LCmd->SetParameterName("Layer_L",true);
	Layer_LCmd->SetRange("Layer_L > 0."); 
	Layer_LCmd->SetDefaultValue(200.0);
	Layer_LCmd->SetDefaultUnit("cm");

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
	
// Si(Li) thickness
  SiLi_ZCmd = new G4UIcmdWithADoubleAndUnit("/SiLi_Z",this);  
  SiLi_ZCmd->SetGuidance("Si(Li) thickness (cm)");
  SiLi_ZCmd->SetParameterName("SiLi_Z",true);
	SiLi_ZCmd->SetRange("SiLi_Z > 0."); 
	SiLi_ZCmd->SetDefaultValue(0.25);
	SiLi_ZCmd->SetDefaultUnit("cm");

// Frame thickness
  Frame_ZCmd = new G4UIcmdWithADoubleAndUnit("/Frame_Z",this);  
  Frame_ZCmd->SetGuidance("Frame thickness (cm)");
  Frame_ZCmd->SetParameterName("Frame_Z",true);
	Frame_ZCmd->SetRange("Frame_Z > 0."); 
	Frame_ZCmd->SetDefaultValue(0.5);
	Frame_ZCmd->SetDefaultUnit("cm");
	
// Atmos thickness
  Atmos_ZCmd = new G4UIcmdWithADoubleAndUnit("/Atmos_Z",this);  
  Atmos_ZCmd->SetGuidance("Atmos thickness (cm)");
  Atmos_ZCmd->SetParameterName("Atmos_Z",true);
	Atmos_ZCmd->SetRange("Atmos_Z >= 0."); 
	Atmos_ZCmd->SetDefaultValue(3.9);
	Atmos_ZCmd->SetDefaultUnit("cm");

// Detector overlap check
  CheckOverlapCmd = new G4UIcmdWithAnInteger("/CheckOverlap", this );
  CheckOverlapCmd->SetGuidance("Verbose level of GAPSs");
  CheckOverlapCmd->SetParameterName("flag",true);
  CheckOverlapCmd->SetDefaultValue(0);
	
// update geometry
	UpdateCmd = new G4UIcmdWithoutParameter("/update",this);
  UpdateCmd->SetGuidance("Update geometry.");
  UpdateCmd->AvailableForStates(G4State_Idle);
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GAPSDetectorMessenger::~GAPSDetectorMessenger()
{
	delete OutDirCmd;
  delete OutFileCmd;
	delete InDirCmd;
  delete InFileCmd;
  delete GunSeedCmd;
  delete GAPSVerboseCmd;
	delete SimulationTypeCmd;
	delete DetectorVisualizationCmd;
	delete DetectorTypeCmd;
	delete NbOfLayersCmd;
	delete OutputTypeCmd;
	delete OutputFormatCmd;
	delete TrackTypeCmd;
  delete TrackEdepCmd;
	delete StopEventCmd;
	delete GPSCmd;
	delete EminCmd;
	delete EmaxCmd;
	delete areaCmd;
	delete LayerSpaceCmd;
	delete Layer_LCmd;
	delete TOFout_ZCmd;
	delete TOFout_LCmd;
	delete TOFout_HCmd;
	delete TOFout_AngleCmd;
	delete TOFin_ZCmd;
	delete SiLi_ZCmd;
	delete Frame_ZCmd;
	delete Atmos_ZCmd;
	delete CheckOverlapCmd;
	delete UpdateCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GAPSDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
 extern global_struct global;
	if(command == OutDirCmd) strcpy(global.outdir, newValue );
	if(command == OutFileCmd) strcpy(global.outfile, newValue );
	if(command == InDirCmd) strcpy(global.indir, newValue );
	if(command == InFileCmd) strcpy(global.infile, newValue );
  if(command == GunSeedCmd) global.seed=(GunSeedCmd->GetNewIntValue(newValue));
  if(command == GAPSVerboseCmd) global.GAPSVerbose = (GAPSVerboseCmd->GetNewIntValue(newValue));
	if(command == SimulationTypeCmd) global.SimulationType = (SimulationTypeCmd->GetNewIntValue(newValue));
	if(command == DetectorVisualizationCmd) global.DetectorVisualization = (DetectorVisualizationCmd->GetNewIntValue(newValue));
	if(command == DetectorTypeCmd) global.DetectorType = (DetectorTypeCmd->GetNewIntValue(newValue));
	if(command == NbOfLayersCmd) myDetector->SetNbOfLayers(NbOfLayersCmd->GetNewIntValue(newValue));
	if(command == OutputTypeCmd) global.OutputType = (OutputTypeCmd->GetNewIntValue(newValue));
	if(command == OutputFormatCmd) global.OutputFormat = (OutputFormatCmd->GetNewIntValue(newValue));
	if(command == TrackTypeCmd) global.TrackType = (TrackTypeCmd->GetNewIntValue(newValue));
  if(command == TrackEdepCmd) global.TrackEdep = (TrackEdepCmd->GetNewIntValue(newValue));
	if(command == StopEventCmd) global.StopEvent = (StopEventCmd->GetNewIntValue(newValue));
	if(command == GPSCmd) global.GPS = (GPSCmd->GetNewIntValue(newValue));
  if(command == EminCmd) global.Emin = (EminCmd->GetNewDoubleValue(newValue)); 
	if(command == EmaxCmd) global.Emax = (EmaxCmd->GetNewDoubleValue(newValue));
	if(command == areaCmd) global.area = (areaCmd->GetNewDoubleValue(newValue));
	if(command == LayerSpaceCmd) myDetector->SetLayerSpace(LayerSpaceCmd->GetNewDoubleValue(newValue));
	if(command == Layer_LCmd) myDetector->SetLayer_L(Layer_LCmd->GetNewDoubleValue(newValue));
	if(command == TOFout_ZCmd) myDetector->SetTOFout_Z(TOFout_ZCmd->GetNewDoubleValue(newValue));
	if(command == TOFout_LCmd) myDetector->SetTOFout_L(TOFout_LCmd->GetNewDoubleValue(newValue));
	if(command == TOFout_HCmd) myDetector->SetTOFout_H(TOFout_HCmd->GetNewDoubleValue(newValue));
	if(command == TOFout_AngleCmd) myDetector->SetTOFout_Angle(TOFout_AngleCmd->GetNewDoubleValue(newValue));
	if(command == TOFin_ZCmd) myDetector->SetTOFin_Z(TOFin_ZCmd->GetNewDoubleValue(newValue));
	if(command == SiLi_ZCmd) myDetector->SetSiLi_Z(SiLi_ZCmd->GetNewDoubleValue(newValue));
	if(command == Frame_ZCmd) myDetector->SetFrame_Z(Frame_ZCmd->GetNewDoubleValue(newValue));
	if(command == Atmos_ZCmd) myDetector->SetAtmos_Z(Atmos_ZCmd->GetNewDoubleValue(newValue));
	if(command == CheckOverlapCmd) global.CheckOverlap = (CheckOverlapCmd->GetNewIntValue(newValue));
	if(command == UpdateCmd) myDetector->UpdateGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
