
# // ***********************************Foundation Mesh Physical Groups ********************************
# // 2 1 "$NegY_Soil_Contact$"
# // 2 2 "$NegX_Soil_Contact$"
# // 2 3 "$PosY_Soil_Contact$"
# // 2 4 "$PosX_Soil_Contact$"
# // 2 5 "$Top_Soil_Contact$"
# // 2 6 "$NegY_Foundation_Contact$"
# // 2 7 "$NegX_Foundation_Contact$"
# // 2 8 "$PosY_Foundation_Contact$"
# // 2 9 "$PosX_Foundation_Contact$"
# // 2 10 "$Bottom_Foundation_Contact$"
# // 2 12 "$Raised_Platform_Upper_Surface$"
# // 2 14 "$SoilUpperSurface$"
# // 3 11 "$Foundation_Volume$"
# // 3 13 "$Raised_Platform_Volume$"
# // 3 15 "$Soil_Volume$ "
# // *****************************************************************************************************


import gmessi

GmshConverter = gmessi.gmESSIPython()

GmshConverter.loadMshFile("Foundation.msh")

Inputfile = open('Foundation.gmessi', 'r')

for line in Inputfile:
	line = line.partition('//')[0]
	line = line.rstrip()
	if (line):
		GmshConverter.Convert(line)

x = GmshConverter.LoadFile

ElementList = GmshConverter.getSphereSelectionData("Physical_Group#12",5,0,0,5).ElementList
NodeList = GmshConverter.getSphereSelectionData("Physical_Group#12",5,0,0,5).NodeList
# NodeList = GmshConverter.getSphereSelectionData("All",1.5,0,0,0).NodeList

GmshConverter.CreatePhysicalGroup("HelloWorld",NodeList,ElementList)
GmshConverter.Convert("[Vary_8NodeBrick_SurfaceLoad{Physical_Group#13,Physical_Group#HelloWorld,10*z+abs(x)\\0\Pa}]")

x = """ ewfkhrehf nwehfrewf sndfhndjus  sdh sdfh sdhf sd
dksjcsdjc
sdjckljsdlcv
jsdicfjsdol]
sljfcklsdf
jlsdjcfklsdcf
fh """

GmshConverter.DisplayNewTagNumbering()


GmshConverter.Convert("[Add_Node_Load_Linear{Physical_Group#1,Fx,10*N}]")
GmshConverter.Convert("Add_20NodeBrick_elastic{Physical_Group#1,1}")
GmshConverter.Convert("[Add_Element_Self_Weight{Physical_Group#1,1}]")

GmshConverter.pwd
GmshConverter.GmshFile
GmshConverter.GeometryFile
GmshConverter.LoadFile
GmshConverter.MainFile
