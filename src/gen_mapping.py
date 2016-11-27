#!/usr/bin/env python

#############################################################################################################################
##                                                                                                                         ##
##  GMESSI (Gmsh to Real-ESSI Translator) Simulator Utility                                                                ##
##  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -                                      ##
##                                                                                                                         ##
##  gmESSI is a powerful hybrid python-based translator that takes gmessi as well as python commands                       ##
##  and translates the mesh built in gmsh to ESSI inpute .fei files.                                                       ##
##  linked in lib folder.                                                                                                  ##
##                                                                                                                         ##
##  GitHub         :: https://github.com/SumeetSinha/gmESSI                                                                ##
##  Sublime Plugin :: gmESSI-Tools                                                                                         ##
##                                                                                                                         ##
##  Sumeet Kumar Sinha (November,2016)                                                                                     ##
##  Computational Geomechanics Group                                                                                       ##
##  University of California, Davis                                                                                        ##
##  s u m e e t k s i n h a . c o m                                                                                        ##
#############################################################################################################################

import sys, os, os.path
import numpy as np
import plistlib
import simplejson

# # removing old snippet and keywords
os.system("rm -r mapping.fei")
os.system("rm -r gmESSI_Command_Library.pdf")


# # updating snippets and keywords
os.system("svn export --force https://github.com/BorisJeremic/Real-ESSI/branches/sumeet/DSL/GmessiDocs/mapping.fei") 
os.system("svn export --force https://github.com/BorisJeremic/Real-ESSI/branches/sumeet/DSL/gmESSI_Command_Library.pdf") 
