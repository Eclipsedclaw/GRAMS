//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
// $Id: OriginalPhysicsList.cc,v 1.4 2003/06/16 16:47:06 gunter Exp $
//
// @author Tsuguo Aramaki
// @date 2015 March 23
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#include "OriginalPhysicsList.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
OriginalPhysicsList::OriginalPhysicsList():  G4VModularPhysicsList()
{
    
    defaultCutValue = 0.7*CLHEP::mm;
    G4int ver = 1;
    SetVerboseLevel(ver);
    
    // EM Physics
    RegisterPhysics( new G4EmStandardPhysics(ver) );
    
    // Synchroton Radiation & GN Physics
    RegisterPhysics( new G4EmExtraPhysics(ver) );
    
    // Decays
    RegisterPhysics( new G4DecayPhysics(ver) );
    
    // Hadron Elastic scattering
    RegisterPhysics( new G4HadronElasticPhysics(ver) );
    
    // Hadron Physics
    RegisterPhysics( new G4HadronPhysicsQGSP_BERT(ver));
    
    // Stopping Physics
    RegisterPhysics( new G4StoppingPhysics(ver) );
    
    // Ion Physics
    RegisterPhysics( new G4IonPhysics(ver));
    
    // Neutron tracking cut
    RegisterPhysics( new G4NeutronTrackingCut(ver));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
OriginalPhysicsList::~OriginalPhysicsList()
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OriginalPhysicsList::SetCuts()
{
    // Use default cut values gamma and e processes
    SetCutsWithDefault();   
}
