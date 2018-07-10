# SPHASE
Smoothed Particle Hydrodynamics Activated Sludge Engine is a 2D SPH solver for CPU and CUDA capable devices.

## Overview

### User manual

The gpuSPHASE user manual is available as
* [PDF](http://iut-ibk.github.io/gpusphase/documentation/userguide.pdf)
* [HTML](https://github.com/iut-ibk/gpusphase/blob/gh-pages/documentation/userguide.pdf)

Instructions for using the ASM routine
* [PDF](http://iut-ibk.github.io/gpusphase/documentation/ASMroutine_instructions.pdf)
* [HTML](https://github.com/iut-ibk/gpusphase/blob/gh-pages/documentation/ASMroutine_instructions.pdf)

### Source code documentation

Source code is documented inline and a external documentation is created with doxygen.
The documentation is available at [DOC](http://iut-ibk.github.io/gpusphase/doxygen/html/index.html).

### Citation

Please refer to the code if you use it in a paper with reference [[Winkler et al., 2017](http://www.sciencedirect.com/science/article/pii/S0010465516303666)].
This publication is accompanied with the source code of sphase and discusses the implementation and application.

> Daniel Winkler, Michael Meister, Massoud Rezavand, Wolfgang Rauch, gpuSPHASE - A shared memory caching implementation for 2D SPH using CUDA, Computer Physics Communications, Volume 213, April 2017, Pages 165-180, ISSN 0010-4655, https://doi.org/10.1016/j.cpc.2016.11.011.

If you refer to the coupling of ASM to SPH please refer to  [[Meister and Rauch, 2016](http://www.sciencedirect.com/science/article/pii/S1364815215300682)]. This publication discusses the distinguishing feature of the the coupling between SPH and process-based biokinetic wastewater treatment models.

> Michael Meister, Wolfgang Rauch, Wastewater treatment modelling with smoothed particle hydrodynamics, Environmental Modelling & Software, Volume 75, January 2016, Pages 206-211, ISSN 1364-8152, https://doi.org/10.1016/j.envsoft.2015.10.010.

### Homepage

More information on the SPHASE project is available at https://www.uibk.ac.at/umwelttechnik/research/projects/sphase.html

## Installation

Requirements software
* Build tools  
  Linux: use package tool or install latest version from web (GL only necessary for GUI builds)  
  `sudo apt-get install build-essential`  
  `sudo yum install gcc gcc-c++ make openssl-devel mesa-libGL-devel mesa-libGLU-devel`  
* CMake  
  Linux: use package tool or install latest version from web  
  `sudo apt-get install cmake`  
  `sudo yum install cmake3`  
* Qt5  
  www.qt.io/download-open-source  
  Windows: install to C:\Qt and add C:\Qt\bin directory to path  
  Linux: use package tool or install latest version from web  
  `sudo apt-get install qt5-default`  
  `sudo yum install qt5-qtbase-devel qt5-qttools-devel qt5-qtconfiguration-devel`
* Boost  
  http://www.boost.org/  
  Linux: use package tool or install latest version from web  
  `sudo apt-get install libboost-all-dev`  
  `sudo yum install boost-devel`  
* CUDA Toolkit
  https://developer.nvidia.com/cuda-toolkit  
  Install for your platform, detailed information can be found on the Nvidia website


### Linux

This installation instructions are tested on a Ubuntu 16.04 distribution.
Different Linux versions or distributions might have to adapt the instructions to the specific package control system and software versions.

* gpuSPHASE clone
  * clone repository or download snapshot
  * `git clone --recursive git@github.com:iut-ibk/sphase.git`
  * download [ZIP](https://github.com/iut-ibk/gpusphase/raw/gh-pages/mirror/sphase.zip) file
  

* gpuSPHASE build
  * `mkdir build && cd build`
  * `cmake ..` ... will install all necessary dependencies (e.g., no GPU `cmake -DBUILD_GPU=OFF ..`)
  * `ccmake ..` ... to configure build
    * `CMAKE_BUILD_TYPE` ... set to Release or Debug
    * `BUILD_GPU_ARCH` ... find gpu architecture for your card [here](https://developer.nvidia.com/cuda-gpus)
    * `BUILD_GPU_FORCE_INLINES` ... enable if build fails, e.g. ubuntu 16.04 + cuda sdk from ubuntu sources
    * `BUILD_***` ... select to build GPU, CPU, GUI version
  * `make -j` ... to build using multiple cores
  * `make install` ... to install executable into bin folder

### Mac OS

See linux instructions

### Windows (install scripts not working yet)

## Run
### Input

Example input files are available in the `examples` folder.
More information on the format can be found in the user manual.

### Command line

The following line loads the scene `myscene.json` and executes it for a total of 3 seconds, sampling every 0.01 to the H5Part output file.
```
./gpusphase -i ./input/myscene.json -t 3.0 -s 0.01
```

## View

SPHASE contains a GUI that is compiled as `sphasegui`.
We also recommend ParaView, make sure to enable the H5Part module.
More information can be found in the user manual.
