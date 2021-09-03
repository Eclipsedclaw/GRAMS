GRAMS Geometrical information
@author Tsuguo Aramaki
@date 2015 March 23

Air Column: 3.9g/cm2

Detector
Layer size: 2mx2m
Frame (hexagon) thickness: 5mm
SiLi thickness: 2mm/3mm/4mm
Pipe
TOF thickness: 2mm

Source 
Area: 20mx20m
Angular distribution: cos_theta^2 = random
Energy distribution: 0-0.5GeV, 0-0.25GeV

// output format
eventID
trackID
parentID
particleID
copyNb1 (mother volume)
copyNb
material
time (ns)
energy (MeV)
energy deposit (MeV)
parent process name
process name
px (MeV/c)
py (MeV/c)
pz (MeV/c)
step length (cm)
x (cm)
y (cm)
z (cm)

// copy number
 copySiLi = 0;
 copyLayer = 0;
 copyFrame = 1000;
 copyTOFout = 10000;
 copyTOFin = 11000;
 copyScore1 = 12000;
 copyScore2 = 12001; // reference plane
 copyAtmosphere = 12002;
 copyOutlet = 12003;
 copyPipe = 12004;
 copyElectronics = 12005;
 copyWorld = -1;
