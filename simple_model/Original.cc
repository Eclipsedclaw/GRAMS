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
// $Id: exampleN02.cc,v 1.12 2006/06/29 17:47:25 gunter Exp $
// GEANT4 tag $Name: geant4-09-00 $
//
// @author Tsuguo Aramaki
// @date 2015 March 23
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "OriginalDetectorConstruction.hh"
#include "OriginalPhysicsList.hh"
#include "OriginalPrimaryGeneratorAction.hh"
#include "OriginalRunAction.hh"
#include "OriginalEventAction.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

//#include "QGSP.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BERT_HP.hh"
#include "QGSP_BIC.hh"
#include "QGSP_BIC_HP.hh"
//#include "LHEP_BERT.hh"
#include "Shielding.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "global.h"


global_struct global;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Run manager
  //
  G4RunManager * runManager = new G4RunManager;

  // User Initialization classes (mandatory)
  //
  OriginalDetectorConstruction* detector = new OriginalDetectorConstruction;
  runManager->SetUserInitialization(detector);
  // CLHEP::HepRandom::setTheSeed((unsigned)clock()); // for random seeds
  //

//  G4VUserPhysicsList* physics = new PhysicsList;
//  runManager->SetUserInitialization(physics);
	
//	G4VUserPhysicsList* physics = new QGSP;
//	G4VUserPhysicsList* physics = new QGSP_BERT_HP;
//	G4VUserPhysicsList* physics = new QGSP_BIC_HP;
//	G4VUserPhysicsList* physics = new LHEP_BERT;
//	physics->SetDefaultCutValue(0.01*mm) ;

//	G4VUserPhysicsList* physics = new OriginalPhysicsList;
	G4VUserPhysicsList* physics = new QGSP_BERT;
//  G4VUserPhysicsList* physics = new QGSP_BIC;
//  G4VUserPhysicsList* physics = new Shielding;
  
	physics->SetDefaultCutValue(1.0*mm) ;
	runManager->SetUserInitialization(physics);

  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
   
  // User Action classes
  //
  G4VUserPrimaryGeneratorAction* gen_action = new OriginalPrimaryGeneratorAction();
  runManager->SetUserAction(gen_action);
  //
  G4UserRunAction* run_action = new OriginalRunAction;
  runManager->SetUserAction(run_action);
  //
  G4UserEventAction* event_action = new OriginalEventAction;
  runManager->SetUserAction(event_action);
  // Initialize G4 kernel
  //
  runManager->Initialize();
      
  // Get the pointer to the User Interface manager
  //
  G4UImanager * UI = G4UImanager::GetUIpointer();  
  G4UIExecutive* session = 0;
  if(argc==1)  // Define (G)UI terminal for interactive mode
  { 
    // G4UIterminal is a (dumb) terminal
    //
    session = new G4UIExecutive(argc, argv);
    UI->ApplyCommand("/control/execute");    
    session->SessionStart();
    delete session;
  }
  else   // Batch mode
  { 
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UI->ApplyCommand(command+fileName);
  }

  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !

  delete visManager;

  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

