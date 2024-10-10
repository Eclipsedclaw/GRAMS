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
// $Id: OriginalDetectorMessenger.hh,v 1.8 2006/06/29 17:47:33 gunter Exp $
// GEANT4 tag $Name: geant4-09-00 $
//
// @author Tsuguo Aramaki
// @date 2015 March 23
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef OriginalDetectorMessenger_h
#define OriginalDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"
#include "global.h"

class OriginalDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class OriginalDetectorMessenger: public G4UImessenger
{
  public:
    OriginalDetectorMessenger(OriginalDetectorConstruction*);
   ~OriginalDetectorMessenger();
    
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
  private:
    OriginalDetectorConstruction* myDetector;
    G4UIcmdWithAString*	       OutDirCmd;
    G4UIcmdWithAString*	       OutFileCmd;
    G4UIcmdWithAString*	       InDirCmd;
    G4UIcmdWithAString*	       InFileCmd;
    G4UIcmdWithAnInteger*      GunSeedCmd;
    G4UIcmdWithAnInteger*      OutputFormatCmd;
    G4UIcmdWithAnInteger*      TrackTypeCmd;
    G4UIcmdWithAnInteger*      TrackEdepCmd;
    G4UIcmdWithAnInteger*      GPSCmd;
    G4UIcmdWithAnInteger*      DetectorVisualizationCmd;
    G4UIcmdWithAnInteger*      CheckOverlapCmd;
    G4UIcmdWithoutParameter*   UpdateCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

