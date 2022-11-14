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

#include "GRAMSPrimaryGeneratorAction.hh"
#include "GRAMSDetectorConstruction.hh"

#include "G4GeneralParticleSource.hh"
#include "G4SPSAngDistribution.hh"
#include "G4SPSEneDistribution.hh"
#include "G4SPSPosDistribution.hh"
#include "G4SingleParticleSource.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"
#include <math.h>
#include "global.h"
#include "G4SystemOfUnits.hh"
//#include "Randomize.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GRAMSPrimaryGeneratorAction::GRAMSPrimaryGeneratorAction(
                                               GRAMSDetectorConstruction* myDC)
:myDetector(myDC)
{
	GeneralParticleSource = new G4GeneralParticleSource();
		G4int n_particle = 1;	
		particleGun = new G4ParticleGun(n_particle);
		G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
		G4ParticleDefinition* particle = particleTable->FindParticle("anti_proton");
		particleGun->SetParticleDefinition(particle);
		particleGun->SetParticleEnergy(10*MeV);
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GRAMSPrimaryGeneratorAction::~GRAMSPrimaryGeneratorAction()
{
	delete GeneralParticleSource;
	delete particleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GRAMSPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
	extern global_struct global;
	if(global.ParticleSource == 1) GeneralParticleSource->GeneratePrimaryVertex(anEvent);
	else
	{
		if(global.ParticleSource == 0) // for particle gun
		{
			G4double lSquare,X,Y,Z,cosTheta,sinTheta,phi,directionX,directionY,directionZ,minE,maxE,kinetic ;  
			lSquare = global.area;
			X = lSquare*(G4UniformRand()-0.5);
			Y = lSquare*(G4UniformRand()-0.5);
		//	Z = (TOFinL_cm*0.5+TOFinZ_cm+TOFspace_cm+TOFoutZ_cm+atmosZ_cm+scoreZ_cm)*1.0*cm;
			Z = global.beamZ;
			particleGun->SetParticlePosition(G4ThreeVector(X,Y,Z));
			
		//		cosTheta = 2.*G4UniformRand() - 1.;
		//	cosTheta = -1.0*G4UniformRand();
			cosTheta = -1.0*sqrt(G4UniformRand());
			phi = CLHEP::twopi*G4UniformRand();
			sinTheta = std::sqrt(1. - cosTheta*cosTheta);
			directionX = sinTheta*std::cos(phi);
			directionY = sinTheta*std::sin(phi);
			directionZ = cosTheta;
			minE = global.Emin;
			maxE= global.Emax ;
			kinetic = G4UniformRand()*(maxE - minE) + minE;
			particleGun->SetParticleEnergy(kinetic);	
			particleGun->SetParticleMomentumDirection(G4ThreeVector(directionX,directionY,directionZ));
			particleGun->GeneratePrimaryVertex(anEvent);
		}
		else if(global.ParticleSource == 2) // for pbar/dbar stop event, generate one particle
		{
			G4double cosTheta,sinTheta,phi,ux,uy,uz,x,y,z;
			x = global.inputX[global.eventID%global.nLine]*cm;
			y = global.inputY[global.eventID%global.nLine]*cm;
			z = global.inputZ[global.eventID%global.nLine]*cm;
			
			particleGun->SetParticlePosition(G4ThreeVector(x,y,z));
			cosTheta = 2.*G4UniformRand() - 1.0;
			phi = CLHEP::twopi*G4UniformRand();
			sinTheta = std::sqrt(1. - cosTheta*cosTheta);
			ux = sinTheta*std::cos(phi),
			uy = sinTheta*std::sin(phi),
			uz = cosTheta;
			particleGun->SetParticleMomentumDirection(G4ThreeVector(ux,uy,uz));
			particleGun->GeneratePrimaryVertex(anEvent);
		}
		else if(global.ParticleSource == 3) // for dbar stop event, generate all particles
		{
			G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
			G4ParticleDefinition* particle;
			G4double cosTheta,sinTheta,phi,ux,uy,uz,x,y,z;
            G4double yield[2] = {0.92,0.701}; // 74, 114 keV
			x = global.inputX[global.eventID%global.nLine]*cm;
			y = global.inputY[global.eventID%global.nLine]*cm;
			z = global.inputZ[global.eventID%global.nLine]*cm;
			
			// dammy e- to find out the initial position
			particle = particleTable->FindParticle("e-");
			particleGun->SetParticleDefinition(particle);
			particleGun->SetParticlePosition(G4ThreeVector(x,y,z));
			particleGun->SetParticleEnergy(1.0*keV);
			particleGun->GeneratePrimaryVertex(anEvent);
			
			particle = particleTable->FindParticle("gamma");
			particleGun->SetParticleDefinition(particle);
			particleGun->SetParticleDefinition(particle);
			particleGun->SetParticlePosition(G4ThreeVector(x,y,z));
			cosTheta = 2.*G4UniformRand() - 1.0;
			phi = CLHEP::twopi*G4UniformRand();
			sinTheta = std::sqrt(1. - cosTheta*cosTheta);
			ux = sinTheta*std::cos(phi),
			uy = sinTheta*std::sin(phi),
			uz = cosTheta;
			particleGun->SetParticleMomentumDirection(G4ThreeVector(ux,uy,uz));
			particleGun->SetParticleEnergy(73.7*keV);
			if(G4UniformRand() <= yield[0]) particleGun->GeneratePrimaryVertex(anEvent);
			
			particle = particleTable->FindParticle("gamma");
			particleGun->SetParticleDefinition(particle);
			particleGun->SetParticlePosition(G4ThreeVector(x,y,z));
			cosTheta = 2.*G4UniformRand() - 1.0;
			phi = CLHEP::twopi*G4UniformRand();
			sinTheta = std::sqrt(1. - cosTheta*cosTheta);
			ux = sinTheta*std::cos(phi),
			uy = sinTheta*std::sin(phi),
			uz = cosTheta;
			particleGun->SetParticleMomentumDirection(G4ThreeVector(ux,uy,uz));
			particleGun->SetParticleEnergy(113.6*keV);
			if(G4UniformRand() <= yield[1]) particleGun->GeneratePrimaryVertex(anEvent);
			
			/*
      particle = particleTable->FindParticle("anti_deuteron");
      particleGun->SetParticleDefinition(particle);
      particleGun->SetParticlePosition(G4ThreeVector(x,y,z));
      particleGun->SetParticleEnergy(1.0*keV);
      particleGun->GeneratePrimaryVertex(anEvent);
      */
      
			particle = particleTable->FindParticle("anti_proton");
			particleGun->SetParticleDefinition(particle);
			particleGun->SetParticlePosition(G4ThreeVector(x,y,z));
			particleGun->SetParticleEnergy(1.0*keV);
			particleGun->GeneratePrimaryVertex(anEvent);
			
//			particle = particleTable->FindParticle("anti_proton");
			particle = particleTable->FindParticle("anti_neutron");
			particleGun->SetParticleDefinition(particle);
			particleGun->SetParticlePosition(G4ThreeVector(x,y,z));
			particleGun->SetParticleEnergy(1.0*keV);
			particleGun->GeneratePrimaryVertex(anEvent);
      
			
		}
		else if(global.ParticleSource == 4) // for pbar stop event, generate all particles
		{
			G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
			G4ParticleDefinition* particle;
            G4double cosTheta,sinTheta,phi,ux,uy,uz,x,y,z;
            G4double yield[2] = {0.782,0.837}; // 58.2 keV, 96.5 keV
			
			x = global.inputX[global.eventID%global.nLine]*cm;
			y = global.inputY[global.eventID%global.nLine]*cm;
			z = global.inputZ[global.eventID%global.nLine]*cm;
			
			// dammy e- to find out the initial position
			particle = particleTable->FindParticle("e-");
			particleGun->SetParticleDefinition(particle);
			particleGun->SetParticlePosition(G4ThreeVector(x,y,z));
			particleGun->SetParticleEnergy(1.0*keV);
			particleGun->GeneratePrimaryVertex(anEvent);
			
			particle = particleTable->FindParticle("gamma");
			particleGun->SetParticleDefinition(particle);
			particleGun->SetParticleDefinition(particle);
			particleGun->SetParticlePosition(G4ThreeVector(x,y,z));
			cosTheta = 2.*G4UniformRand() - 1.0;
			phi = CLHEP::twopi*G4UniformRand();
			sinTheta = std::sqrt(1. - cosTheta*cosTheta);
			ux = sinTheta*std::cos(phi),
			uy = sinTheta*std::sin(phi),
			uz = cosTheta;
			particleGun->SetParticleMomentumDirection(G4ThreeVector(ux,uy,uz));
			particleGun->SetParticleEnergy(58.2*keV);
			if(G4UniformRand() <= yield[0]) particleGun->GeneratePrimaryVertex(anEvent);
			
			particle = particleTable->FindParticle("gamma");
			particleGun->SetParticleDefinition(particle);
			particleGun->SetParticlePosition(G4ThreeVector(x,y,z));
			cosTheta = 2.*G4UniformRand() - 1.0;
			phi = CLHEP::twopi*G4UniformRand();
			sinTheta = std::sqrt(1. - cosTheta*cosTheta);
			ux = sinTheta*std::cos(phi),
			uy = sinTheta*std::sin(phi),
			uz = cosTheta;
			particleGun->SetParticleMomentumDirection(G4ThreeVector(ux,uy,uz));
			particleGun->SetParticleEnergy(96.5*keV);
			if(G4UniformRand() <= yield[1]) particleGun->GeneratePrimaryVertex(anEvent);
			
			particle = particleTable->FindParticle("anti_proton");
			particleGun->SetParticleDefinition(particle);
			particleGun->SetParticlePosition(G4ThreeVector(x,y,z));
			particleGun->SetParticleEnergy(1.0*keV);
			particleGun->GeneratePrimaryVertex(anEvent);
					
		}
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

