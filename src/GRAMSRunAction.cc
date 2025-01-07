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

#include "GRAMSRunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
/*
#include "G4ios.hh"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
*/
#include "global.h"
#include <fstream>

using namespace std;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GRAMSRunAction::GRAMSRunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GRAMSRunAction::~GRAMSRunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GRAMSRunAction::BeginOfRunAction(const G4Run* aRun)
{
  extern global_struct global;
  char fname[100];
  global.NbStop = 0;
  global.NbInflight = 0;
	
  ((G4Run *)(aRun))->SetRunID(global.runnum);
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

// Particles in Scoring volume
  if(global.OutputFormat == 0)
  {
    sprintf(fname, "%s/%s.dat", global.outdir, global.outfile );
    G4cout << "Output file: " << fname << G4endl;
    global.output.open (fname);
  }
  if(global.ParticleSource > 2) // for pbar/dbar stop event
  {
    global.inputX.clear();
    global.inputY.clear();
    global.inputZ.clear();
    sprintf(fname, "%s/%s", global.indir, global.infile );
    G4cout << "Input file for primary particles: " << fname << G4endl;
    global.input.open (fname);
    // Read input files for primary particles
    float X,Y,Z;
    int nLine = 0;
    while (global.input.good())
    {
      global.input >> X >> Y >> Z;
      //				cout << X << " " << Y << " " <<  Z << endl;
      global.inputX.push_back(X);
      global.inputY.push_back(Y);
      global.inputZ.push_back(Z);
      nLine++;
    }
    cout << "there are " << nLine << " lines in the input file" << endl;
    global.nLine = nLine;
    global.input.close();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GRAMSRunAction::EndOfRunAction(const G4Run*)
{
	extern global_struct global;
    std::ofstream outfile;
	if(global.OutputFormat == 0) global.output.close();
  if(global.StopEvent == 1)
  {
    if(global.NbStop > 0) 
    {
        G4cout << "Stop Event = " << global.NbStop << G4endl;
        outfile.open("NoS.txt", std::ios_base::app); 
        outfile << global.NbStop << '\t' << global.NbInflight << '\t' << global.Emin << '\n';
        outfile.close();
    }
    if(global.NbInflight > 0) G4cout << "Inflight Event = " << global.NbInflight << G4endl;
  }
	G4cout << "Run end  " << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



