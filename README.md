gmESSI
=========

### Usage

gmESSI is a finite element translator from [gmsh](http://gmsh.info/) (a three-dimensional finite element mesh generator with built-in pre- and post-procESSIng facilities) to [REAL ESSI](http://sokocalo.engr.ucdavis.edu/~jeremic/Real_ESSI_Simulator/)(UC Davis Earthquake-Soil-Structure-Interaction Simmulator). The translator has been developed in C++ language and generates the ```.fei``` files required by ESSI. This translator provides an easy, handy and powerfool pre-procESSIng tool to develop FEA models and interface with various ESSI functionalities. The gmESSI Translator Package consist of the Translator, Python module, a Sublime plugin and its manual. More detailed information about the translator can be read at [gmESSI-Manual](./gmESSI_Manual.pdf)

****The translator is made very general, so that any other FEM program can use it with little tweaks to have their own conversion tool.****

### Installation

#### Building Dependencies

1) Boost and Python

```bash
sudo apt-get install libboost1.48-all-dev
sudo apt-get install build-essential
sudo apt-get install python-dev 
```

2) Octave

```bash
# For ubuntu 12
sudo apt-get install liboctave-pkg-dev 
# For ubuntu 14 onwards 
sudo apt-get install liboctave-dev 
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

Send your comments, bugs, issues and features to add to [Sumeet Kumar Sinha](http://www.sumeetksinha.com) at sumeet.kumar507@gmail.com.
   




