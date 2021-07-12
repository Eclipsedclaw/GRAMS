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
// $Id: OriginalDetectorSDHit.cc,v 1.10 2006/06/29 17:48:24 gunter Exp $
// GEANT4 tag $Name: geant4-09-00 $
//
// @author Tsuguo Aramaki
// @date 2015 March 23
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "OriginalDetectorHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "global.h"
#include "G4SystemOfUnits.hh"

G4Allocator<OriginalDetectorHit> OriginalDetectorHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OriginalDetectorHit::OriginalDetectorHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OriginalDetectorHit::~OriginalDetectorHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OriginalDetectorHit::OriginalDetectorHit(const OriginalDetectorHit& right)
  : G4VHit()
{
	trackID_   = right.trackID_;
	originalID_   = right.originalID_;
	time_ = right.time_;
	particleID_ = right.particleID_;
	parentID_   = right.parentID_;
	momentum_ = right.momentum_;
	kEnergy_   = right.kEnergy_;
	parentProcess_= right.parentProcess_;
	processName_= right.processName_;
	vertexPos_ = right.vertexPos_;
	copyNb_ = right.copyNb_;
	copyNb1_ = right.copyNb1_;
	materialName_ = right.materialName_;
  edep_      = right.edep_;
	stepLength_      = right.stepLength_;
  pos_       = right.pos_;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const OriginalDetectorHit& OriginalDetectorHit::operator=(const OriginalDetectorHit& right)
{
  trackID_   = right.trackID_;
	time_ = right.time_;
	particleID_ = right.particleID_;
	parentID_   = right.parentID_;
	momentum_ = right.momentum_;
	kEnergy_   = right.kEnergy_;
	parentProcess_= right.parentProcess_;
	processName_= right.processName_;
	vertexPos_ = right.vertexPos_;
	copyNb_ = right.copyNb_;
	copyNb1_ = right.copyNb1_;
	materialName_ = right.materialName_;
  edep_      = right.edep_;
	stepLength_      = right.stepLength_;
  pos_       = right.pos_;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int OriginalDetectorHit::operator==(const OriginalDetectorHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OriginalDetectorHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(pos_);
    circle.SetScreenSize(2.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OriginalDetectorHit::Print()
{
  G4cout << "  trackID_: " << trackID_ << "  time_: " << G4BestUnit(time_,"time_") 
				 << "  particleID_: " << particleID_
				 << "  copyNb_: " << copyNb_
				 << "  copyNb1_: " << copyNb1_
         << "  energy: " << kEnergy_ / MeV
				 << "  energy deposit: " << G4BestUnit(edep_,"Energy")
				 << "  momentum_: " << G4BestUnit(momentum_,"Energy")
				 << "  parent: "<< parentProcess_
				 << "  position: " << G4BestUnit(pos_,"Length") << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OriginalDetectorHit::fPrint()
{
  extern global_struct global;
  global.output
  //                          << global.runnum << "\t"
            << global.eventID << "\t"
            << trackID_ << "\t"
            << parentID_ << "\t"
            << particleID_ << "\t"
            << copyNb1_ << "\t"
            << copyNb_ << "\t"
            << materialName_ << "\t"
            << time_ / ns << "\t"
            << kEnergy_ / MeV << "\t"
            << edep_ / MeV << "\t"
            << parentProcess_ << "\t"
            << processName_ << "\t"
            << momentum_.getX() / MeV << "\t"
            << momentum_.getY() / MeV << "\t"
            << momentum_.getZ() / MeV << "\t"
            //                          << vertexPos_.getX() / cm << "\t"
            //                          << vertexPos_.getY() / cm << "\t"
            //                          << vertexPos_.getZ() / cm << "\t"
            << stepLength_ / cm << "\t"
            << pos_.getX() / cm << "\t"
            << pos_.getY() / cm << "\t"
            << pos_.getZ() / cm << "\n";
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

