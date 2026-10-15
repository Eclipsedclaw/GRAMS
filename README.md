# Geant-simulation
For GRAMS simulation toy model. If you are going through this the first time, please navigate to section **Download GRAMS toy model**.

# Download GRAMS toy model
First please use any method you like to download all of the files from this git repository. You could use git clone command if you already got a github account
```bash
git clone https://github.com/Eclipsedclaw/GRAMS.git
```

If you don't have one, I strongly suggest you creat one. But at the same time you could directly click the code button on the top right and then download a zip file of it.

<img width="416" alt="Screen Shot 2022-11-14 at 2 56 27 PM" src="https://user-images.githubusercontent.com/37788723/201753644-100a27ad-f98e-40bb-9f2a-8c2e1c9d1916.png">


# Install GRAMS toy model

first we need to make sure root and geant4 are properly working. Specificly for Aramaki Lab's ubuntu machine, you could use following command to activate root and geant4
```bash
source /opt/root/root_install/bin/thisroot.sh
```

```bash
source /opt/geant4/geant4-11.1.0-install/bin/geant4.sh
```
## Saving time tips
There are ways to prevent you from typing same commands every time. One way is to activate shell environment automatically when you open a new terminal. This will slow down the speed and potentially have some unknown shell conflict issues. But it's less concern if you don't have a long term plan. You could run these command to setup auto activation
```bash
echo 'source /opt/geant4/geant4-11.1.0-install/bin/geant4.sh' >> ~/.bashrc
echo 'source /opt/root/root_install/bin/thisroot.sh' >> ~/.bashrc
echo 'cd' >> ~/.bashrc 
source ~/.bashrc
```

You could also use .profile to keep track of the shell file directory. If you do this
```bash
echo 'export ROOTsh=/opt/root/root_install/bin/thisroot.sh' >> ~/.profile
echo 'export Geant4sh=/opt/geant4/geant4-11.1.0-install/bin/geant4.sh' >> ~/.profile
```

```bash
source ~/.profile
```
You should be able to directly use string ROOTsh and Geant4sh to call the directory instead of typing them out everytime.
```bash
source $ROOTsh
source $Geant4sh
```
You only need one of these two solutions. Please choose the one that you like better!

## install toy model
For the first time that you open the toy model or the first time after you change something in the source folder, you need to reinstall the toy model. Go to local GRAMS toy model directory and we will install the model in another build directory. Here I suggest to have another installation directory aside from the source directory. You should run the following commands in the parent directory of GRAMS git directory. You could do
```bash
cd ..
```
to go to the parent directory.

Then you could make a work directory and install GRAMS toymodel using commands below
```bash
mkdir toymodel_work && cd toymodel_work
cmake ../GRAMS
make
```
After these commands you should see GRAMS successfully installed as showed below. If there is error, please check the error messege and contact me if you don't know how to fix it.

<img width="468" alt="Screen Shot 2022-11-14 at 3 18 19 PM" src="https://user-images.githubusercontent.com/37788723/201757448-d12d6070-5822-418d-b84e-517f615f0731.png">

ROOT is needed to build GRAMS, because the simulation writes its output as ROOT files. If cmake stops with `ROOT not found`, ROOT is not activated in that terminal: run the `source .../thisroot.sh` command above and run cmake again.


# Run GRAMS toy model
We use macro files to control the operation of the simulation such as choosing particle types and energy range. Check macro file [README.md](https://github.com/Eclipsedclaw/GRAMS/tree/main/macro) to see all the different options that you can change. Specificly there are geant4 library [GPS](https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html) that you could use in the macro files to control the behavior of the input event, such as initial angle and generated location.
And the normal simulation running command is to run command below in the build directory (toymodel_work)
```bash
./GRAMS macro/"YOUR MACRO FILES"
```

For example, the anti-proton simulation for the GRASP study is
```bash
./GRAMS macro/GRASP_anti_proton/anti_proton_all.mac
```
It runs all 15 energy bins (0 to 750 MeV in 50 MeV steps, 1e7 events each) one after the other in one job and writes one output file per bin. This used to be split into `anti_proton_1.mac` ... `anti_proton_15.mac` because the memory of a job kept growing with the number of events. That memory leak is fixed now, so one job can run as many events as you need.

If you add new macro files, run cmake again in the build directory. cmake copies the `macro` folder into the build directory, and only when it runs.

## Run several jobs at the same time
You could also run several jobs in parallel, for example one per energy bin. Then every job needs a different random seed in its macro, otherwise jobs with the same settings produce exactly the same events:
```
/random/setSeeds 7 12345
```
`/gun/seed` does not do anything. `anti_proton_all.mac` already sets a different seed for every bin.

## Output
Each run (each `/run/beamOn`) writes `<OutputFile>.root` into `/OutputDirectory`. The directory is created if it does not exist yet. The ROOT file has two trees:
- `hits`: one entry per recorded step, with the same columns and units as the old `.dat` files (eventID, trackID, parentID, particleID, ..., x, y, z)
- `run`: one entry with the number of generated events `nEvents` (use it for normalization, the `hits` tree only has the events that were written) and the primary particle settings

`/OutputFormat 0` in a macro still writes the old `.dat` text file instead. Most of the existing macros still have this line, so change it to `/OutputFormat 1` to get ROOT files.

The ROOT file is saved every 5 minutes during the run, so if a job is killed you still have the data up to the last save. Output files (`.root`, `.dat`) are ignored by git, please don't commit them.

See [docs/output.md](docs/output.md) for all the branches and their units, the copy numbers, more ways to read the files, and how to run large or parallel jobs.

## Read the output
In Python, `analysis/2023_Paper/toy_model_analysis.py` reads a ROOT file into the same pandas DataFrame as the old `.dat` reader, so the existing analysis code still works. It needs uproot
```bash
pip install uproot
```
```python
import toy_model_analysis

df = toy_model_analysis.read_root_from_toymodel("output/anti_proton/GRASP_anti_proton_1e7_1.root")
n_generated = toy_model_analysis.read_run_info_from_toymodel("output/anti_proton/GRASP_anti_proton_1e7_1.root")['nEvents']
```
You could also look at a file directly with ROOT
```bash
root -l output/anti_proton/GRASP_anti_proton_1e7_1.root
```
```
root [1] hits->Draw("energy", "trackID==1 && material==\"LAr\"")
```

## visulization
There is a vis.mac file under macro directory, you could use this to generate heprep files. For default setting you could run
```bash
./GRAMS macro/vis.mac
```
This will generate heprep files, G4data0 usually just represents for empty geometry without any events. And to actually see the simulation visulization we need HepRApp. Run this command to link HepRapp to a simple format
```bash
echo 'alias HepRApp="java -jar /opt/HepRApp/HepRApp.jar"' >> ~/.bashrc
source ~/.bashrc
```
Then you should be able to call HepRApp in the source folder by simply typing
```bash
HepRApp G4Data0.heprep
```
You should be able to see the UI interface of HepRApp. You could explore all the function of it! Visualization keeps the events in memory, so only use it for a few events, not for large runs.
<img width="1066" alt="Screen Shot 2022-11-14 at 3 35 26 PM" src="https://user-images.githubusercontent.com/37788723/201760546-78b86043-ee4a-42da-a051-f98b9c47b3fe.png">


# Get the newest version
If you downloaded GRAMS before October 2026, download it again with `git clone` as in **Download GRAMS toy model**, then run cmake in your build directory again. The history of this repository was cleaned up then (old data files were removed), so `git pull` stops with `refusing to merge unrelated histories` for those older copies.

If you want to follow the newest version of this software that has the development feature and analysis code by others, you could type codes below in your source directory(where you downloaded this software from github)
```bash
git pull
cd ../toymodel_work
ccmake .
```
From the UI interface of ccmake, you could hit 'c' to configure your software with newest updates, then follow the instruction showed on screen. Once configured successfully, you should see the option 'generate'. Then you could re-generate GRAMS toy model by press button 'g'. This also copies new or changed macro files into the build directory. After re-generate the software, you could use make to install it with updated version again.

```bash
make
```
