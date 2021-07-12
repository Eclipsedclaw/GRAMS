  
//	Original Detector Simulation Program 	GSIM
//	FHG 20 OCT 2003
//
// @author Tsuguo Aramaki
// @date 2015 March 23
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
	G4int SimulationType;	
	G4int OutputType;
	G4int OutputFormat;
	G4int TrackType;
    G4int TrackEdep;
	G4int DetectorVisualization;
	G4int DetectorType;
	G4int GPS;
	ofstream output;
	ifstream input;
	
	vector<float> inputX,inputY,inputZ,inputPx,inputPy,inputPz,inputE;
	int nLine;
	int runnum;
	int ID1, ID2, ID3;
	int eventID, trackID, parentID, particleID, copyNb1, copyNb;
	float time, energy, eDep, px, py, pz, x, y,z, stepLength;
	char material[10],parentProcess[30], processName[30];
	int CheckOverlap;
	char outfile[100];
	char outdir[100];
	char infile[100];
	char indir[100];
    G4int seed;
};


#endif
