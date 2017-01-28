import sys
import gmessi
import os
from math import *

 
GmESSI = gmessi.gmESSIPython() 
GmESSI.loadMshFile("Example_2.msh")


#0 1 "All_Points"

#1 3 "All_Lines"

#2 2 "All_Surfaces"

#2 5 "ApplySurfaceLoad"

#2 6 "SurfaceToBeFixed"

#3 4 "All_Volumes"

#// My new model
ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write("// My new model\n");ANALYSIS_FILE.close();
GmESSI.Convert("[Define_Model_Name{ModelName:= \"Cantilever_Analysis\"}]")

ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write('\n');ANALYSIS_FILE.close();
GmESSI.Convert("[Add_All_Node{Unit:= m, NumDofs:= 3}]")

ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write('\n');ANALYSIS_FILE.close();

#// Adding Material
ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write("// Adding Material\n");ANALYSIS_FILE.close();

ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write('\n');ANALYSIS_FILE.close();
GmESSI.Convert("[Add_Linear_Elastic_Isotropic_3D{MaterialNo:= 1, Density:= 2000*kg/m^3, ElasticModulus:= 200*MPa, PoissonRatio:= 0.2}]")
GmESSI.Convert("[Add_8NodeBrick{Physical_Group#All_Volumes, material_no:= 1}]")

ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write('\n');ANALYSIS_FILE.close();

#// Applying Fixities
ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write("// Applying Fixities\n");ANALYSIS_FILE.close();
GmESSI.Convert("[Fix_Dofs{Physical_Group#SurfaceToBeFixed, all}]")

ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write('\n');ANALYSIS_FILE.close();
GmESSI.Convert("[Include{\"Example_2_geometry.fei\"}]")

ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write('\n');ANALYSIS_FILE.close();

[New_Loading\_Stage{StageName:= \"Stage1_Uniform_Surface_Load\"}]


# Adding Surface Load

#[Add_8NodeBrick_SurfaceLoad{Physical_Group#All_Volumes,Physical_Group#ApplySurfaceLoad,-10*Pa}]
GmESSI.Convert("[Add_Node_Load_Linear{Physical_Group#ApplySurfaceLoad, ForceType:= Fz, Mag:= -10*kN}]")

ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write('\n');ANALYSIS_FILE.close();
GmESSI.Convert("[Include{\"Example_2_load.fei\"}]")

ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write('\n');ANALYSIS_FILE.close();
GmESSI.Convert("[Comment{Starting the simulation}]")

ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write('\n');ANALYSIS_FILE.close();
GmESSI.Convert("[Define_Algorithm{With_no_convergence_check}]")
GmESSI.Convert("[Define_Solver{SolverName:= UMFPack}]")
GmESSI.Convert("[Define_Load_Factor_Increment{Incr:= 1}]")
GmESSI.Convert("[Simulate_Using_Static_Algorithm{NumSteps:= 10}]")

ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write('\n');ANALYSIS_FILE.close();
GmESSI.Convert("[Bye{}]")

ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write('\n');ANALYSIS_FILE.close();

ANALYSIS_FILE = open(GmESSI.MainFile,'a');ANALYSIS_FILE.write('\n');ANALYSIS_FILE.close();
