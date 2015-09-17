
import gmessi

GmshConverter = gmessi.gmESSIPython()

GmshConverter.loadMshFile("111.msh")

Inputfile = open('111.gmessi', 'r')

for line in Inputfile:
	GmshConverter.Convert(line)

ElementList = GmshConverter.getSphereSelectionData("Physical_Group#1",0.5,1.5,0.5,0.5).ElementList
NodeList = GmshConverter.getSphereSelectionData("Physical_Group#1",0.5,1.5,0.5,0.5).NodeList
NodeList = GmshConverter.getSphereSelectionData("All",1.5,0,0,0).NodeList

GmshConverter.CreatePhysicalGroup("HelloWorld",NodeList,ElementList)

GmshConverter.UpdateGmshFile()


GmshConverter.Convert("[Add_Node_Load_Linear{Physical_Group#1,Fx,10*N}]")
GmshConverter.Convert("Add_20NodeBrick_elastic{Physical_Group#1,1}")
GmshConverter.Convert("[Add_Element_Self_Weight{Physical_Group#1,1}]")

GmshConverter.pwd
GmshConverter.GmshFile
GmshConverter.GeometryFile
GmshConverter.LoadFile
GmshConverter.MainFile
