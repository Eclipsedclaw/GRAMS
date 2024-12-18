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


# Run GRAMS toy model
We use macro files to control the operation of the simulation such as choosing particle types and energy range. Check macro file [README.md](https://github.com/Eclipsedclaw/GRAMS/tree/main/macro) to see all the different options that you can change. Specificly there are geant4 library [GPS](https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html) that you could use in the macro files to control the behavior of the input event, such as initial angle and generated location.
And the normal simulation running command is to run command below in the source directory
```bash
./GRAMS macro/"YOUR MACRO FILES"
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
You should be able to see the UI interface of HepRApp. You could explore all the function of it!
<img width="1066" alt="Screen Shot 2022-11-14 at 3 35 26 PM" src="https://user-images.githubusercontent.com/37788723/201760546-78b86043-ee4a-42da-a051-f98b9c47b3fe.png">


# Get the newest version
If you want to follow the newest version of this software that has the development feature and analysis code by others, you could type codes below in your source directory(where you downloaded this software from github)
```bash
git pull
cd ../toymodel_work
ccmake .
```
From the UI interface of ccmake, you could hit 'c' to configure your software with newest updates, then follow the instruction showed on screen. Once configured successfully, you should see the option 'generate'. Then you could re-generate GRAMS toy model by press button 'g'. After re-generate the software, you could use make to install it with updated version again.

```bash
make
```
