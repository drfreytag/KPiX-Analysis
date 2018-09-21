# KPiX-Analysis

Desciption: analysis code for R&D use on the LYCORIS telescope project.

### Notice:
--------------
* in .gitignore: in order to ignore the symlink files, please do
```
find . -type l | sed -e s'/^\.\///g' >> .gitignore
```

### Prerequisite:
--------------

In order to run this package, you will need:
* libkpix.so produced from the kpix DAQ: https://github.com/Lycoris2017/KPiX-Lycoris.git
* you need to either configure the LD_LIBRARY_PATH to have the 'make install'ed kpix package to be used for this package; or you can
```
mkdir lib
cp $KPIX-DAQ/libkpix.so lib/
```

### Basic setup:
--------------

#### Add the central KPiX-Daq repository to get the master branch:

```
git clone https://github.com/Lycoris2017/KPiX-Analysis.git kpix_ana
cd kpix_ana
```

#### (Option) As developper:

If you are a developper of this software, please click the 'Fork' on top right of this page. Then add your mirror and push your 'dev' branch to it:

```
git remote add myana git@github.com:$YOUR_GITHUB_REPOSITORY/KPiX-Analysis.git
git checkout -b local.dev
git commit -m "init commit, copy from central master branch"
git push -u myana local.dev
```

Collaboration ATTENTION! Please always push to your own branch, then to ask for a pull-request on the central git repository!

#### Compile with CMake

```
mkdir build/; cd build/
cmake ..
make install
```

#### Usage:

If you run an analysis on Acquire binary:
```
bin/analysis acquire.bin
```

If you run ananalysis on Calibration binary:
```
bin/new_CalibrationFitter calib.xml calib.bin
```

If you want the fancy ecal map:
```
cd python
python ecal_mapping.py my_ecal_acquire.bin.root -k 30
```

If you want the coloful strips :
```
cd python
python strip_mapping.py my_strip.bin.root
```

If you want to produce the so-called DA map (the 1024 channels in a 32*(4*8) matrix to activate/deactivate their internal trigger), based on the slope value, pedestal RMS and even hot channels during acquire noise run:
```
python python/channel_disabler.py --calib calib.bin.root --acquire acquire.bin.root -k 26 28
```

or for ecal:
```
python python/channel_disabler.py --calib calib.bin.root --acquire acquire.bin.root -k 30 --ecal True
```

#### Branch descriptions:

* master: up-to-date latest development version.


