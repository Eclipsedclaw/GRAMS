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
#include <filesystem>
#include "TFile.h"
#include "TTree.h"

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

  // create the output directory if it does not exist (otherwise nothing would be written)
  if(global.OutputFormat == 0 || global.OutputFormat == 1)
  {
    std::error_code ec;
    std::filesystem::create_directories(global.outdir, ec);
  }

// Particles in Scoring volume
  if(global.OutputFormat == 0)
  {
    sprintf(fname, "%s/%s.dat", global.outdir, global.outfile );
    G4cout << "Output file: " << fname << G4endl;
    global.output.open (fname);
  }
  if(global.OutputFormat == 1)
  {
    G4String rootName = G4String(global.outdir) + "/" + global.outfile + ".root";
    G4cout << "Output file: " << rootName << G4endl;
    // ZLIB (algorithm 1) level 4: readable by every ROOT 6 version and by uproot without extra packages
    global.rootFile = new TFile(rootName.c_str(), "RECREATE", "GRAMS simulation output", 104);
    if(global.rootFile->IsZombie())
    {
      G4ExceptionDescription msg;
      msg << "Cannot open output file " << rootName;
      G4Exception("GRAMSRunAction::BeginOfRunAction()", "GRAMSOutput", FatalException, msg);
    }
    // same columns and units as the .dat file
    global.hitTree = new TTree("hits", "GRAMS hits (time ns, energy MeV, momentum MeV/c, length cm)");
    global.hitTree->Branch("eventID", &global.eventID, "eventID/I");
    global.hitTree->Branch("trackID", &global.trackID, "trackID/I");
    global.hitTree->Branch("parentID", &global.parentID, "parentID/I");
    global.hitTree->Branch("particleID", &global.particleID, "particleID/I");
    global.hitTree->Branch("copyNb1", &global.copyNb1, "copyNb1/I");
    global.hitTree->Branch("copyNb", &global.copyNb, "copyNb/I");
    global.hitTree->Branch("material", &global.material);
    global.hitTree->Branch("time", &global.time, "time/F");
    global.hitTree->Branch("energy", &global.energy, "energy/F");
    global.hitTree->Branch("eDep", &global.eDep, "eDep/F");
    global.hitTree->Branch("parentProcess", &global.parentProcess);
    global.hitTree->Branch("processName", &global.processName);
    global.hitTree->Branch("px", &global.px, "px/F");
    global.hitTree->Branch("py", &global.py, "py/F");
    global.hitTree->Branch("pz", &global.pz, "pz/F");
    global.hitTree->Branch("stepLength", &global.stepLength, "stepLength/F");
    global.hitTree->Branch("x", &global.x, "x/F");
    global.hitTree->Branch("y", &global.y, "y/F");
    global.hitTree->Branch("z", &global.z, "z/F");
    // No automatic AutoSave by ROOT: it deletes the tree header that the last checkpoint
    // in GRAMSEventAction points to, which leaves a killed job with an unreadable file
    global.hitTree->SetAutoSave(0);
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

void GRAMSRunAction::EndOfRunAction(const G4Run* aRun)
{
	extern global_struct global;
    std::ofstream outfile;
	if(global.OutputFormat == 0) global.output.close();
  if(global.OutputFormat == 1 && global.rootFile)
  {
    // take the file from the tree: above 100 GB ROOT continues the tree in a new file
    // <OutputFile>_1.root and deletes the first one
    TFile* rootFile = global.hitTree->GetCurrentFile();
    // "run" tree with one entry: number of generated events (the hits tree only
    // has the events that passed the selection) and the primary particle settings
    rootFile->cd();
    TTree* runTree = new TTree("run", "GRAMS run summary (Emin, Emax MeV, area cm)");
    G4int nEvents = aRun->GetNumberOfEvent();
    G4int nStop = global.NbStop;
    G4int nInflight = global.NbInflight;
    G4double Emin = global.Emin / MeV;
    G4double Emax = global.Emax / MeV;
    G4double area = global.area / cm;
    runTree->Branch("nEvents", &nEvents, "nEvents/I");
    runTree->Branch("nStop", &nStop, "nStop/I");
    runTree->Branch("nInflight", &nInflight, "nInflight/I");
    runTree->Branch("Emin", &Emin, "Emin/D");
    runTree->Branch("Emax", &Emax, "Emax/D");
    runTree->Branch("area", &area, "area/D");
    runTree->Fill();

    G4int nFiles = global.hitTree->GetFileNumber() + 1;
    if(nFiles == 1) G4cout << "Wrote " << global.hitTree->GetEntries() << " hits of " << nEvents
                           << " events to " << rootFile->GetName() << G4endl;
    else G4cout << "Wrote the hits of " << nEvents << " events to " << nFiles
                << " files, the last one (with the run tree) is " << rootFile->GetName() << G4endl;
    rootFile->Write("", TObject::kOverwrite); // replaces the periodic AutoSave copy of "hits"
    rootFile->Close(); // also deletes the trees
    delete rootFile;
    global.rootFile = nullptr;
    global.hitTree = nullptr;
  }
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



