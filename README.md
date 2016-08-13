gmESSI
=========

### Usage

gmESSI is a finite element translator from [gmsh](http://gmsh.info/) (a three-dimensional finite element mesh generator with built-in pre- and post-procESSIng facilities) to [REAL ESSI](http://sokocalo.engr.ucdavis.edu/~jeremic/Real_ESSI_Simulator/)(UC Davis Earthquake-Soil-Structure-Interaction Simmulator). The translator has been developed in C++ language and generates the ```.fei``` files required by ESSI. This translator provides an easy, handy and powerfool pre-procESSIng tool to develop FEA models and interface with various ESSI functionalities. The gmESSI Translator Package consist of the Translator, Python module, a Sublime plugin and its manual. More detailed information about the translator can be read at [gmESSI-Manual](./gmESSI_Manual.pdf)

### Installation

#### Building Dependencies

1) Boost

```bash
sudo apt-get install boost-all-dev
```

2) Octave

```bash
# Building Octave dependencies
sudo apt-get install libpcre3 libpcre3-dev
sudo apt-get build-dep octave

# Compiling Octave
wget ftp://ftp.gnu.org/gnu/octave/octave-4.0.3.tar.gz  (or any other newer version)
tar -xvf octave-4.0.3.tar.gz
mv octave-4.0.3 Octave
cd Octave

# comment out all the warnings of file 
# ./libinterp/octave-value/ov-typeinfo.cc

# configure octave build

./configure CPPFLAGS=-I/usr/include/hdf5/serial 
LDFLAGS=-L/usr/lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH)/hdf5/serial --enable-jit 
make -j 8
make check
make install PREFIX=/usr/local
```

3) Python

```bash
sudo apt-get install build-essential
sudo apt-get install boost-all-dev
sudo apt-get install python-dev 
sudo apt-get install libqt4-core libqt4-gui qt4-qmake libxt-dev 
sudo apt-get install g++ gcc cmake-curses-gui libqt4-opengl-dev 
sudo apt-get install mesa-common-dev python-dev
```

#### Compiling gmESSI translator

1) Get the latest version of gmESSI from github

```bash
git clone https://github.com/SumeetSinha/gmESSI.git
```

2) Compile 

```bash
cd gmESSI
make -j ${nop}
make install
```

#### Installing Sublime plugins

1) gmsh-Tools :: [.msh and .geo]

	a) Open Sublime 
	b) Go to Preference/Package Control
	c) Select Add Repository
	d) Enter the plugin github repo https://github.com/SumeetSinha/gmsh-Tools
	e) Again, open Preferences/Package Control
	f) Select Install Package
	g) Search for gmsh-Tools and install it.


2) gmESSI-Tools:: [.gmessi]

	a) Open Sublime 
	b) Go to Preference/Package Control
	c) Select Add Repository
	d) Enter the plugin github repo https://github.com/SumeetSinha/gmESSI-Tools
	e) Again, open Preferences/Package Control
	f) Select Install Package
	g) Search for gmESSSI-Tools and install it.

#### Running Examples 

1) Run an example to check the correctness of installation


---
[UCD CompGeoMech](http://sokocalo.engr.ucdavis.edu/~jeremic/)

Created by: [Sumeet Kumar Sinha](http://www.sumeetsinha.in)

Request for adding features on sumeet.kumar507@gmail.com
   




