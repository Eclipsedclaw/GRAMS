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

#ifndef __DEF__GLOBAL__
#define __DEF__GLOBAL__

#include <fstream>

using namespace std;

#include "G4Types.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
class G4LogicalVolume;
class G4VPhysicalVolume;


struct global_struct{
	G4int DetectorType;
	G4int OutputType;
	G4int OutputFormat;
	G4int TrackType;
  G4int TrackEdep;
	G4int StopEvent;
  G4int NbStop;
  G4int NbInflight;
	G4int lStopEvent;
	G4int DetectorVisualization;
	G4int ParticleSource;
	ofstream output;
	ifstream input;
	
	vector<float> inputX,inputY,inputZ,inputPx,inputPy,inputPz,inputE;
	int nLine;
	int runnum;
//	int eventID;
	int ID1, ID2, ID3;
	int eventID, trackID, parentID, particleID, copyNb1, copyNb;
	float time, energy, eDep, px, py, pz, x, y,z, stepLength, TOF0, TOF1, TScore1, TScore2;
	char material[10],parentProcess[30], processName[30];
	int CheckOverlap;
  int NbLArTPC;
  int NbLXeTPC;
  int NbCellL;
  int NbLArCellZ;
  int NbLXeCellZ;
	char outfile[100];
	char outdir[100];
	char infile[100];
	char indir[100];
	double Emin;
	double Emax;
	double area;
	double beamZ;  
  int seed;
};


#endif
