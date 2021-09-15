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

#include "GRAMSDetectorSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4ios.hh"
#include "global.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GRAMSDetectorSD::GRAMSDetectorSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
//  collectionName.insert(HCname="DetectorCollection");
	collectionName.insert(HCname=name);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GRAMSDetectorSD::~GRAMSDetectorSD(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GRAMSDetectorSD::Initialize(G4HCofThisEvent* HCE)
{
  DetectorCollection = new GRAMSDetectorHitsCollection
													(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if(HCID<0)
  { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
  HCE->AddHitsCollection( HCID, DetectorCollection );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool GRAMSDetectorSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
	extern global_struct global;
  G4double eDep = aStep->GetTotalEnergyDeposit();
	G4double energy = aStep->GetPreStepPoint()->GetKineticEnergy();
	G4int particleID = aStep->GetTrack()->GetDynamicParticle()->GetDefinition()->GetPDGEncoding();
	G4int trackID = aStep->GetTrack()->GetTrackID();
  G4int parentID = aStep->GetTrack()->GetParentID();
	G4ThreeVector Momentum = aStep->GetPostStepPoint()->GetMomentum();
  G4String processName = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  G4String Material = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetMaterial()->GetName();
  
  if(eDep == 0. && global.TrackEdep != 0 && processName == "Transportation") return false;
//	if(edep == 0.) return false;
//	if(eDep == 0. && global.TrackType != 2 && particleID != 0) return false; // track only particles energy deposited
  
//	if(global.TrackType == 1 && trackID != 1) return false; // track only primary particles
  if(global.TrackType == 1 && !(!(aStep->GetTrack()->GetCreatorProcess()))) return false; // track only primary particles
  if(global.TrackType == 2 && parentID != 1) return false; // track only parentID = 1 (particles generated from primary)
  if(global.TrackType == 3 && (parentID > 1)) return false; // track only trackID = 1 or parentID = 1 (primary particles or particles generated from primary)
  GRAMSDetectorHit* newHit = new GRAMSDetectorHit();
  newHit->SetTrackID  (aStep->GetTrack()->GetTrackID());
  newHit->SetTime     (aStep->GetTrack()->GetGlobalTime());
  newHit->SetParticleID   (aStep->GetTrack()->GetDynamicParticle()->GetDefinition()->GetPDGEncoding());
  newHit->SetParentID  (aStep->GetTrack()->GetParentID());
  newHit->SetMomentum (aStep->GetPostStepPoint()->GetMomentum()); // post-step
  // newHit->SetMomentum (aStep->GetPreStepPoint()->GetMomentum()); // pre-step
  newHit->SetEnergy   (aStep->GetPreStepPoint()->GetKineticEnergy()); // pre-step
//		newHit->SetEnergy   (aStep->GetPostStepPoint()->GetKineticEnergy()); // post-step
  newHit->SetVertexPos(aStep->GetTrack()->GetVertexPosition());
  //newHit->SetCopyNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
  //newHit->SetCopyNb1(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1));

  G4String logicName = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
  if (logicName == "LArCell" || logicName == "LXeCell")
  {
    G4int copyCell2 = aStep->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(2);
    G4int copyCell1 = aStep->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(1);
    G4int copyCell0 = aStep->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber();
    newHit->SetCopyNb1(aStep->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(3));
    newHit->SetCopyNb(copyCell2*global.NbCellL*global.NbCellL+copyCell1*global.NbCellL+copyCell0);
    //newHit->SetCopyNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
    //newHit->SetCopyNb1(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1));
    // G4cout << "copy2: " << aStep->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(2) << G4endl;
  }
  else
  {
    newHit->SetCopyNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber());
    newHit->SetCopyNb1(aStep->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(1));
  }

  if(global.OutputType == 1) // simple output mode
  {
    newHit->SetParentProcess("0");
    newHit->SetProcessName("0");
  }
  else // output text
  {
    if(!(aStep->GetTrack()->GetCreatorProcess())) newHit->SetParentProcess("N/A"); // N/A for the manually generated primary particles
    else newHit->SetParentProcess(aStep->GetTrack()->GetCreatorProcess()->GetProcessName());
    newHit->SetProcessName(aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName());
  }
  newHit->SetMaterialName(aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetMaterial()->GetName());
  newHit->SetEdep				(eDep);
  newHit->SetStepLength	(aStep->GetStepLength());
  newHit->SetPos				(aStep->GetPostStepPoint()->GetPosition());
//		if(trackID == 1 && eDep == energy) global.lStopEvent = 1;
  // if(trackID == 1 && Momentum.getX()*Momentum.getX()+Momentum.getY()*Momentum.getY()+Momentum.getZ()*Momentum.getZ() == 0.0)
  if(trackID == 1 && Momentum.getX()*Momentum.getX()+Momentum.getY()*Momentum.getY()+Momentum.getZ()*Momentum.getZ() == 0.0 && Material == "LAr")
  {    
    if(global.lStopEvent == 0)
    {
      if(eDep == energy && energy > 0.0)
      {
        global.NbStop++;
        global.lStopEvent = 1;
      }
      else
      {
        global.NbInflight++;
        global.lStopEvent = 2;
      }
    }
  }
  DetectorCollection->insert( newHit );
  
  //newHit->Print();
  //newHit->Draw();
	
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GRAMSDetectorSD::EndOfEvent(G4HCofThisEvent*)
{
	extern global_struct global;	
  G4int NbHits = DetectorCollection->entries();
  for(G4int i=0; i<NbHits; i++ )
  {
    if(global.StopEvent >= 0)
    {
      if(global.StopEvent == 1)
      {
        if(global.lStopEvent == 1)(*DetectorCollection)[i]->fPrint();
      }
      else if(global.StopEvent == 2)
      {
        if(global.lStopEvent == 1 || global.lStopEvent == 2)(*DetectorCollection)[i]->fPrint();
      }
      else if(global.StopEvent == 3)
      {
        if(global.lStopEvent == 1)(*DetectorCollection)[i]->fPrint();
      }
      else if(global.StopEvent == 4)
      {
        if(global.lStopEvent == 1 || global.lStopEvent == 2)(*DetectorCollection)[i]->fPrint();
      }
      else (*DetectorCollection)[i]->fPrint();
    }
  }
  
  if (verboseLevel>1) { 
    G4cout << "\n-------->Hits Collection: in this event there are " << NbHits 
		<< " hits in the NaI: " << G4endl;
    for (G4int i=0;i<NbHits;i++) (*DetectorCollection)[i]->Print();
  } 
	
	else if (verboseLevel>0) { 
		G4int NbHits = DetectorCollection->entries();
		G4cout << "\n-------->Hits Collection: in this event they are " << NbHits 
		<< " hits in the tracker layers: " << G4endl;
		for (G4int i=0;i<NbHits;i++) (*DetectorCollection)[i]->Print();
	} 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

