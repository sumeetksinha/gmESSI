#!/bin/bash

################### gmESSI Program Check ######################
gmessi PresentationExample.msh > logfile
gmessi Foundation.msh  > logfile
gmessi 111.msh  > logfile
gmessi Contact.msh  > logfile
gmessi Dome.msh  > logfile
gmessi example.msh  > logfile
gmessi Example.msh  > logfile
gmessi Example3.msh  > logfile
gmessi Necking.msh  > logfile
#############################################################
echo "Sucessively completed checking of gmESSI Program"
################### Python Module Check ######################
python PresentationExample.py > logfile
python Foundation.py > logfile
python 111.py > logfile
##############################################################
echo "Sucessively checked gmESSI Python Module"
################## Remove the files generated ################
rm -r *_ESSI_Simulation* logfile
##############################################################
echo "Sucessively removed all  ESSI_Simulation folders"
