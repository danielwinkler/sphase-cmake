# gpuSPHASE
Smoothed Particle Hydrodynamics Activated Sludge Engine is a 2D SPH solver for CUDA capable devices.
This software is part of a bigger package devoted to the simulation of biochemical processes coupled to fluid dynamics, information on the distribution of the full package are below.

## Overview

### User manual

The gpuSPHASE user manual is available as
* [PDF](http://iut-ibk.github.io/gpusphase/documentation/userguide.pdf)
* [HTML](https://github.com/iut-ibk/gpusphase/blob/gh-pages/documentation/userguide.pdf)

### Source code documentation

Source code is documented inline and a external documentation is created with doxygen.
The documentation is available at [DOC](http://iut-ibk.github.io/gpusphase/doxygen/html/index.html).

### Citation

Please refer to the code if you use it in a paper with reference [[Winkler et al., 2017](http://www.sciencedirect.com/science/article/pii/S0010465516303666)].
This publication is accompanied with the source code of sphase and discusses the implementation and application.

> Daniel Winkler, Michael Meister, Massoud Rezavand, Wolfgang Rauch, gpuSPHASE - A shared memory caching implementation for 2D SPH using CUDA, Computer Physics Communications, Volume 213, April 2017, Pages 165-180, ISSN 0010-4655, https://doi.org/10.1016/j.cpc.2016.11.011.

### Homepage

More information on the SPHASE project is available at https://www.uibk.ac.at/umwelttechnik/research/projects/sphase.html

## Installation

Requirements software
* Build tools  
  Linux: use package tool or install latest version from web (GL only necessary for GUI builds)  
  `sudo apt-get install build-essential`  
  `sudo yum install gcc gcc-c++ make openssl-devel mesa-libGL-devel mesa-libGLU-devel`  
* CMake (>= v3.10)  
  Linux: use package tool or install latest version from web  
  `sudo apt-get install cmake`  
  `sudo yum install cmake3`  
* Qt5 core  
  www.qt.io/download-open-source  
  Windows: install to C:\Qt and add C:\Qt\bin directory to path  
  Linux: use package tool or install latest version from web  
  `sudo apt-get install qt5-default`  
  `sudo yum install qt5-qtbase-devel qt5-qttools-devel qt5-qtconfiguration-devel`
* CUDA Toolkit
  https://developer.nvidia.com/cuda-toolkit  
  Install for your platform, detailed information can be found on the Nvidia website


### Linux

This installation instructions are tested on a Ubuntu 18.04 distribution.
Different Linux versions or distributions might have to adapt the instructions to the specific package control system and software versions.

* gpuSPHASE clone
  * clone repository or download snapshot
  * `git clone --recurse-submodules git@github.com:danielwinkler/sphase-cmake.git`
  * download [ZIP](https://github.com/iut-ibk/gpusphase/raw/gh-pages/mirror/sphase.zip) file
  

* gpuSPHASE build
  * `cd [SOURCE DIRECTORY]`
  * `mkdir build && cd build`
  * `cmake ..` ... will install all necessary dependencies
  * `ccmake ..` ... to configure build (e.g., no GPU architecture)
    * `CMAKE_BUILD_TYPE` ... set to Release or Debug
    * `BUILD_GPU_ARCH` ... find gpu architecture for your card [here](https://developer.nvidia.com/cuda-gpus)
    * `BUILD_GPU_FORCE_INLINES` ... enable if build fails, e.g. ubuntu 16.04 + cuda sdk from ubuntu sources
  * `make -j` ... to build using multiple cores
  * `make install` ... to install executable into bin folder

### Mac OS

See linux instructions

### Windows

* gpuSPHASE clone
  * clone repository or download snapshot
  * `git clone --recurse-submodules git@github.com:danielwinkler/sphase-cmake.git`
  * download [ZIP](https://github.com/iut-ibk/gpusphase/raw/gh-pages/mirror/sphase.zip) file
  

* gpuSPHASE build
  * Use CMake GUI
  * Open source folder
  * Choose empty build folder
  * Configure CMake for your build environment (e.g. Visual Studio VS 20XX)
  * Generate VS solution file
  * Open Visual Studio solution, choose Debug or Release 64 bit and build solution
  

## Run
### Input

Example input files are available in the `data` folder.
More information on the format can be found in the user manual.

### Command line

The following line loads the scene `lobovsky.json` and executes it for a total of 3 seconds, sampling every 0.05 to the H5Part output file. After installation (`make install`) the executable is in the `bin` subfolder.
```
cd bin
./gpusphase -i ../../data/lobovsky.json -t 3.0 -s 0.05
```

## View

The full SPHASE project contains a GUI that is compiled as `sphasegui`.
For standalone CFD simulation we also recommend ParaView, make sure to enable the H5Part module in older versions.
More information can be found in the user manual.
