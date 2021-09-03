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

#include "GRAMSDetectorHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "global.h"
#include "G4SystemOfUnits.hh"

G4Allocator<GRAMSDetectorHit> GRAMSDetectorHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GRAMSDetectorHit::GRAMSDetectorHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GRAMSDetectorHit::~GRAMSDetectorHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GRAMSDetectorHit::GRAMSDetectorHit(const GRAMSDetectorHit& right)
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

const GRAMSDetectorHit& GRAMSDetectorHit::operator=(const GRAMSDetectorHit& right)
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

G4int GRAMSDetectorHit::operator==(const GRAMSDetectorHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GRAMSDetectorHit::Draw()
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

void GRAMSDetectorHit::Print()
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

void GRAMSDetectorHit::fPrint()
{
  extern global_struct global;
	if(global.OutputFormat == 0)
	{
		if(global.StopEvent == 3)
    {
      if(copyNb_ == -12000) // Score1
      {
        if(global.TScore1 == 0.0 || global.TScore1 > time_ || (momentum_.getX()*momentum_.getX()+momentum_.getY()*momentum_.getY()+momentum_.getZ()*momentum_.getZ() == 0.0))
        {
          global.output << global.eventID << "\t" << trackID_ << "\t" << parentID_ << "\t" << particleID_ << "\t" << copyNb1_ << "\t" << copyNb_ << "\t" << materialName_ << "\t" << time_ / ns << "\t" << kEnergy_ / MeV << "\t" << edep_ / MeV << "\t" << parentProcess_ << "\t" << processName_ << "\t" << momentum_.getX() / MeV << "\t" << momentum_.getY() / MeV << "\t" << momentum_.getZ() / MeV << "\t" << stepLength_ / cm << "\t" << pos_.getX() / cm << "\t" << pos_.getY() / cm << "\t" << pos_.getZ() / cm << "\n";
          global.TScore1 = time_;
        }
      }
      else if(copyNb_ == -12001) // Score1
      {
        if(global.TScore2 == 0.0 || global.TScore2 > time_ || (momentum_.getX()*momentum_.getX()+momentum_.getY()*momentum_.getY()+momentum_.getZ()*momentum_.getZ() == 0.0))
        {
          global.output << global.eventID << "\t" << trackID_ << "\t" << parentID_ << "\t" << particleID_ << "\t" << copyNb1_ << "\t" << copyNb_ << "\t" << materialName_ << "\t" << time_ / ns << "\t" << kEnergy_ / MeV << "\t" << edep_ / MeV << "\t" << parentProcess_ << "\t" << processName_ << "\t" << momentum_.getX() / MeV << "\t" << momentum_.getY() / MeV << "\t" << momentum_.getZ() / MeV << "\t" << stepLength_ / cm << "\t" << pos_.getX() / cm << "\t" << pos_.getY() / cm << "\t" << pos_.getZ() / cm << "\n";
          global.TScore2 = time_;
        }
      }
      if(copyNb_ == -10000 || copyNb_ == -10002 ||copyNb_ == -10003 ||copyNb_ == -10004 || copyNb_ == -10005) // TOF out
      {
        if(global.TOF0 == 0.0 || global.TOF0 > time_ || (momentum_.getX()*momentum_.getX()+momentum_.getY()*momentum_.getY()+momentum_.getZ()*momentum_.getZ() == 0.0))
        {
          global.output << global.eventID << "\t" << trackID_ << "\t" << parentID_ << "\t" << particleID_ << "\t" << copyNb1_ << "\t" << copyNb_ << "\t" << materialName_ << "\t" << time_ / ns << "\t" << kEnergy_ / MeV << "\t" << edep_ / MeV << "\t" << parentProcess_ << "\t" << processName_ << "\t" << momentum_.getX() / MeV << "\t" << momentum_.getY() / MeV << "\t" << momentum_.getZ() / MeV << "\t" << stepLength_ / cm << "\t" << pos_.getX() / cm << "\t" << pos_.getY() / cm << "\t" << pos_.getZ() / cm << "\n";
          global.TOF0 = time_;
        }
      }
      else if(copyNb_ == -11000 || copyNb_ == -11001 || copyNb_ == -11002 ||copyNb_ == -11003 ||copyNb_ == -11004 || copyNb_ == -11005) // TOF in
      {
        if(global.TOF1 == 0.0 || global.TOF1 > time_ || (momentum_.getX()*momentum_.getX()+momentum_.getY()*momentum_.getY()+momentum_.getZ()*momentum_.getZ() == 0.0))
        {
          global.output << global.eventID << "\t" << trackID_ << "\t" << parentID_ << "\t" << particleID_ << "\t" << copyNb1_ << "\t" << copyNb_ << "\t" << materialName_ << "\t" << time_ / ns << "\t" << kEnergy_ / MeV << "\t" << edep_ / MeV << "\t" << parentProcess_ << "\t" << processName_ << "\t" << momentum_.getX() / MeV << "\t" << momentum_.getY() / MeV << "\t" << momentum_.getZ() / MeV << "\t" << stepLength_ / cm << "\t" << pos_.getX() / cm << "\t" << pos_.getY() / cm << "\t" << pos_.getZ() / cm << "\n";
          global.TOF1 = time_;
        }
      }
      else if(momentum_.getX()*momentum_.getX()+momentum_.getY()*momentum_.getY()+momentum_.getZ()*momentum_.getZ() == 0.0)
      {
        global.output << global.eventID << "\t" << trackID_ << "\t" << parentID_ << "\t" << particleID_ << "\t" << copyNb1_ << "\t" << copyNb_ << "\t" << materialName_ << "\t" << time_ / ns << "\t" << kEnergy_ / MeV << "\t" << edep_ / MeV << "\t" << parentProcess_ << "\t" << processName_ << "\t" << momentum_.getX() / MeV << "\t" << momentum_.getY() / MeV << "\t" << momentum_.getZ() / MeV << "\t" << stepLength_ / cm << "\t" << pos_.getX() / cm << "\t" << pos_.getY() / cm << "\t" << pos_.getZ() / cm << "\n";
      }
    }
    else
    {
      global.output
  //													<< global.runnum << "\t"
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
													<< stepLength_ / cm << "\t"
													// << vertexPos_.getX() / cm << "\t"
													// << vertexPos_.getY() / cm << "\t"
													// << vertexPos_.getZ() / cm << "\t"

													<< pos_.getX() / cm << "\t"
                          << pos_.getY() / cm << "\t"
                          << pos_.getZ() / cm << "\n";
    }
  }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

