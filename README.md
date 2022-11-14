# Geant-simulation
For GRAMS simulation toy model. 

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
source /opt/geant4/geant4-11.0.3-install/bin/geant4.sh
```
## Saving time tips
There are ways to prevent you from keeping typing same command every time. One way is to activate shell environment automatically when you open a new terminal. This will slow down the computational speed. But it's less concern if you don't have a long term plan. You could run these command to setup auto activation
```bash
echo 'cd /opt/geant4/geant4-11.0.3-install/bin/' >> ~/.bashrc
echo 'source geant4.sh' >> ~/.bashrc
echo 'cd /opt/root/root_install/bin/' >> ~/.bashrc
echo 'source thisroot.sh' >> ~/.bashrc
echo 'cd' >> ~/.bashrc 
```

You could also use bash_profile to keep track of the shell file directory. If you do this
```bash
echo 'export ROOTsh=/opt/root/root_install/bin/thisroot.sh' >> ~/.bash_profile
echo 'export Geant4sh=/opt/geant4/geant4-11.0.3-install/bin/geant4.sh' >> ~/bash_profile
source ~./bash_profile
```
You should be able to directly use string ROOTsh and Geant4sh to call the directory instead of typing them out everytime.
```bash
source $ROOTsh
source $Geant4sh
```
You only need one of these two solutions. Please choose the one that you like better!

## install toy model
For the first time that you open the toy model or the first time after you change something in the src folder, you need to reinstall the toy model. Go to local GRAMS toy model directory and we will install the model in build directory.
```bash
cd build
cmake ../
make
```
After these command you should see GRAMS successfully installed as showed below. If there is error, please check the error messege and contact me if you don't know how to fix it.
<img width="468" alt="Screen Shot 2022-11-14 at 3 18 19 PM" src="https://user-images.githubusercontent.com/37788723/201757448-d12d6070-5822-418d-b84e-517f615f0731.png">


## Install anyenv/pyenv 
> brew install pyenv
> 
> echo 'export PYENV_ROOT="$HOME/.pyenv"' >> ~/.zshrc
> 
> echo 'export PATH="$PYENV_ROOT/bin:$PATH"' >> ~/.zshrC
> 
> echo -e 'if command -v pyenv 1>/dev/null 2>&1; then\n  eval "$(pyenv init --path)" \n  eval "$(pyenv init -)"\nfi' >> ~/.zshrc 

## Install miniforge 
> pyenv install miniforge** (most recent version)
> 
> pyenv global miniforge** (most recent version)

## Install ROOT
> conda install ROOT 

## Install GEANT4 
do not reinstall cmake when you finish set up geant4 which will cause error. 
> brew install cmake qt5
> 
> sudo mkdir /opt/geant4
> 
> sudo chown your-username:staff /opt/geant4
> 
> cd /opt/geant4
> 
> mv ~/Downloads/geant4.10.07.p01.tar.gz .
> 
> tar zxvf geant4.10.07.p01.tar.gz
> 
> mkdir geant4.10.07.p01-build
> 
> cd geant4.10.07.p01-build
> 
> cmake -DCMAKE_INSTALL_PREFIX=/opt/geant4/geant4.10.07.p01-install -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_QT=OFF /opt/geant4/geant4.10.07.p01/
> 
> make -j8
> 
> make install
> 
> ln -s /opt/geant4/geant4.10.07.p01-install /opt/geant4/pro
> 
> echo 'cd /opt/geant4/pro/bin/' >> ~/.zshrc
> 
> echo 'source geant4.sh' >> ~/.zshrc
> 
> echo 'cd' >> ~/.zshrc

## No need below if you didn’t turn on qt during the installation 
> echo 'export PATH="/opt/homebrew/opt/qt5/bin:$PATH"' >> ~/.zshrc
> 
> echo 'export LDFLAGS="-L/opt/homebrew/opt/qt5/lib":$LDFLAGS' >> ~/.zshrc
> 
> echo 'export CPPFLAGS="-I/opt/homebrew/opt/qt5/include":$CPPFLAGS' >> ~/.zshrc
> 
> echo 'export PKG_CONFIG_PATH="/opt/homebrew/opt/qt5/lib/pkgconfig":$PKG_CONFIG_PATH' >> ~/.zshrc

## Run example（not necessary）
> mkdir g4work  
> 
> cd g4work
> 
> cp -r /opt/geant4/pro/share/Geant4-10.7.1/examples .
> 
> cd example/basic/B1
> 
> mkdir B1-build
> 
> cd B1-build
> 
> cmake ../B1
> 
> make
> 
> ./exampleB1

## Run Original
> cd Original/build
> 
> cmake ../
> 
> make
> 
> cd ..
> 
> build/Original macro/vis.mac

## Visualization (see files in Teams/Labsetup). Download/Install Java8. Download and move HepRApp to /Application
> echo 'alias HepRApp="java -jar /Applications/HepRApp.jar"' >> ~/.zshrc
> 
> HepRApp G4Data0.heprep

## Install gerbv 
> brew install gerbv
> 
> gerbv

 

 

 
