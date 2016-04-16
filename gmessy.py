#!/usr/bin/python

import sys
import os
import re
import time
import argparse
import string

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


Number_Of_Arguments = len(sys.argv);

# if (Number_Of_Arguments==1):
# 	print bcolors.WARNING + "Please give a Input File Name xyz.gmessi or xyz.msh"+ bcolors.ENDC;
# 	sys.exit();



parser = argparse.ArgumentParser()
parser.add_argument('-l', help='generate the log file at the current location',action="store_true")
parser.add_argument('-nm', help='generate the node map file at the current location',action="store_true")
parser.add_argument('-em', help='generate the element map file at the current location',action="store_true")
parser.add_argument('-ne', help='dont carry out the conversion',action="store_true")
parser.add_argument('--logfile', help='generate the log file at specified location', type=str, dest='log_file')
parser.add_argument('--nodemap=', help='generate the nodemap file at specified location', type=str, dest='nm_file')
parser.add_argument('--elemap=', help='generate the elementmap file at specified location', type=str, dest='elm_file')
parser.add_argument('gmessi_filename', help='filename containing semantics of conversion', type=str)


parsed = parser.parse_args()   
l=parsed.l; nm=parsed.nm; em=parsed.em; ne=parsed.ne;
Temp_GmESSI_File=parsed.gmessi_filename; 
Temp_Input_File=parsed.log_file; 
Temp_Node_Map_File=parsed.nm_file;
Temp_Element_Map_File=parsed.elm_file;

try:

	Temp_GmESSI_File = os.path.realpath(Temp_GmESSI_File);
	Temp_Input_File = os.path.dirname(Temp_GmESSI_File) + "/GmESSI_CONVERTOR_"+(os.path.basename(Temp_GmESSI_File)).replace(".gmessi","")+"_Log.py";
	
	# print Temp_Input_File;
	# print Temp_GmESSI_File;
	if(not os.path.exists(Temp_GmESSI_File)):
		print bcolors.FAIL + "ERROR: File Does Not Exist"+ bcolors.ENDC;
		sys.exit()

	INPUT_FILE = open(Temp_Input_File, 'w');
	GmESSI_FILE = open(Temp_GmESSI_File, 'r');

	regexp = re.compile(r'\[(\w)*{(.)*}\s*\]');
	gmESSI_Command =0;gmESSI_Comment =0;

	INPUT_FILE.write("import sys\n");
	INPUT_FILE.write("import gmessi\n");
	INPUT_FILE.write("import os\n");
	INPUT_FILE.write("from math import *\n");

	INPUT_FILE.write("\n \nGmESSI = gmessi.gmESSIPython() \n");

	for newline in GmESSI_FILE:
		line=newline.lstrip();
		line=line.rstrip();
		if len(line)==0:
			if(gmESSI_Command==1):
				INPUT_FILE.write("\nANALYSIS_FILE = open(GmESSI.MainFile,'a');");
				INPUT_FILE.write("ANALYSIS_FILE.write("+"\'\\n\'"+");");
				INPUT_FILE.write("ANALYSIS_FILE.close();\n");
			continue;
		elif line.startswith("//"):
			INPUT_FILE.write("\n" +"#" +line);
			INPUT_FILE.write("\nANALYSIS_FILE = open(GmESSI.MainFile,'a');");
			INPUT_FILE.write("ANALYSIS_FILE.write(\""+(line.replace("\\","\\\\")).replace("\"","\\\"")+"\\n\");");
			INPUT_FILE.write("ANALYSIS_FILE.close();\n");
			continue;
		elif line.startswith("#"):
			INPUT_FILE.write("\n" +line+"\n");
			gmESSI_Command=0;
			continue;
		elif regexp.search(line) is not None:
			INPUT_FILE.write("GmESSI.Convert(\""+(line.replace("\\","\\\\")).replace("\"","\\\"")+"\")\n");
			gmESSI_Command=1;
			continue;
		if (gmESSI_Command==0):
			INPUT_FILE.write(newline+"\n");
		else:
			INPUT_FILE.write("\n"+newline.replace("\"","\\\"")+"\n");
		gmESSI_Command=0;

	INPUT_FILE.close();

	if(not(ne)):
		execfile(Temp_Input_File)

	if(not(l)):
		os.remove(Temp_Input_File)

	# if(nm):
	# 	NodeMap = GmESSI.getNodeMap();
	# 	NodeNoMap = GmESSI.getNodeNoMap();
	# 	f = open("SurfaceLoad_Gmsh_To_ESSI_Node_Mapping",'w');
	# 	for x in NodeNoMap:
	# 		f.write(str(x)+"\n")
	# 	f.close();
	# if(em):
	# 	os.remove(Temp_Input_File)	

except (RuntimeError, TypeError, NameError):
	print bcolors.FAIL + "ERROR: "+newline+ bcolors.ENDC;
