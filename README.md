# Geant-simulation
For GRAMS simulation toy model. 

# Download GRAMS toy model
First please use any method you like to download all of the files from this git repository. You could use git clone command if you already got a github account
> git clone https://github.com/Eclipsedclaw/GRAMS.git

If you don't have one, I strongly suggest you creat one. But at the same time you could directly click the code button on the top right and then download a zip file of it.
<img width="416" alt="Screen Shot 2022-11-14 at 2 56 27 PM" src="https://user-images.githubusercontent.com/37788723/201753644-100a27ad-f98e-40bb-9f2a-8c2e1c9d1916.png">


# Install GRAMS toy model


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

 

 

 
